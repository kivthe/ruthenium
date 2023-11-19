
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <random>
#include <thread>
#include <functional>

#include "core.h"
#include "window.h"
#include "format.h"
#include "logger.h"
#include "clock.h"

int main(int argc, char** argv)
{
    SYSLOG_INFO("Execution started");
    ruthen::InitializeAPIs();
    ruthen::Window window(720, 480, "OpenGL 3.3 window");
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
    SYSLOG_INFO("Execution finished successfully");
    SYSLOG_WRITE_DETAILS();
    SYSLOG_CLEAR_DETAILS();
    return 0;
}