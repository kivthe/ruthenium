
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

constexpr float kUpdateRateMS = 1.0f / 60.0f * 1000.0f;


int main(int argc, char** argv)
{
    ruthen::Logger logger("Main Logger", ruthen::Logger::LogLevel::kInfo);
    logger.SetLevel(ruthen::Logger::kInfo);
    logger.Log("logs.txt", "Test message #1");
    logger.Log(ruthen::Logger::kTrace, "logs.txt", "Test message #2");
    logger.WriteLogDetailsToFile("log_details.txt");

    ruthen::InitializeAPIs();
    ruthen::Window window(720, 480, "OpenGL 3.3 window");

    while(!window.ShouldClose())
    {
        static auto begin = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        auto delta_time = now - begin;
        auto mils = std::chrono::duration_cast<std::chrono::microseconds>(delta_time);
        float ms_passed = mils.count() / 1000.0f;
        if(ms_passed < kUpdateRateMS) continue;
        begin = now;


        window.Update();
        window.SwapBuffers();
        window.Clear();
        
    }

    ruthen::TerminateAPIs();

    return 0;
}