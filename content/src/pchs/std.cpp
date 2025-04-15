#include "pchs/std.hpp"

template class std::basic_string<char>;
template class std::basic_string_view<char>;
template class std::array<char, magic_256>;
template class std::array<char, magic_512>;
template class std::array<const char*, 1>;
template class std::array<char*, 1>;
template class std::array<char*, magic_8>;
template class std::array<const char*, magic_8>;
template class std::array<std::array<char, magic_256>, 1>;
template class std::array<std::array<char, magic_512>, 1>;
template class std::array<std::array<char, magic_256>, magic_8>;
template class std::array<std::array<char, magic_512>, magic_8>;
