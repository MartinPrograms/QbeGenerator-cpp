#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H
#include <string>

namespace Utilities {

class StringBuilder {
public:
    StringBuilder() = default;

    StringBuilder& Append(const std::string& str) {
        result += str;
        return *this;
    }

    StringBuilder& Append(char c) {
        result += c;
        return *this;
    }

    StringBuilder& AppendLine(const std::string& str) {
        result += str + "\n";
        return *this;
    }

    StringBuilder& AppendLine() {
        result += "\n";
        return *this;
    }

    [[nodiscard]] std::string ToString() const {
        return result;
    }

private:
    std::string result;
};

} // Utilities

#endif //STRINGBUILDER_H
