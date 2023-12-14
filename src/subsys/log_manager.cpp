#include <iostream>
#include <fstream>
#include <ctime>
#include <array>
#include <stdexcept>
#include <algorithm>

#include "subsys/log_manager.h"
#include "format.h"

namespace ruthen
{

//----------------------------------------------------------------------

namespace
{

struct ReservedLoggerInfo
{
    LoggerID id;
    const char* name;
    LogLevel level;
};

}

static std::array<ReservedLoggerInfo, 4> kReservedLoggers =
{
    ReservedLoggerInfo{1, "SystemLogger",   LogLevel::kError},
    ReservedLoggerInfo{2, "DebugLogger",    LogLevel::kDebug},
    ReservedLoggerInfo{3, "GraphicsLogger", LogLevel::kTrace},
    ReservedLoggerInfo{4, "ClientLogger",    LogLevel::kError}
};

//----------------------------------------------------------------------

Logger::Logger() :
    name_{},
    level_{LogLevel::kTrace},
    suppress_output_{false},
    suppress_callback_{false},
    on_log_callback_{nullptr}
{}
Logger::Logger(const std::string& name):
    name_{name},
    level_{LogLevel::kTrace},
    suppress_output_{false},
    suppress_callback_{false},
    on_log_callback_{nullptr}
{}

Logger::Logger(const std::string& name, const LogLevel& level):
    name_{name},
    level_{level},
    suppress_output_{false},
    suppress_callback_{false},
    on_log_callback_{nullptr}
{}

bool Logger::operator==(const std::string& name) const
{
    return name_ == name;
}

bool Logger::operator!=(const std::string& name) const
{
    return !operator==(name);
}

bool Logger::operator==(const Logger& rhs) const
{
    return name_ == rhs.name_ && 
           level_ == rhs.level_ && 
           suppress_output_ == rhs.suppress_output_ &&
           suppress_callback_ == rhs.suppress_callback_ &&
           on_log_callback_ == rhs.on_log_callback_;
}

bool Logger::operator!=(const Logger& rhs) const
{
    return !operator==(rhs);
}

Logger::~Logger()
{}

void Logger::Log(const std::string& filename, const std::string& log, LogLevel level) const
{
    if (suppress_output_) return;
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(filename, std::ios::app);
    std::time_t time = std::time(nullptr);
    const std::tm* tm = std::localtime(&time);
    std::string time_str = Format("%1:%2:%3 %4-%5-%6", 
                          std::to_string(tm->tm_sec),
                          std::to_string(tm->tm_min),
                          std::to_string(tm->tm_hour),
                          std::to_string(tm->tm_mday),
                          std::to_string(tm->tm_mon + 1),
                          std::to_string(tm->tm_year + 1900));
    std::string level_str;
    switch(level)
    {
        case LogLevel::kTrace:    level_str = "Trace";    break;
        case LogLevel::kDebug:    level_str = "Debug";    break;
        case LogLevel::kInfo:     level_str = "Info";     break;
        case LogLevel::kWarn:     level_str = "Warn";     break;
        case LogLevel::kError:    level_str = "Error";    break;
        case LogLevel::kCritical: level_str = "Critical"; break;
        case LogLevel::kCrash:    level_str = "Crash";    break;
        default: level_str = "Trace";
    }

    file << "--------------------------------------------" << '\n';
    file << "Time:         " << time_str << '\n';
    file << "Level:        " << level_str << '\n';
    file << "Name:         " << name_ << '\n';
    file << "Destination:  " << filename << '\n';
    file << '\n';
    file << log << '\n';

    file.close();
    if (suppress_callback_ || on_log_callback_ == nullptr) return;
    LogDetails details;
    details.time = time_str;
    details.name = name_;
    details.destination = filename;
    details.message = log;
    on_log_callback_(details);
}

void Logger::Log(const std::string& filename, const std::string& log) const
{
    Log(filename, log, level_);
}

void Logger::SetName(const std::string& name)
{
    name_ = name;
}

void Logger::SetLevel(LogLevel level)
{
    level_ = level;
}

void Logger::SetLogCallback(void(*log_callback)(LogDetails))
{
    if(log_callback != nullptr) on_log_callback_ = log_callback;
}

std::string Logger::GetName() const
{
    return name_;
}

//----------------------------------------------------------------------

namespace subsys
{


LogManager::LogManager() :
    loggers_{},
    reusable_ids_{}
{}


Logger& LogManager::operator[](LoggerID id)
{
    return loggers_.at(id);
}

LogManager::~LogManager()
{
    Shutdown();
}

void LogManager::Initialize()
{
    for(std::size_t i = 0; i < kReservedLoggers.size(); ++i)
    {
        CreateLogger(kReservedLoggers[i].name);
    }
}

void LogManager::Shutdown()
{
    loggers_.clear();
    reusable_ids_ = {};
}

LoggerID LogManager::CreateLogger(const std::string& name)
{
    for(const auto& [id, logger] : loggers_)
    {
        if(logger == name) throw std::invalid_argument{"logger with given name already exists"};
    }

    for(const auto& [res_id, res_name, res_level] : kReservedLoggers)
    {
        if (name != res_name) continue;
        auto result = loggers_.emplace(std::make_pair(res_id, res_name));
        if(!result.second) throw std::runtime_error{"failed to create a new logger"};
        result.first->second.SetLevel(res_level);
        return res_id;
    }

    LoggerID id{};
    if (!reusable_ids_.empty())
    {
        id = reusable_ids_.front();
        reusable_ids_.pop();
    }
    else id = (loggers_.size() < kReservedLoggers.size() ? kReservedLoggers.back().id + 1 : loggers_.size() + 1);

    auto result = loggers_.emplace(std::make_pair(id, name));
    if(!result.second) throw std::runtime_error{"failed to create a new logger"};
    result.first->second.SetLevel(LogLevel::kTrace);
    return id;
}

void LogManager::DeleteLogger(LoggerID id)
{
    auto iterator = loggers_.find(id);
    if (iterator == loggers_.end()) throw std::out_of_range{"invalid logger identifier"};
    auto reserved_iter = std::find_if(kReservedLoggers.begin(), kReservedLoggers.end(), [&](const ReservedLoggerInfo& info)->bool{return loggers_[id].GetName() == info.name;});
    if(reserved_iter != kReservedLoggers.end()) throw std::invalid_argument{"can not explicitly delete reserved loggers"};
    loggers_.erase(iterator);
    reusable_ids_.push(id);
}

void LogManager::DeleteLogger(const std::string& name)
{
    for(const auto& [res_id, res_name, res_level] : kReservedLoggers)
    {
        if (name == res_name) throw std::invalid_argument{"can not explicitly delete reserved loggers"};
    }
    for(auto& [id, logger] : loggers_)
    {
        if (logger == name) 
        {
            loggers_.erase(id);
            reusable_ids_.push(id);
            break;
        }
    }

}

LoggerID LogManager::GetLogger(const std::string& name) const
{
    for(auto& [id, logger_name] : loggers_)
    {
        if (logger_name == name) 
        {
            return id;
        }
    }
    return -1;
}
LoggerID LogManager::GetSystemLogger() const
{
    return GetLogger(kReservedLoggers[0].name);
}
LoggerID LogManager::GetDebugLogger() const
{
    return GetLogger(kReservedLoggers[1].name);
}
LoggerID LogManager::GetGraphicsLogger() const
{
    return GetLogger(kReservedLoggers[2].name);
}
LoggerID LogManager::GetClientLogger() const
{
    return GetLogger(kReservedLoggers[3].name);
}
bool LogManager::LoggerExists(LoggerID id) const
{
    return loggers_.find(id) != loggers_.end();
}
bool LogManager::LoggerExists(const std::string& name) const
{
    for(auto& [id, logger] : loggers_)
    {
        if (logger == name) return true;
    }
    return false;
}
bool LogManager::DefaultLoggersInitialized() const
{
    for(const auto& [res_id, res_name, res_level] : kReservedLoggers)
    {
        if (!LoggerExists(res_name)) return false;
    }
    return true;
}

}

}