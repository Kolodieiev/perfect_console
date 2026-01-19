#pragma GCC optimize("O3")

#include "FileStream.h"

#include "pixeler/manager/FileManager.h"

namespace pixeler
{
  FileStream::~FileStream()
  {
    close();
  }

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

  const char* FileStream::name() const
  {
    return _name.c_str();
  }

  size_t FileStream::size() const
  {
    return _size;
  }

  FileStream::operator bool() const
  {
    return _file != nullptr;
  }
}  // namespace pixeler
