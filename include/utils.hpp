#pragma once

#include <vector>
#include <string>

namespace utils
{
    std::vector <std::string> stringToVector(const std::string &str, char separator, bool pushEmptyStrings = false)
    {
        std::vector<std::string> array;
        std::string temp;
        size_t len = str.size();

        for (size_t i = 0; i < len; i++) {
            if (str[i] == separator) {
                if (pushEmptyStrings || !temp.empty()) {
                    array.push_back(temp);
                    temp.clear();
                }
            }
            else
                temp.push_back(str[i]);
        }
        if (pushEmptyStrings || !temp.empty())
            array.push_back(temp);
        return array;
    }
}