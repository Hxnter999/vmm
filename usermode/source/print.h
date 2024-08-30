#ifndef __cpp_lib_print
#include <iostream>
#include <string_view>
#include <format>

namespace std {
  template <typename... Ts>
  void println(std::string_view format_str, Ts&&... args) {
    std::cout << std::vformat(format_str, std::make_format_args(std::forward<Ts>(args)...)) << std::endl;
  }
}
#else
#include <print>
#endif