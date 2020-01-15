#ifndef FILE_FILE_FORMAT_EXCEPTION_HPP
#define FILE_FILE_FORMAT_EXCEPTION_HPP

#include <iostream>
#include <exception>

class FileFormatException : public std::exception {
private:
  std::string m_msg;
public:
  FileFormatException(std::string msg) : m_msg{msg} {}

  virtual const char* what() const throw() {
    return m_msg.c_str();
  }
};

#endif // FILE_FILE_FORMAT_EXCEPTION_HPP