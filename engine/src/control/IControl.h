#pragma once

#include "../logging/Logger.h"

#include <memory>
#include <string>
#include <vector>

class IControl {
public:
    IControl() = default;

    virtual ~IControl() = default;

    void clearUserInput() {
        _userInputString.clear();
        _userInputVector.clear();
    }

    int getUserInputSize() {
        return static_cast<int>(_userInputVector.size());
    }

    void deleteLastUserInput() {
        if (!_userInputVector.empty()) {
            _userInputVector.pop_back();
        }
        _userInputString.clear();
        for (const auto& str : _userInputVector) {
            _userInputString += str;
        }
    }

    const std::string& getUserInput() {
        return _userInputString;
    }

    void addUserInput(const std::string& str) {
        _userInputString += str;
        _userInputVector.emplace_back(str);
    }

    const std::vector<std::string>& getUserInputVector() {
        return _userInputVector;
    }

    static std::string encodeUTF8(const unsigned int codepoint) {
        std::string str;

        if (codepoint < 0x80) {
            str += static_cast<char>(codepoint);
        }
        else if (codepoint < 0x800) {
            str += (codepoint >> 6) | 0xc0;
            str += (codepoint & 0x3f) | 0x80;
        }
        else if (codepoint < 0x10000) {
            str += (codepoint >> 12) | 0xe0;
            str += ((codepoint >> 6) & 0x3f) | 0x80;
            str += (codepoint & 0x3f) | 0x80;
        }
        else if (codepoint < 0x110000) {
            str += (codepoint >> 18) | 0xf0;
            str += ((codepoint >> 12) & 0x3f) | 0x80;
            str += ((codepoint >> 6) & 0x3f) | 0x80;
            str += (codepoint & 0x3f) | 0x80;
        }

        return str;
    }

protected:
    std::string _userInputString;
    std::vector<std::string> _userInputVector;

};
