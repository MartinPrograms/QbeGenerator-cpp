#ifndef COPY_H
#define COPY_H
#include "Interfaces.h"
#include "References.h"

namespace Qbe::Instructions {
    class Copy : public IInstruction {
    public:
        Qbe::ValueReference source;
        Qbe::ValueReference destination;

        Copy(Qbe::ValueReference source, Qbe::ValueReference destination)
            : source(std::move(source)), destination(std::move(destination)) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            auto destinationStr = destination.Emit(is64Bit);
            auto sourceTypeStr = source.GetType()->GetBodyString(is64Bit);
            auto sourceEmit = source.Emit(is64Bit);
            return fmt::format("{} ={} copy {}",
                               destinationStr,
                               sourceTypeStr,
                               sourceEmit
                               );
        }
    };
}

#endif //COPY_H
