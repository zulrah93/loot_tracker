#ifndef MONSTER_H
#define MONSTER_H

#include <item_t.hpp>
#include <string>
#include <cstdint>

using monster_id_t = uint64_t;
using probability_t = uint16_t;

constexpr const size_t MAX_LOOT_TABLE_SIZE{64};

struct monster_t {
    monster_id_t id;
    item_id_t loot_table[MAX_LOOT_TABLE_SIZE]; // Array of item ids if one of them is 0 its invalid you can skip
    probability_t probability[MAX_LOOT_TABLE_SIZE]; //Array of proability which maps to the loot_table
} __attribute__((packed));

static std::unordered_map<monster_id_t, std::string> GOBLIN_ID_DATABASE = {
        {0, "Bronze Sword"}, {1, "Iron Sword"}, {2, "Steel Sword"},
        {3, "Bronze Shield"}, {4, "Iron Shield"}, {5, "Steel Shield"},
        {6, "Mirror Shield"}, {7, "Wooden Bow"}, {8, "Yew Bow"}, {9, "Echo sword"}
                                
};

#endif