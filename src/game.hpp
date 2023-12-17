#ifndef GAME_HPP
#define GAME_HPP

#include "player.hpp"

namespace rpg {

extern void init_game(void);
extern void start_menu(void);
extern void character_create(void);
extern void game_loop(void);


extern bool run_game;
extern Player main_char;
}
#endif