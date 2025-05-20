#include "pchs/ecs.hpp"

template class std::vector<std::uint32_t>;
template class std::vector<std::uint32_t, std::pmr::polymorphic_allocator<std::uint32_t>>;

template class std::vector<std::uint64_t>;
template class std::vector<std::uint64_t, std::pmr::polymorphic_allocator<std::uint64_t>>;

template class std::unordered_set<std::uint64_t>;
template class std::unordered_set<
    std::uint64_t,
    std::hash<std::uint64_t>,
    std::equal_to<std::uint64_t>,
    std::pmr::polymorphic_allocator<std::uint64_t>>;

template class atom::utils::dense_map<std::uint32_t, void*>;
// template class atom::utils::dense_map<
//     std::uint32_t,
//     void*,
//     std::pmr::polymorphic_allocator<std::pair<std::uint32_t, void*>>>;
