#ifndef FILE_PLUGIN_EXCEPTION_HPP
#define FILE_PLUGIN_EXCEPTION_HPP

#include <iostream>
#include <exception>

class PluginException : public std::exception {
private:
  std::string m_msg;
public:
  PluginException(std::string msg) : m_msg{msg} {}

  virtual const char* what() const throw() {
    return m_msg.c_str();
  }
};

#endif // FILE_PLUGIN_EXCEPTION_HPP