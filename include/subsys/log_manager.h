#ifndef RUTHEN_LOG_MANAGER_H
#define RUTHEN_LOG_MANAGER_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <queue>

#include "format.h"
#include "patterns/singleton.h"

namespace ruthen
{
//----------------------------------------------------------------------

class LogManager;

//----------------------------------------------------------------------

enum LogLevel : int
{
    kTrace = 0,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kCritical,
    kCrash
};

typedef std::size_t LoggerID;

//----------------------------------------------------------------------

class Logger
{
    friend class LogManager;

public:
    struct LogDetails
    {
        std::string time;
        std::string level;
        std::string name;
        std::string destination;
        std::string message;
    };

public:
    Logger();
    Logger(const std::string& name);
    Logger(const std::string& name, const LogLevel& level);
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept = default;
    Logger& operator=(Logger&&) noexcept = default;
    bool operator==(const std::string& name) const;
    bool operator!=(const std::string& name) const;
    bool operator==(const Logger& rhs) const;
    bool operator!=(const Logger& rhs) const;
    ~Logger();

public:
    void Log(const std::string& filename, const std::string& log) const;
    void Log(const std::string& filename, const std::string& log, LogLevel level) const;
    template<typename... Args>
    void Log(const std::string& filename, const std::string& format, Args... args) const
    {
        if (suppress_output_) return;
        std::string formated = format;
        if constexpr(sizeof...(Args) > 0) formated = Format(format, std::forward<Args>(args)...);
        Log(filename, formated);
    }
    template<typename... Args>
    void Log(const std::string& filename, const std::string& format, LogLevel level, Args... args) const
    {
        if (suppress_output_) return;
        std::string formated = format;
        if constexpr(sizeof...(Args) > 0) formated = Format(format, std::forward<Args>(args)...);
        Log(filename, formated, level);
    }
    
    void SetName(const std::string& name);
    void SetLevel(LogLevel level);
    void SetLogCallback(void(*log_callback)(LogDetails));

public:
    [[nodiscard]] std::string GetName() const;

public:
    std::string name_;
    LogLevel level_;
    bool suppress_output_;
    bool suppress_callback_;

    void (*on_log_callback_)(LogDetails);
};

//----------------------------------------------------------------------

namespace subsys
{

//----------------------------------------------------------------------

class LogManager : public patterns::Singleton<LogManager>
{
public:
    LogManager();

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) noexcept = default;
    LogManager& operator=(LogManager&&) noexcept = default;
    Logger& operator[](LoggerID id);
    ~LogManager();

public:
    void Initialize();
    void Shutdown();

public:
    LoggerID CreateLogger(const std::string& name);
    void DeleteLogger(LoggerID id);
    void DeleteLogger(const std::string& name);
    
public:
    LoggerID GetLogger(const std::string& name) const;
    LoggerID GetSystemLogger()   const;
    LoggerID GetDebugLogger()    const;
    LoggerID GetGraphicsLogger() const;
    LoggerID GetClientLogger()   const;
    bool LoggerExists(LoggerID id) const;
    bool LoggerExists(const std::string& name) const;
    bool DefaultLoggersInitialized() const;

private:
    std::unordered_map<LoggerID, Logger> loggers_;
    std::queue<LoggerID> reusable_ids_;
};

//----------------------------------------------------------------------



//----------------------------------------------------------------------

}

}

#endif