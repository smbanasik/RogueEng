// Objective: Create an ASCI RPG game.
// Files:
// - main.cpp: calls the engine
// - engine.hpp & cpp: core engine, runs all functions, initializes things, creates the window
// - render.hpp & cpp: core render area, handles all graphics
// - input.hpp & cpp: core input handling, provides a method for input listening
// - game.hpp & cpp: core of game, runs all game logic
// - shader.hpp: file that contains abstractions for shaders
// - various other game files
#include <engine.hpp>

int main(int argc, char* argv[]) {

    uteng::Engine eng;
    eng.run_engine();

    return 0;
}