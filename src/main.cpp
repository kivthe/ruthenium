
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>
#include <thread>
#include <functional>

#include "core.h"
#include "window.h"
#include "format.h"
#include "clock.h"

#include "subsys/log_manager.h"
//#include "memory/stack_allocator.h"

int main(int argc, char** argv)
{   
    std::unique_ptr<ruthen::subsys::LogManager> lm_ptr = std::make_unique<ruthen::subsys::LogManager>();
    lm_ptr->Initialize();
    lm_ptr->operator[](lm_ptr->GetClientLogger()).Log("Example.txt", "Pointer to Log Manager", ruthen::LogLevel::kTrace);

    ruthen::InitializeAPIs();
    ruthen::Window window(720, 480, "OpenGL 4.5 window");
    if(!window.GraphicsInitialized())
    {
        std::exit(-1);
    }
    ruthen::Clock clock;
    while(!window.ShouldClose())
    {
        if(clock.ElapsedTime().AsMicroseconds() < 16666) continue;
        clock.Reset();

        window.Update();
        window.SwapBuffers();
        window.Clear();
        
    }
    ruthen::TerminateAPIs();
    return 0;
}