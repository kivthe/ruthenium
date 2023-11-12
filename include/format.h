#ifndef RUTHEN_FORMAT_H
#define RUTHEN_FORMAT_H

#include <string>
#include <vector>

namespace ruthen
{

template <typename... T>
std::string Format(const std::string &format, T... args)
{
    if (format.empty()) return {};
    std::vector<std::string> vec{args...};
    if (vec.empty()) return format;
    std::string result = format;
    std::size_t search_index = 0;
    std::size_t index = result.find('%', search_index);
    while (index != std::string::npos)
    {
        if (!std::isdigit(result[index + 1]))
        {
            search_index = index + 1;
            index = result.find('%', search_index);
            continue;
        }
        std::size_t last_digit = index;
        for (std::size_t i = last_digit + 1; i < result.size(); ++i)
        {
            if (!std::isdigit(result[i]))
                break;
            ++last_digit;
        }
        if (last_digit == index)
        {
            search_index = index + 1;
            index = result.find('%', search_index);
            continue;
        }
        std::string number = result.substr(index + 1, last_digit - index);
        std::int64_t n = std::stoi(number);
        if (n < 1 || n > static_cast<std::int64_t>(vec.size()))
        {
            search_index = index + 1;
            index = result.find('%', search_index);
            continue;
        }
        result.erase(index, number.size() + 1);
        result.insert(index, vec[n - 1]);
        search_index = index + 1;
        index = result.find('%', search_index);
    }
    return result;
}

}
#endif