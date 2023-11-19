#ifndef RUTHEN_CLOCK_H
#define RUTHEN_CLOCK_H

#include <chrono>

namespace ruthen
{

constexpr static double kUpdateRate = 1.0 / 60.0;

class Time
{
public:
    Time() = default;
    template<typename Rep, typename Ratio>
    Time(const std::chrono::duration<Rep, Ratio>& duration)
    {
        nanoseconds_ = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    }

    Time operator+(const Time& time) const;
    Time operator-(const Time& time) const;
    Time operator*(const std::int64_t& multiplier) const;
    Time operator/(const std::int64_t& multiplier) const;

    Time& operator+=(const Time& time);
    Time& operator-=(const Time& time);
    Time& operator*=(const std::int64_t& multiplier);
    Time& operator/=(const std::int64_t& multiplier);

    bool operator==(const Time& time) const;
    bool operator!=(const Time& time) const;
    bool operator<(const Time& time) const;
    bool operator>(const Time& time) const;
    bool operator<=(const Time& time) const;
    bool operator>=(const Time& time) const;

    
public:
    static Time FromSeconds(std::int64_t seconds);
    static Time FromMilliseconds(std::int64_t millisseconds);
    static Time FromMicroseconds(std::int64_t microseconds);
    static Time FromNanoseconds(std::int64_t nanoseconds);

public:
    std::int64_t AsSeconds() const;
    std::int64_t AsMilliseconds() const;
    std::int64_t AsMicroseconds() const;
    std::int64_t AsNanoseconds() const;

private:
    std::chrono::nanoseconds nanoseconds_;
};

class Clock
{
public:
    Clock();

public:
    Time Reset();

public:
    Time ElapsedTime() const;

private:
    Time time_;
};

}

#endif