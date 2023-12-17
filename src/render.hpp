#ifndef UT_RENDER_HPP
#define UT_RENDER_HPP
// - render.hpp & cpp: core render area, handles all graphics
// This render is split into two parts. The first is the engine agnostic Opengl render code, dupped core_render
// TODO: Consider splitting core render into its own section? Depends on if we need to call core_render things
// The next is the engine specific rendering such as ascii graphics and text, dubbed uteng_render
namespace core_render {



extern void init_render(void);
extern void terminate_render(void);
extern void run_render_loop(void);

}
namespace uteng_render {

}

#endif