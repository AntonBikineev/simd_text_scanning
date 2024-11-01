#include <cstddef>
#include <string>

std::string generate_alpha_num_string(size_t len) {
  static constexpr char alphanum[] = "0123456789"
                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "abcdefghijklmnopqrstuvwxyz";

  std::string result;
  result.reserve(len);

  for (size_t i = 0; i < len; ++i) {
    result += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return result;
}
