#ifndef NAMETRACKER_H
#define NAMETRACKER_H

#include <string>
#include <fmt/core.h>
namespace Qbe {
    class NameTracker {
        inline static int nextNameIndex = 0;
    public:
        static std::string getNextName() {
            return fmt::format("name_{}", nextNameIndex++);
        }
    };
}

#endif //NAMETRACKER_H
