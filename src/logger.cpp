
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <array>
#include <chrono>
#include <filesystem>
#include <memory>

#include "logger.h"
#include "format.h"

namespace ruthen
{

std::shared_ptr<Logger> SystemLogger = std::make_shared<Logger>("System Logger", Logger::kTrace);

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

static std::array<std::string, 7> kLevelNames =
{
    "Trace",
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Critical",
    "Crash"
};

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

Logger::Logger(const std::string& name) :
    level_{LogLevel::kTrace},
    logger_name_{name},
    log_details_recognition_flag_{true},
    log_details_stack_limit_{500},
    log_details_{}
{}

//------------------------------------------------------------

Logger::Logger(const std::string& name, LogLevel level) :
    level_{level},
    logger_name_{name},
    log_details_recognition_flag_{true},
    log_details_stack_limit_{500},
    log_details_{}
{}

//------------------------------------------------------------

Logger::Logger(const Logger& src) :
    level_{src.level_},
    logger_name_{src.logger_name_},
    log_details_recognition_flag_{src.log_details_recognition_flag_},
    log_details_stack_limit_{src.log_details_stack_limit_},
    log_details_{src.log_details_}
{}

//------------------------------------------------------------

Logger& Logger::operator=(const Logger& rhs)
{
    level_ = rhs.level_;
    logger_name_ = rhs.logger_name_;
    log_details_recognition_flag_ = rhs.log_details_recognition_flag_;
    log_details_stack_limit_ = rhs.log_details_stack_limit_;
    log_details_ = rhs.log_details_;
    return *this;
}

//------------------------------------------------------------

Logger::Logger(Logger&& src) noexcept :
    level_{std::move(src.level_)},
    logger_name_{std::move(src.logger_name_)},
    log_details_recognition_flag_{std::move(src.log_details_recognition_flag_)},
    log_details_stack_limit_{std::move(src.log_details_stack_limit_)},
    log_details_{std::move(src.log_details_)}
{}

//------------------------------------------------------------

Logger& Logger::operator=(Logger&& rhs) noexcept
{
    level_ = rhs.level_;
    logger_name_ = rhs.logger_name_;
    log_details_recognition_flag_ = rhs.log_details_recognition_flag_;
    log_details_stack_limit_ = rhs.log_details_stack_limit_;
    log_details_ = rhs.log_details_;
    return *this;
}

//------------------------------------------------------------

Logger::~Logger() 
{
    log_details_.clear();
}

//------------------------------------------------------------

void Logger::SetName(const std::string& name)
{
    logger_name_ = name;
}

//------------------------------------------------------------

void Logger::SetLevel(LogLevel level)
{
    level_ = level;
}

//------------------------------------------------------------

bool Logger::WriteLogDetailsToFile(const std::string& file_name)
{
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(file_name, std::ios::app);
    if(file.fail() || !file.is_open()) return false;
    for(const auto& details : log_details_)
    {
        file << std::string(50, '-') << '\n';
        file << "Exact time              : " << details.time_string << '\n';
        file << "Nanoseconds since epoch : " << details.since_epoch_nano << '\n';
        file << "Log sender name         : " << details.name << '\n';
        file << "Level of the log        : " << kLevelNames[static_cast<std::size_t>(details.level)] << '\n';
        file << "Log file destination    : "<< details.destination << '\n';
        file << "Formated log message    : "<< details.formated_message << '\n';
    }
    file.close();
    return true;
}

//------------------------------------------------------------

void Logger::ClearLogDetails()
{
    log_details_.clear();
}

//------------------------------------------------------------

void Logger::SetLogDetailsLimit(std::size_t stack_limit)
{
    if(stack_limit <= 0) throw std::invalid_argument{"Log details recognition limit is to to zero or lower"};
    log_details_stack_limit_= stack_limit;
}

//------------------------------------------------------------

void Logger::EraseLogDetails(std::size_t index_from_top)
{
    if(index_from_top + 1 > log_details_.size()) throw std::invalid_argument{"Failed to erase log details of an invalid index"};
    log_details_.erase(log_details_.begin() + log_details_.size() + index_from_top - 1);
}

//------------------------------------------------------------

void Logger::SwitchLogDetailsRecognition(bool flag)
{
    log_details_recognition_flag_ = flag;
}

//------------------------------------------------------------

bool Logger::Log(const std::string& file_name, const std::string& message)
{
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(file_name, std::ios::app);
    if(file.fail() || !file.is_open()) return false;
    std::time_t time = std::time(nullptr);
    std::tm* tm = std::localtime(&time);
    std::string formated = Format("[%1.%2.%3 %4:%5:%6] [%7] |%8| : %9", 
                                    std::to_string(tm->tm_mday), 
                                    std::to_string(tm->tm_mon + 1), 
                                    std::to_string(tm->tm_year + 1900),
                                    std::to_string(tm->tm_sec),
                                    std::to_string(tm->tm_min),
                                    std::to_string(tm->tm_hour),
                                    GetName(),
                                    kLevelNames[static_cast<std::size_t>(level_)],
                                    message
                                    );
    file << formated << std::endl;
    file.close();
    if(log_details_recognition_flag_ && log_details_stack_limit_ > log_details_.size())
    {
        LogDetails details;

        auto now = std::chrono::system_clock::now();
        auto since_epoch = now.time_since_epoch();
        std::int64_t nano = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch).count();
        details.time_string = Format("[%1.%2.%3 %4:%5:%6]", 
                                    std::to_string(tm->tm_mday), 
                                    std::to_string(tm->tm_mon + 1), 
                                    std::to_string(tm->tm_year + 1900),
                                    std::to_string(tm->tm_sec),
                                    std::to_string(tm->tm_min),
                                    std::to_string(tm->tm_hour)
                                    );
        details.since_epoch_nano = nano;
        details.name = logger_name_;
        details.level = level_;
        details.destination = std::filesystem::absolute(file_name);
        details.formated_message = message;

        log_details_.emplace_back(std::move(details));
    }
    return true;
}

//------------------------------------------------------------

bool Logger::Log(LogLevel level, const std::string& file_name, const std::string& message)
{
    std::ofstream file;
    file.exceptions(std::ios::badbit | std::ios::failbit);
    file.open(file_name, std::ios::app);
    if(file.fail() || !file.is_open()) return false;
    std::time_t time = std::time(nullptr);
    std::tm* tm = std::localtime(&time);
    std::string formated = Format("[%1.%2.%3 %4:%5:%6] [%7] |%8| : %9", 
                                    std::to_string(tm->tm_mday), 
                                    std::to_string(tm->tm_mon + 1), 
                                    std::to_string(tm->tm_year + 1900),
                                    std::to_string(tm->tm_sec),
                                    std::to_string(tm->tm_min),
                                    std::to_string(tm->tm_hour),
                                    GetName(),
                                    kLevelNames[static_cast<std::size_t>(level)],
                                    message
                                    );
    file << formated << std::endl;
    file.close();
    if(log_details_recognition_flag_ && log_details_stack_limit_ > log_details_.size())
    {
        LogDetails details;

        auto now = std::chrono::system_clock::now();
        auto since_epoch = now.time_since_epoch();
        std::int64_t nano = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch).count();
        
        details.time_string = Format("[%1.%2.%3 %4:%5:%6]", 
                                    std::to_string(tm->tm_mday), 
                                    std::to_string(tm->tm_mon + 1), 
                                    std::to_string(tm->tm_year + 1900),
                                    std::to_string(tm->tm_sec),
                                    std::to_string(tm->tm_min),
                                    std::to_string(tm->tm_hour)
                                    );
        details.since_epoch_nano = nano;
        details.name = logger_name_;
        details.level = level;
        details.destination = std::filesystem::absolute(file_name);
        details.formated_message = message;

        log_details_.emplace_back(std::move(details));
    }

    return true;
}

//------------------------------------------------------------

std::string Logger::GetName() const
{
    return logger_name_;
}

//------------------------------------------------------------

bool Logger::LogDetailsEmpty() const
{
    return log_details_.empty();
}

//------------------------------------------------------------

const Logger::LogDetails& Logger::GetLogDetails(std::size_t index_from_top) const
{
    if(index_from_top + 1 > log_details_.size()) throw std::invalid_argument{"Failed to obtain log details via an invalid index"};
    return *(log_details_.begin() + log_details_.size() - index_from_top - 1);
}

//------------------------------------------------------------

const Logger::LogDetails& Logger::GetLastLogDetails() const
{
    if(log_details_.size() <= 0) throw std::out_of_range{"Failed to obtain the last log details from an empty log details stack"};
    return log_details_.back();
}

//------------------------------------------------------------

const Logger::LogDetails& Logger::GetOldestLogDetails() const
{
    if(log_details_.size() <= 0) throw std::out_of_range{"Failed to obtain the oldest log details from an empty log details stack"};
    return log_details_.front();
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}



