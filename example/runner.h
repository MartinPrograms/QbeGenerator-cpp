#ifndef QBEIR_RUNNER_H
#define QBEIR_RUNNER_H

#include <string>

class runner {
public:
    runner (std::string output) : output(std::move(output)) {}

    int run();

private:
    std::string output;
};


#endif //QBEIR_RUNNER_H