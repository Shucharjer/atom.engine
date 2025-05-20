#pragma once
#include <cstdint>
#include <functional>
#include <memory_resource>
#include <unordered_set>
#include <vector>
#include <ecs.hpp>
#include <schedule.hpp>
#include <structures/dense_map.hpp>
#include <world.hpp>

extern template class std::vector<std::uint32_t>;
extern template class std::vector<std::uint32_t, std::pmr::polymorphic_allocator<std::uint32_t>>;

extern template class std::vector<std::uint64_t>;
extern template class std::vector<std::uint64_t, std::pmr::polymorphic_allocator<std::uint64_t>>;

extern template class std::unordered_set<std::uint64_t>;
extern template class std::unordered_set<
    std::uint64_t,
    std::hash<std::uint64_t>,
    std::equal_to<std::uint64_t>,
    std::pmr::polymorphic_allocator<std::uint64_t>>;

extern template class atom::utils::dense_map<std::uint32_t, void*>;
// extern template class atom::utils::dense_map<
//     std::uint32_t,
//     void*,
//     std::pmr::polymorphic_allocator<std::pair<std::uint32_t, void*>>>;
