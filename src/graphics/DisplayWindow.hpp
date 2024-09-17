#pragma once

#include <functional>

#include "../common.hpp"
#include "GraphicsBuffer.hpp"
#include "../RGFW_bridge.hpp"


// TODO: fix a crash when interacting with the window

namespace nyasRT
{
class DisplayWindow
{
public:

    GraphicsBuffer * gbuf_p;
    RGFW_window * window;
    bool window_closed;

    DisplayWindow(GraphicsBuffer & gbuf)
    : gbuf_p{&gbuf}, window{nullptr}, window_closed{true} {

        window = wrapped_RGFW_createWindow("nyasRT display", 0, 0, gbuf.width(), gbuf.height(), 0);
        window_closed = window == nullptr;

        if (!window_closed)
        {
            RGFW_window_makeCurrent(window);
            _glClearColor(0, 0, 0, 0);
        }
    }

    ~DisplayWindow()
    {
        close();
    }

    bool should_close()
    {
        return RGFW_window_shouldClose(window);
    }
    void close()
    {
        if (!window_closed) { RGFW_window_close(window); }
    }
    bool closed() const noexcept
    {
        return window_closed;
    }

    /// @return refresh time
    f32 refresh()
    {
        constexpr nyasRT::u32 GL_COLOR_BUFFER_BIT = 0x00004000;
        constexpr nyasRT::u32 GL_DEPTH_BUFFER_BIT = 0x00000100;
        constexpr nyasRT::u32 GL_RGB = 0x1907;
        constexpr nyasRT::u32 GL_FLOAT = 0x1406;

        if (window_closed) { return 0; }
        return timeit([this] ()
        {
            RGFW_window_makeCurrent(window);
            _glClearColor(0, 0, 0, 0);

            _glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _glDrawPixels(gbuf_p->width(), gbuf_p->height(), GL_RGB, GL_FLOAT, gbuf_p->data());

            RGFW_window_swapBuffers(window);
        });
    }
};

} // namespace nyasRT
