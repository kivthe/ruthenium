
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "core.h"
#include "window.h"

constexpr float kUpdateRateMS = 1.0f / 60.0f * 1000.0f;

int main(int argc, char** argv)
{
    ruthen::InitializeAPIs();

    ruthen::Window window(720, 480, "OpenGL 3.3 window");
    window.MakeCurrent();
    ruthen::Window::SetBufferSwapInterval(0);
    ruthen::Window::InitializeGraphicsFunctional();

    while(!window.ShouldClose())
    {
        static auto begin = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        auto delta_time = now - begin;
        auto mils = std::chrono::duration_cast<std::chrono::microseconds>(delta_time);
        float ms_passed = mils.count() / 1000.0f;
        if(ms_passed < kUpdateRateMS) continue;
        begin = now;

        ruthen::Window::PollEvents();
        window.SwapBuffers();
        window.Clear();
        
    }

    ruthen::TerminateAPIs();

    return 0;
}