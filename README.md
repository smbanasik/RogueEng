# Archival
I have moved my development to GarFish Engine. While this has been an informative project, I've decided to start from scratch with a focus on utilizing less tutorial code, git from the beginning, better documentation of my processess and setup, and finally more thought into the design of the engine.


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

While it is relatively simple to create text or sprites on screen, abstracting all of this into an interface can be more challenging.
The current idea for the graphics is to build layers of abstraction on top of each other. The full low-level render interface will be split into text and sprites.
Each of these interfaces should offer sprites to "stick" to the screen (such as UI elements), or be in the world. The low level interface will manage the resources for these
and always render whatever it's given, with the interface it provides giving the ability to add, remove, or modify these elements. The sprite and text interfaces should
provide the ability to construct whatever is to be given to the low level render interface. The high level interface considers what should and shouldn't be on the screen.
A "scene" is a series of elements on the screen. A scene is an options menu, the main menu, an inventory screen, the game scree, etc. Anything that could be a window is a 
scene. The high level interface handles these scenes and determines what should and shouldn't be rendered. Additionally, this interface should handle "streaming," where 
items outside of the scenes limits are not rendered. Think of the game window, or a sprawling skill tree; these should be streamed.

## Build Instructions

Build using CMake and Visual Studio, use OpenGL Core 3.0

Source files need to be added to the script in source, then rebuild cmake
Shaders go into the `./shaders` folder, then rebuild cmake
Assets (images, fonts, etc.) go into the `./assets` folder, then rebuild cmake

### Third Party addons

Below are third party libraries used in this project, their expected folder names in quotes,
and either the expected folder structure or some instructions. Freetype is especially finicky,
so the folder order matters here.

- Freetype "freetype"
  - include
  - lib
- Glad "glad"
  - include
  - src
  - My CMake script
- GLFW-3.38 "glfw-3.3.8"
  - Download and place into folder
- GLM "glm_lib"
  - Git clone the repo into a folder, trim out anything extra like util or docs
- STB Image "stb_image"
  - Put the header into the folder
