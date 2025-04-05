#pragma once
#include <array>
#include <string>
#include <string_view>
#include "core/langdef.hpp"

extern template class std::basic_string<char>;
extern template class std::basic_string_view<char>;
extern template class std::array<char, num_256>;
extern template class std::array<char, num_512>;
extern template class std::array<const char*, 1>;
extern template class std::array<char*, 1>;
extern template class std::array<char*, num_eight>;
extern template class std::array<const char*, num_eight>;
extern template class std::array<std::array<char, num_256>, 1>;
extern template class std::array<std::array<char, num_512>, 1>;
extern template class std::array<std::array<char, num_256>, num_eight>;
extern template class std::array<std::array<char, num_512>, num_eight>;
