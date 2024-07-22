# RogueEng
This is RogueEng, a game engine written in C++ which uses OpenGL as it's rendering platform.

## Scope

RogueEng is intended to service the needs for a 2d tile based, turn based game. Most of these would be roguelikes, but RPG games are viable as well.
The engine will not have a graphic interface, but instead serve as a framework of higher level interfaces and prebuild classes to allow for focus on the game itself, while
still providing the developer with technical control of underlying systems, allowing them to make further optimizations.

## Progress

- [X] Sprite Rendering - Completed sometime in January
- [X] Text Rendering - Completed in May
- [ ] Low-Level Render Interface
- [ ] High-Level Render Interface
- [ ] Window Class
- [ ] Button Class
- [ ] Proper Mouse and Keyboard Support
- [ ] Integration of SB Lib's Dynamic Pool
- [ ] Rewrite of text and low-level render interfaces
- [ ] Sound
- [ ] Attributions for third party uses

## Design

## Build Instructions

### Third Party addons

Freetype
Glad
GLFW-3.38
GLM
STB Image
