#pragma GCC optimize("O3")
#include "FileManager.h"

#include <dirent.h>
#include <errno.h>
#include <esp_task_wdt.h>
#include <sd_diskio.h>
#include <sys/stat.h>

#include <cctype>

#include "pixeler/setup/sd_setup.h"
#include "pixeler/util/AutoLock.h"

#define IDLE_WD_GUARD_TIME 250U

namespace pixeler
{
  String FileManager::makeFullPath(const char* path)
  {
    String full_path = SD_MOUNTPOINT;
    full_path += path;
    return full_path;
  }

  String FileManager::makeUniqueFilename(const String& file_path)
  {
    uint16_t counter = 1;
    String temp_path = file_path;
    String unique_filename = file_path;
    while (fileExist(unique_filename.c_str(), true))
    {
      unique_filename = temp_path.substring(0, temp_path.lastIndexOf("."));
      unique_filename += "(";
      unique_filename += counter;
      unique_filename += ")";
      unique_filename += temp_path.substring(temp_path.lastIndexOf("."));
      ++counter;
    }

    return unique_filename;
  }

  FileManager::FileManager() : _sd_mutex{xSemaphoreCreateMutex()}
  {
  }

  uint8_t FileManager::getEntryTypeUnlocked(const char* path, dirent* entry)
  {
    if (entry && entry->d_type != DT_UNKNOWN)
      return entry->d_type;
    else
    {
      struct stat st;
      if (stat(path, &st) == 0)
      {
        if (S_ISREG(st.st_mode))
          return DT_REG;
        else if (S_ISDIR(st.st_mode))
          return DT_DIR;
      }

      return DT_UNKNOWN;
    }
  }

  size_t FileManager::getFileSize(const char* path)
  {
    AutoLock lock(_sd_mutex);
    return getFileSizeUnlocked(path);
  }

  size_t FileManager::getFileSizeUnlocked(const char* path)
  {
    String full_path = makeFullPath(path);
    struct stat st;
    if (stat(full_path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
      return 0;

    return static_cast<size_t>(st.st_size);
  }

  bool FileManager::readStat(struct stat& out_stat, const char* path)
  {
    String full_path = makeFullPath(path);

    AutoLock lock(_sd_mutex);
    if (stat(full_path.c_str(), &out_stat) != 0)
      return false;

    return true;
  }

  bool FileManager::fileExist(const char* path, bool silently)
  {
    String full_path = makeFullPath(path);

    AutoLock lock(_sd_mutex);
    bool result = getEntryTypeUnlocked(full_path.c_str()) == DT_REG;

    if (!result && !silently)
      log_e("File %s not found", full_path.c_str());

    return result;
  }

  bool FileManager::dirExist(const char* path, bool silently)
  {
    String full_path = makeFullPath(path);

    AutoLock lock(_sd_mutex);
    bool result = getEntryTypeUnlocked(full_path.c_str()) == DT_DIR;

    if (!result && !silently)
      log_e("Dir %s not found", full_path.c_str());

    return result;
  }

  bool FileManager::exists(const char* path, bool silently)
  {
    String full_path = makeFullPath(path);
    AutoLock lock(_sd_mutex);
    uint8_t type = getEntryTypeUnlocked(full_path.c_str());

    if (type == DT_REG || type == DT_DIR)
      return true;

    log_e("[ %s ] not exist", full_path.c_str());
    return false;
  }

  bool FileManager::createDir(const char* path)
  {
    String full_path = makeFullPath(path);

    errno = 0;

    AutoLock lock(_sd_mutex);
    bool result = !mkdir(full_path.c_str(), 0777);

    if (!result)
    {
      log_e("Помилка створення директорії: %s", full_path.c_str());
      if (errno == EEXIST)
        log_e("Директорія існує");
    }

    return result;
  }

  size_t FileManager::readFile(const char* path, void* out_buffer, size_t len, int32_t seek_pos)
  {
    String full_path = makeFullPath(path);

    AutoLock lock(_sd_mutex);
    FILE* f = fopen(full_path.c_str(), "rb");

    if (!f)
    {
      log_e("Помилка відкриття файлу: %s", full_path.c_str());
      return 0;
    }

    if (seek_pos > 0 && fseek(f, seek_pos, SEEK_SET))
    {
      log_e("Помилка встановлення позиції(%d) у файлі %s", seek_pos, full_path.c_str());
      fclose(f);
      return 0;
    }

    size_t bytes_read = fread(out_buffer, 1, len, f);

    if (bytes_read != len)
      log_e("Прочитано: [ %zu ]  Очікувалося: [ %zu ]", bytes_read, len);

    fclose(f);
    return bytes_read;
  }

  bool FileManager::readChunkFromFile(FILE* file, void* out_buffer, size_t len, size_t seek_pos)
  {
    if (len == 0)
      return false;

    if (!file)
    {
      log_e("FILE* не повинен бути null");
      return false;
    }

    AutoLock lock(_sd_mutex);
    if (seek_pos > 0 && fseek(file, seek_pos, SEEK_SET))
    {
      log_e("Помилка встановлення позиції: %zu", seek_pos);
      return false;
    }

    size_t section_read = fread(out_buffer, len, 1, file);

    if (section_read == 0)
    {
      log_e("Не вдалося прочитати всі %zu байтів", len);
      return false;
    }

    return true;
  }

  size_t FileManager::readFromFile(FILE* file, void* out_buffer, size_t len, size_t seek_pos)
  {
    if (!file)
    {
      log_e("FILE* не повинен бути null");
      return 0;
    }

    if (len == 0)
      return 0;

    AutoLock lock(_sd_mutex);
    if (seek_pos > 0 && fseek(file, seek_pos, SEEK_SET))
      return 0;

    size_t bytes_read = fread(out_buffer, 1, len, file);

    if (bytes_read != len)
      log_e("Прочитано: [ %zu ]  Очікувалося: [ %zu ]", bytes_read, len);

    return bytes_read;
  }

  size_t FileManager::writeFile(const char* path, const void* buffer, size_t len)
  {
    if (len == 0)
      return 0;

    if (!path || !buffer)
    {
      log_e("Bad arguments");
      return 0;
    }

    String full_path = makeFullPath(path);

    AutoLock lock(_sd_mutex);
    FILE* f = fopen(full_path.c_str(), "wb");

    if (!f)
    {
      log_e("Помилка відркиття файлу: %s", full_path.c_str());
      return 0;
    }

    size_t written = writeOptimalUnlocked(f, buffer, len);

    fclose(f);

    return written;
  }

  size_t FileManager::writeToFile(FILE* file, const void* buffer, size_t len)
  {
    if (len == 0)
      return 0;

    if (!file || !buffer)
    {
      log_e("Bad arguments");
      return 0;
    }

    AutoLock lock(_sd_mutex);
    return writeOptimalUnlocked(file, buffer, len);
  }

  size_t FileManager::writeOptimalUnlocked(FILE* file, const void* buffer, size_t len)
  {
    size_t opt_size = 256;
    size_t total_written = 0;

    size_t full_blocks = len / opt_size;
    size_t remaining_bytes = len % opt_size;

    unsigned long ts = millis();
    for (size_t i = 0; i < full_blocks; ++i)
    {
      total_written += fwrite(static_cast<const uint8_t*>(buffer) + total_written, opt_size, 1, file) * opt_size;
      if (millis() - ts > IDLE_WD_GUARD_TIME)
      {
        delay(1);
        ts = millis();
      }
    }

    if (remaining_bytes > 0)
      total_written += fwrite(static_cast<const uint8_t*>(buffer) + total_written, remaining_bytes, 1, file) * remaining_bytes;

    fflush(file);

    if (total_written != len)
      log_e("Записано: [ %zu ]  Очікувалося: [ %zu ]", total_written, len);

    return total_written;
  }

  FILE* FileManager::openFile(const char* path, const char* mode)
  {
    String full_path = makeFullPath(path);

    AutoLock lock(_sd_mutex);
    FILE* f = fopen(full_path.c_str(), mode);

    if (!f)
      log_e("Помилка взяття дескриптора для %s", full_path.c_str());

    return f;
  }

  void FileManager::closeFile(FILE*& file)
  {
    if (file)
    {
      AutoLock lock(_sd_mutex);
      fclose(file);
      file = nullptr;
    }
  }

  bool FileManager::seekPos(FILE* file, int32_t pos, uint8_t mode)
  {
    if (!file)
      return false;

    AutoLock lock(_sd_mutex);
    if (fseek(file, pos, mode))
    {
      log_e("Помилка встановлення позиції [%d]", pos);
      return false;
    }
    return true;
  }

  size_t FileManager::getPos(FILE* file)
  {
    if (!file)
      return 0;

    AutoLock lock(_sd_mutex);
    return ftell(file);
  }

  size_t FileManager::available(FILE* file, size_t file_size)
  {
    AutoLock lock(_sd_mutex);
    return availableUnlocked(file, file_size);
  }

  size_t FileManager::availableUnlocked(FILE* file, size_t file_size)
  {
    if (!file || feof(file))
      return 0;

    long tell = ftell(file);

    if (file_size < tell)
      return 0;

    return file_size - tell;
  }

  void FileManager::rm()
  {
    bool result = false;

    String full_path = makeFullPath(_rm_path.c_str());

    bool is_dir = dirExist(_rm_path.c_str(), true);

    {
      AutoLock lock(_sd_mutex);
      if (!is_dir)
        result = rmFileUnlocked(full_path.c_str());
      else
        result = rmDirUnlocked(full_path.c_str());
    }

    if (result)
      log_i("Успішно видалено: %s", full_path.c_str());

    taskDone(result);
  }

  bool FileManager::rmFileUnlocked(const char* path, bool make_full)
  {
    bool result;

    if (make_full)
    {
      String full_path = makeFullPath(path);
      result = !remove(full_path.c_str());

      if (!result)
        log_e("Помилка видалення файлу: %s", full_path.c_str());
    }
    else
    {
      result = !remove(path);

      if (!result)
        log_e("Помилка видалення файлу: %s", path);
    }

    return result;
  }

  bool FileManager::rmDirUnlocked(const char* path, bool make_full)
  {
    bool result = false;

    dirent* dir_entry{nullptr};
    DIR* dir;

    if (make_full)
    {
      String full_path = makeFullPath(path);
      dir = opendir(full_path.c_str());
    }
    else
    {
      dir = opendir(path);
    }

    if (!dir)
      goto exit;

    errno = 0;

    while (!_is_canceled)
    {
      dir_entry = readdir(dir);
      if (!dir_entry)
      {
        if (!errno)
          result = true;
        break;
      }

      if (std::strcmp(dir_entry->d_name, ".") == 0 || std::strcmp(dir_entry->d_name, "..") == 0)
        continue;

      String full_path = path;
      full_path += "/";
      full_path += dir_entry->d_name;

      uint8_t entr_type = getEntryTypeUnlocked(full_path.c_str(), dir_entry);

      if (entr_type == DT_REG)
      {
        result = rmFileUnlocked(full_path.c_str());
        if (!result)
          goto exit;
      }
      else if (entr_type == DT_DIR)
      {
        result = rmDirUnlocked(full_path.c_str());

        if (!result)
          goto exit;
      }
      else
      {
        log_e("Невідомий тип або не вдалося прочитати: %s", full_path.c_str());
        goto exit;
      }

      delay(1);
    }

  exit:
    if (dir)
      closedir(dir);

    if (result)
      result = !rmDirUnlocked(path);
    else
      log_e("Помилка під час видалення: %s", path);

    return result;
  }

  bool FileManager::rmFile(const char* path, bool make_full)
  {
    AutoLock lock(_sd_mutex);
    return rmFileUnlocked(path, make_full);
  }

  bool FileManager::rmDir(const char* path, bool make_full)
  {
    AutoLock lock(_sd_mutex);
    return rmDirUnlocked(path, make_full);
  }

  void FileManager::rmTask(void* params)
  {
    FileManager* instance = static_cast<FileManager*>(params);
    instance->_is_working = true;
    instance->rm();
  }

  bool FileManager::startRemoving(const char* path)
  {
    if (_is_working)
    {
      log_e("Вже працює інша задача");
      return false;
    }

    if (!exists(path))
      return false;

    _rm_path = path;
    _is_canceled = false;

    BaseType_t result = xTaskCreatePinnedToCore(rmTask, "rmTask", TASK_SIZE, this, 10, nullptr, 0);

    if (result == pdPASS)
    {
      log_i("rmTask is working now");
      return true;
    }
    else
    {
      log_e("rmTask was not running");
      return false;
    }
  }

  bool FileManager::rename(const char* old_name, const char* new_name)
  {
    if (!exists(old_name))
      return false;

    String old_n = makeFullPath(old_name);
    String new_n = makeFullPath(new_name);

    if (new_n.length() >= old_n.length() &&
        (new_n.c_str()[old_n.length()] == '/' || new_n.c_str()[old_n.length()] == '\0') &&
        strncmp(old_n.c_str(), new_n.c_str(), old_n.length()) == 0)
    {
      log_e("Старе і нове ім'я збігаються або спроба переміщення каталогу до самого себе");
      return false;
    }

    AutoLock lock(_sd_mutex);
    return !::rename(old_n.c_str(), new_n.c_str());
  }

  bool FileManager::copyFileUnlocked(const String& from, const String& to)
  {
    FILE* n_f = fopen(to.c_str(), "ab");

    if (!n_f)
    {
      log_e("Помилка створення файлу: %s", to.c_str());
      return false;
    }

    FILE* o_f = fopen(from.c_str(), "rb");

    if (!o_f)
    {
      log_e("Помилка читання файлу: %s", from.c_str());
      fclose(n_f);
      return false;
    }

    size_t buf_size = 1024;
    uint8_t* buffer;

    if (psramInit())
    {
      buf_size *= 160;
      buffer = static_cast<uint8_t*>(ps_malloc(buf_size));
    }
    else
    {
      buf_size *= 16;
      buffer = static_cast<uint8_t*>(malloc(buf_size));
    }

    if (!buffer)
    {
      fclose(n_f);
      fclose(o_f);

      log_e("Помилка виділення пам'яті: %zu b", buf_size);
      esp_restart();
    }

    size_t file_size = getFileSizeUnlocked(_copy_from_path.c_str());

    if (file_size > 0)
    {
      log_i("Починаю копіювання");
      log_i("З: %s", from.c_str());
      log_i("До: %s", to.c_str());

      size_t writed_bytes_counter{0};
      size_t bytes_read;
      size_t byte_aval = 0;

      while (!_is_canceled && (byte_aval = availableUnlocked(o_f, file_size)) > 0)
      {
        if (byte_aval < buf_size)
          bytes_read = fread(buffer, byte_aval, 1, o_f) * byte_aval;
        else
          bytes_read = fread(buffer, buf_size, 1, o_f) * buf_size;
        //
        writed_bytes_counter += writeOptimalUnlocked(n_f, buffer, bytes_read);
        _copy_progress = ((float)writed_bytes_counter / file_size) * 100;
      }
    }
    else
    {
      _copy_progress = 100;
      delay(50);
    }

    free(buffer);

    fclose(n_f);
    fclose(o_f);

    return true;
  }

  void FileManager::copyFile()
  {
    _copy_to_path = makeUniqueFilename(_copy_to_path);

    String from = makeFullPath(_copy_from_path.c_str());
    String to = makeFullPath(_copy_to_path.c_str());

    bool result;

    {
      AutoLock lock(_sd_mutex);
      result = copyFileUnlocked(from, to);
    }

    if (_is_canceled)
    {
      log_i("Копіювання скасовано: %s", from.c_str());
      rmFileUnlocked(to.c_str());
      taskDone(false);
    }
    else
    {
      if (result)
        log_i("Успішно скопійовано: %s", from.c_str());
      else
        log_i("Невдача копіювання: %s", from.c_str());

      taskDone(result);
    }
  }

  void FileManager::copyFileTask(void* params)
  {
    FileManager* instance = static_cast<FileManager*>(params);
    instance->_is_working = true;
    instance->copyFile();
  }

  bool FileManager::startCopyFile(const char* from, const char* to)
  {
    if (!from || !to)
    {
      log_e("Bad arguments");
      return false;
    }

    if (_is_working)
    {
      log_e("Вже працює інша задача");
      return false;
    }

    if (!fileExist(from))
      return false;

    _copy_from_path = from;
    _copy_to_path = to;

    _is_canceled = false;
    _copy_progress = 0;

    BaseType_t result = xTaskCreatePinnedToCore(copyFileTask, "copyFileTask", TASK_SIZE, this, 10, nullptr, 0);

    if (result == pdPASS)
    {
      log_i("copyFileTask is working now");
      return true;
    }
    else
    {
      log_e("copyFileTask was not running");
      return false;
    }
  }

  void FileManager::index(std::vector<FileInfo>& out_vec, const char* dir_path, IndexMode mode, const char* file_ext)
  {
    out_vec.clear();
    out_vec.reserve(40);

    if (!dirExist(dir_path))
      return;

    String full_path = makeFullPath(dir_path);

    AutoLock lock(_sd_mutex);
    DIR* dir = opendir(full_path.c_str());
    if (!dir)
    {
      log_e("Помилка відкриття директорії %s", full_path.c_str());
      return;
    }

    dirent* dir_entry{nullptr};
    String filename;
    bool is_dir;

    unsigned long ts = millis();

    while (1)
    {
      dir_entry = readdir(dir);
      if (!dir_entry)
        break;

      filename = dir_entry->d_name;

      if (filename.equals(".") || filename.equals(".."))
        continue;

      String full_name{full_path};
      full_name += "/";
      full_name += filename;

      uint8_t entr_type = getEntryTypeUnlocked(full_name.c_str(), dir_entry);

      if (entr_type == DT_REG)
        is_dir = false;
      else if (entr_type == DT_DIR)
        is_dir = true;
      else
        continue;

      switch (mode)
      {
        case INDX_MODE_DIR:
          if (is_dir)
            out_vec.emplace_back(filename, true);
          break;
        case INDX_MODE_FILES:
          if (!is_dir)
            out_vec.emplace_back(filename, false);
          break;
        case INDX_MODE_FILES_EXT:
          if (!is_dir && filename.endsWith(file_ext))
            out_vec.emplace_back(filename, false);
          break;
        case INDX_MODE_ALL:
          if (is_dir)
            out_vec.emplace_back(filename, true);
          else
            out_vec.emplace_back(filename, false);
          break;
      }

      if (millis() - ts > IDLE_WD_GUARD_TIME)
      {
        delay(1);
        ts = millis();
      }
    }

    out_vec.shrink_to_fit();
    std::sort(out_vec.begin(), out_vec.end());

    if (dir)
      closedir(dir);
  }

  void FileManager::indexFilesExt(std::vector<FileInfo>& out_vec, const char* dir_path, const char* file_ext)
  {
    return index(out_vec, dir_path, INDX_MODE_FILES_EXT, file_ext);
  }

  void FileManager::indexFiles(std::vector<FileInfo>& out_vec, const char* dir_path)
  {
    return index(out_vec, dir_path, INDX_MODE_FILES);
  }

  void FileManager::indexDirs(std::vector<FileInfo>& out_vec, const char* dir_path)
  {
    return index(out_vec, dir_path, INDX_MODE_DIR);
  }

  void FileManager::indexAll(std::vector<FileInfo>& out_vec, const char* dir_path)
  {
    return index(out_vec, dir_path, INDX_MODE_ALL);
  }

  void FileManager::taskDone(bool result)
  {
    _is_working = false;

    _last_task_result = result;

    if (_doneHandler)
      _doneHandler(result, _doneArg);

    vTaskDelete(NULL);
  }

  void FileManager::cancel()
  {
    _is_canceled = true;
  }

  void FileManager::setTaskDoneHandler(TaskDoneHandler handler, void* arg)
  {
    _doneHandler = handler;
    _doneArg = arg;
  }

  //------------------------------------------------------------------------------------------------------------------------

  bool FileManager::isMounted() const
  {
    if (_pdrv == 0xFF)
    {
      log_e("Карту пам'яті не примонтовано");
      return false;
    }

    String path_to_root = SD_MOUNTPOINT;
    path_to_root += "/";
    struct stat st;
    AutoLock lock(_sd_mutex);
    if (stat(path_to_root.c_str(), &st) != 0)
    {
      log_e("Помилка читання stat");
      return false;
    }

    return S_ISDIR(st.st_mode);
  }

  bool FileManager::mount(SPIClass* spi)
  {
    if (_pdrv != 0xFF)
      unmount();

    if (!spi || !spi->begin())
    {
      log_e("Некоректна шина SPI або помилка ініціалізації шини");
      return false;
    }

    AutoLock lock(_sd_mutex);
    _pdrv = sdcard_init(SD_PIN_CS, spi, SD_FREQUENCY);
    if (_pdrv == 0xFF)
    {
      log_e("Помилка ініціалізації SD");
      return false;
    }

    if (!sdcard_mount(_pdrv, SD_MOUNTPOINT, SD_MAX_FILES, false))
    {
      sdcard_unmount(_pdrv);
      sdcard_uninit(_pdrv);
      _pdrv = 0xFF;
      log_e("Помилка монтування SD");
      return false;
    }

    delay(10);
    log_i("Карту пам'яті примонтовано");
    return true;
  }

  void FileManager::unmount()
  {
    AutoLock lock(_sd_mutex);
    sdcard_unmount(_pdrv);
    sdcard_uninit(_pdrv);
    _pdrv = 0xFF;
    delay(10);
  }

  //------------------------------------------------------------------------------------------------------------------------

  FileInfo::FileInfo(const String& name, bool is_dir) : _is_dir{is_dir}, _name_len{name.length()}
  {
    if (psramInit())
      _name = static_cast<char*>(ps_malloc(_name_len + 1));
    else
      _name = static_cast<char*>(malloc(_name_len + 1));

    if (!_name)
    {
      log_e("Помилка виділення буферу");
      esp_restart();
    }

    std::memcpy(_name, name.c_str(), _name_len);
    _name[_name_len] = '\0';
  }

  FileInfo::~FileInfo()
  {
    free(_name);
  }

  bool FileInfo::nameEndsWith(const char* suffix) const
  {
    if (!suffix)
      return false;

    size_t suffix_len = strlen(suffix);

    if (_name_len < suffix_len)
      return false;

    return strcmp(_name + _name_len - suffix_len, suffix) == 0;
  }

  bool FileInfo::operator<(const FileInfo& other) const
  {
    if (_is_dir != other._is_dir)
      return _is_dir;

    const char* lhs = _name;
    const char* rhs = other._name;

    while (*lhs && *rhs)
    {
      if (std::isdigit(*lhs) && std::isdigit(*rhs))
      {
        char* end_lhs;
        char* end_rhs;
        long num_lhs = std::strtol(lhs, &end_lhs, 10);
        long num_rhs = std::strtol(rhs, &end_rhs, 10);

        if (num_lhs != num_rhs)
        {
          return num_lhs < num_rhs;
        }
        lhs = end_lhs;
        rhs = end_rhs;
      }
      else
      {
        if (*lhs != *rhs)
        {
          return *lhs < *rhs;
        }
        ++lhs;
        ++rhs;
      }
    }

    return std::strcmp(lhs, rhs) < 0;
  }

  FileInfo::FileInfo(FileInfo&& other) noexcept : _name(other._name), _is_dir(other._is_dir), _name_len{other._name_len}
  {
    other._name = nullptr;
  }

  FileInfo& FileInfo::operator=(FileInfo&& other) noexcept
  {
    if (this != &other)
    {
      free(_name);
      _name = other._name;
      _is_dir = other._is_dir;
      _name_len = other._name_len;
      other._name = nullptr;
    }
    return *this;
  }

  //------------------------------------------------------------------------------------------------------------------------

  int FileStream::available()
  {
    return _fs.available(_file, _size);
  }

  size_t FileStream::readBytes(char* buffer, size_t length)
  {
    return _fs.readFromFile(_file, buffer, length);
  }

  int FileStream::read()
  {
    return -1;  // stub
  }

  size_t FileStream::write(uint8_t byte)
  {
    return 0;  // stub
  }

  int FileStream::peek()
  {
    return -1;  // stub
  }

  void FileStream::close()
  {
    _fs.closeFile(_file);
  }

  FileManager _fs;
}  // namespace pixeler