#include "pchs/std.hpp"

template class std::basic_string<char>;
template class std::basic_string_view<char>;
template class std::array<char, num_256>;
template class std::array<char, num_512>;
template class std::array<const char*, 1>;
template class std::array<char*, 1>;
template class std::array<char*, num_eight>;
template class std::array<const char*, num_eight>;
template class std::array<std::array<char, num_256>, 1>;
template class std::array<std::array<char, num_512>, 1>;
template class std::array<std::array<char, num_256>, num_eight>;
template class std::array<std::array<char, num_512>, num_eight>;
