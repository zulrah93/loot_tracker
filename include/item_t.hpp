#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <cstdint>
#include <unordered_map>

using item_id_t = uint64_t;

enum item_type_t : uint8_t {
    invalid = 0,
    weapon = 1,
    armor = 2
};

enum item_effect_t : uint8_t {
    none = 0,
    repeat_damage_but_half,
    reflect_damage,
    life_steal
};

struct item_t {
    item_id_t id;
    item_type_t type;
    uint8_t attack_stat;
    uint8_t defense_stat;
    uint8_t hp_bonus;
} __attribute__((packed));

static std::unordered_map<item_id_t, std::string> ITEM_ID_DATABASE = {
        {0, "Bronze Sword"}, {1, "Iron Sword"}, {2, "Steel Sword"},
        {3, "Bronze Shield"}, {4, "Iron Shield"}, {5, "Steel Shield"},
        {6, "Mirror Shield"}, {7, "Wooden Bow"}, {8, "Yew Bow"}, {9, "Echo sword"}
                                
};


#endif