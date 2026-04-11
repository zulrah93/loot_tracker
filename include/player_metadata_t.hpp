#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <cstdint>
#include <stdio.h>
#include <string>

constexpr const size_t MAX_XP{13000000};
constexpr const size_t MAX_HP{100};
constexpr const size_t MAX_CHARACTER_NAME_LIMIT{16};

enum player_class_t : uint8_t {
  invalid_class = 0,
  attacker = 1,
  caster = 2,
  healer = 3
};

struct player_metadata_t {
  char player_name[MAX_CHARACTER_NAME_LIMIT]; // C string to save this later on disk more easily
                        // without serialization
  player_class_t selected_class;
  uint8_t attack_skill_points;  // Skill points go from 1 to 255
  uint8_t defense_skill_points; // Same as above
  uint8_t health_skill_points;  // Same as above
  uint64_t experience;
  uint8_t health;
  uint8_t attack;
  uint8_t defense;
  uint8_t armor;
} __attribute__((packed));

uint8_t experience_to_level(const player_metadata_t &player_meta_data) {
  auto xp = player_meta_data.experience;
  [[unlikely]]
  if (0 == xp) {
    return 1;
  }
  return static_cast<uint8_t>(std::log(static_cast<double>(xp)) /
                              std::log(1.18));
}

bool save_player_data_on_disk(const std::string &file_name,
                              player_metadata_t &player_info) {
  auto player_data_handle = fopen(file_name.c_str(), "wb");
  size_t bytes_written =
      fwrite(reinterpret_cast<uint8_t *>(&player_info), sizeof(uint8_t),
             sizeof(player_metadata_t), player_data_handle);
  return bytes_written == sizeof(player_metadata_t);
}

bool load_player_data_into_memory(const std::string &file_name,
                                  player_metadata_t &player_info) {
  auto player_data_handle = fopen(file_name.c_str(), "rb");
  size_t bytes_written =
      fread(reinterpret_cast<uint8_t *>(&player_info), sizeof(uint8_t),
            sizeof(player_metadata_t), player_data_handle);
  return bytes_written == sizeof(player_metadata_t);
}

#endif