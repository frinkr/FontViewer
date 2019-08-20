#pragma once

#include <algorithm> 
#include <cctype>
#include <locale>
#include <string>

namespace FXHelper {
    template <typename S>
    void ltrim(S & s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](auto ch) {
            return !std::isspace(ch);
        }));
    }

    template <typename S>
    void rtrim(S & s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](auto ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    template <typename S>
    void trim(S & s) {
        ltrim(s);
        rtrim(s);
    }
}
