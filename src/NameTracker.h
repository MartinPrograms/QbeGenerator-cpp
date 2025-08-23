#ifndef NAMETRACKER_H
#define NAMETRACKER_H

#include <string>
#include <fmt/core.h>
namespace Qbe {
    class NameTracker {
        inline static int nextNameIndex = 0;
        inline static uint64_t stackIndex = 0;

    public:
        static std::string getNextName() {
            return fmt::format("name_{}", nextNameIndex++);
        }

        static uint64_t pushStackIndex() {
            return stackIndex++;
        }

        static uint64_t popStackIndex() {
            if (stackIndex == 0) {
                throw std::runtime_error("Stack index underflow");
            }
            return --stackIndex;
        }
    };
}

#endif //NAMETRACKER_H
