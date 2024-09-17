#pragma once

#include <inttypes.h>
#include <functional>


// stuff in RGFW [https://github.com/ColleagueRiley/RGFW]

extern "C"
{
typedef struct RGFW_window RGFW_window;

//extern RGFW_window * RGFW_createWindow(char const* name, RGFW_rect rect, uint16_t args);
extern void RGFW_window_makeCurrent(RGFW_window * win);
extern void RGFW_window_swapBuffers(RGFW_window * win);
extern uint8_t RGFW_window_shouldClose(RGFW_window * win);
extern void RGFW_window_close(RGFW_window * win);

}

extern RGFW_window * wrapped_RGFW_createWindow(char const* name, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t args);

extern std::function<void(float, float, float, float)> _glClearColor;
extern std::function<void(int32_t, int32_t, uint32_t, uint32_t, void const*)> _glDrawPixels;
extern std::function<void(uint32_t)> _glClear;
