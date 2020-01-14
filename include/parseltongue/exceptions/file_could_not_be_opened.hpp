#ifndef FILE_FILE_COULD_NOT_BE_OPENED_EXCEPTION_HPP
#define FILE_FILE_COULD_NOT_BE_OPENED_EXCEPTION_HPP

#include <iostream>
#include <exception>

class FileCouldNotBeOpened : public std::exception {
private:
  std::string m_msg;
public:
  FileCouldNotBeOpened(std::string file_path) : m_msg{std::string("The supplied file path ") + file_path + " could not be opened"} {}

  virtual const char* what() const throw() {
    return m_msg.c_str();
  }
};

#endif // FILE_FILE_COULD_NOT_BE_OPENED_EXCEPTION_HPP