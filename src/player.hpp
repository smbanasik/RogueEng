#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

#include "stats.hpp"

namespace rpg {

class Player {
public:

private:

    std::string name;
    BaseStats     attribute_block;
    DerivedStats  game_block;
};
}

#endif