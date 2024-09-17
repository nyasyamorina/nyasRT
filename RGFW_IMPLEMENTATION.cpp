// This file is to avoid <windows.h> from polluting the namespace

#include "src/RGFW_bridge.hpp"

#define RGFW_IMPLEMENTATION
#include "third-party/RGFW/include/RGFW.h"


static std::function<void(float, float, float, float)> _glClearColor = glClearColor;
static std::function<void(int32_t, int32_t, uint32_t, uint32_t, void const*)> _glDrawPixels = glDrawPixels;
static std::function<void(uint32_t)> _glClear = glClear;

static RGFW_window * wrapped_RGFW_createWindow(
    char const* name,
    int32_t x, int32_t y, int32_t w, int32_t h,
    uint16_t args
) {
    return RGFW_createWindow(name, {x, y, w, h}, args);
}

[[maybe_unused]] static bool _unused_function()
{
    float * tmp = new float[100 * 100 * 3];

    RGFW_window * window = wrapped_RGFW_createWindow("_unused_function", 100, 100, 100, 100, RGFW_CENTER);
    RGFW_window_makeCurrent(window);

    _glClearColor(0, 0, 0, 0);
    _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _glDrawPixels(100, 100, GL_RGB, GL_FLOAT, tmp);
    RGFW_window_swapBuffers(window);

    bool result = RGFW_window_shouldClose(window);
    RGFW_window_close(window);

    delete[] tmp;
    return result;
}
