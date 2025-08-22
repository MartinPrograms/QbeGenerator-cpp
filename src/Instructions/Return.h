#ifndef RETURN_H
#define RETURN_H
#include "Interfaces.h"
#include "References.h"

namespace Qbe::Instructions {
    class Return : public IInstruction {
    public:
        Qbe::ValueReference value;
        explicit Return(Qbe::ValueReference value) : value(std::move(value)) {}

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("ret {}", value.Emit(is64Bit));
        }
    };
}

#endif //RETURN_H
