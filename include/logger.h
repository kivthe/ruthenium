#ifndef RUTHEN_LOGGER_H
#define RUTHEN_LOGGER_H

#include <string>
#include <cstdint>
#include <vector>

namespace ruthen
{

class Logger
{
public:
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
    struct LogDetails
    {
        std::string time_string;
        std::int64_t since_epoch_nano;
        std::string name;
        LogLevel level;
        std::string destination;
        std::string formated_message;
    };

public:
    Logger(const std::string& name);
    Logger(const std::string& name, LogLevel level);
    Logger(const Logger& src);
    Logger& operator=(const Logger& rhs);
    Logger(Logger&& src) noexcept;
    Logger& operator=(Logger&& rhs) noexcept;
    ~Logger();

public:
    void SetName(const std::string& name);
    void SetLevel(LogLevel level);
    bool WriteLogDetailsToFile(const std::string& file_name);
    void ClearLogDetails();
    void SetLogDetailsLimit(std::size_t stack_limit);
    void EraseLogDetails(std::size_t index_from_top);
    void SwitchLogDetailsRecognition(bool flag);
    
    bool Log(const std::string& file_name, const std::string& message);
    bool Log(LogLevel level, const std::string& file_name, const std::string& message);

public:
    std::string GetName() const;
    bool LogDetailsEmpty() const;
    const LogDetails& GetLogDetails(std::size_t index_from_top) const;
    const LogDetails& GetLastLogDetails() const;
    const LogDetails& GetOldestLogDetails() const;

private:
    LogLevel level_;
    std::string logger_name_;
    bool log_details_recognition_flag_;
    std::size_t log_details_stack_limit_;
    std::vector<LogDetails> log_details_;
};

}

#endif