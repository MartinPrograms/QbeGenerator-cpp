#ifndef ALLOCATE_H
#define ALLOCATE_H

#include "Interfaces.h"
#include "References.h"
#include "StringBuilder.h"
#include <fmt/core.h>

namespace Qbe::Instructions {

    class Allocate : public IInstruction {
    public:
        ValueReference destination;
        ValueReference size;
        int alignment;

        explicit Allocate(ValueReference destination, ValueReference size, int alignment = 4)
            : destination(std::move(destination)), size(std::move(size)), alignment(alignment) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
            if (size.GetType() == nullptr || !size.GetType()->IsEqual(Primitive(TypeDefinitionKind::Int64))) {
                throw std::runtime_error("Size must be of int64 type");
            }

            if (alignment != 4 && alignment != 8 && alignment != 16) {
                throw std::runtime_error("Alignment must be 4, 8, or 16 bytes");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("{} ={} alloc{} {}", destination.Emit(is64Bit), destination.GetType()->GetBodyString(is64Bit), alignment, size.Emit(is64Bit));
        }
    };
}

#endif //ALLOCATE_H
