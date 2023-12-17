#ifndef STATS_HPP
#define STATS_HPP
#include <stdint.h>

namespace rpg {

class BaseStats {
public:
    uint16_t strength;
    uint16_t willpower;
    uint16_t magical_prowess;
    uint16_t intelligence;
    uint16_t communication;
    uint16_t dexterity;
    uint16_t resourcefullness;
    uint16_t luck;
};
class DerivedStats {
public:
    uint64_t max_hp;
    uint64_t max_mana;
    uint32_t regeneration;
    uint32_t defense;
    uint16_t evasion;
};


//class LevelingStats {
//public:
//    uint16_t level;
//    uint64_t experience;
//};
//class Skills {
//public:
//    uint32_t skill_points;
//};

}
#endif