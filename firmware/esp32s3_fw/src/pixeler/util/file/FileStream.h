/**
 * @file FileStream.h
 * @brief Нащадок класу "Stream"
 * @details Клас перевантажує деякі методи Stream, та додає нові можливості, такі як закриття потоку.
 * Для доступу до файлів використовує FileManager.h.
 * Використовується в класах серверів, де стандартна реалізація веб-серверу не передбачає переривання передачі файлу.
 */

#pragma once
#pragma GCC optimize("O3")

#include <Stream.h>

namespace pixeler
{
  class FileStream : public Stream
  {
  public:
    FileStream(FILE* file, const char* name, size_t size) : _name{name}, _file(file), _size{size} {}
    virtual ~FileStream();
    virtual int available() override;
    virtual size_t readBytes(char* buffer, size_t length) override;
    virtual int read() override;
    virtual size_t write(uint8_t byte) override;
    virtual int peek() override;
    virtual void flush() override {}
    void close();

    const char* name() const;
    size_t size() const;
    virtual operator bool() const;

  private:
    String _name;
    FILE* _file;
    size_t _size;
  };
}  // namespace pixeler