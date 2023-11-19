#include "clock.h"

namespace ruthen
{
    Time Time::FromSeconds(std::int64_t seconds)
    {
        return Time(std::chrono::seconds(seconds));
    }
    Time Time::FromMilliseconds(std::int64_t seconds)
    {
        return Time(std::chrono::milliseconds(seconds));
    }
    Time Time::FromMicroseconds(std::int64_t seconds)
    {
        return Time(std::chrono::microseconds(seconds));
    }
    Time Time::FromNanoseconds(std::int64_t seconds)
    {
        return Time(std::chrono::nanoseconds(seconds));
    }

    Time Time::operator+(const Time& time) const
    {
        Time copy = *this;
        copy += time;
        return copy;
    }
    Time Time::operator-(const Time& time) const
    {
        Time copy = *this;
        copy -= time.nanoseconds_;
        return copy;
    }
    Time Time::operator*(const std::int64_t& multiplier) const
    {
        Time copy = *this;
        copy *= multiplier;
        return copy;
    }
    Time Time::operator/(const std::int64_t& multiplier) const
    {
        Time copy = *this;
        copy /= multiplier;
        return copy;
    }

    Time& Time::operator+=(const Time& time)
    {
        nanoseconds_ += time.nanoseconds_;
        return *this;
    }
    Time& Time::operator-=(const Time& time)
    {
        nanoseconds_ -= time.nanoseconds_;
        return *this;
    }
    Time& Time::operator*=(const std::int64_t& multiplier)
    {
        nanoseconds_ *= multiplier;
        return *this;
    }
    Time& Time::operator/=(const std::int64_t& multiplier)
    {
        nanoseconds_ /= multiplier;
        return *this;
    }

    bool Time::operator==(const Time& time) const
    {
        return nanoseconds_ == time.nanoseconds_;
    }
    bool Time::operator!=(const Time& time) const
    {
        return !(operator==(time));
    }
    bool Time::operator<(const Time& time) const
    {
        return nanoseconds_ < time.nanoseconds_;
    }
    bool Time::operator>(const Time& time) const
    {
        return !(operator<(time)) && !(operator==(time));
    }
    bool Time::operator<=(const Time& time) const
    {
        return !(operator>(time));
    }
    bool Time::operator>=(const Time& time) const
    {
        return !(operator<(time));
    }
    std::int64_t Time::AsSeconds() const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(nanoseconds_).count();
    }
    std::int64_t Time::AsMilliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(nanoseconds_).count();
    }
    std::int64_t Time::AsMicroseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(nanoseconds_).count();
    }
    std::int64_t Time::AsNanoseconds() const
    {
        return nanoseconds_.count();
    }

    Clock::Clock() :
        time_{std::chrono::system_clock::now().time_since_epoch()}
    {}

    Time Clock::Reset()
    {
        Time copy = time_;
        time_ = Time(std::chrono::system_clock::now().time_since_epoch());
        return copy;
    }
    Time Clock::ElapsedTime() const
    {
        return Time(std::chrono::system_clock::now().time_since_epoch()) - time_;
    }
}