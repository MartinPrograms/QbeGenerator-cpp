#ifndef QBEIR_LIBRARY_H
#define QBEIR_LIBRARY_H

#include "References.h"
#include "StringBuilder.h"
#include "Interfaces.h"
#include "Types.h"
#include "Module.h"
#include "Function.h"
#include "Block.h"

namespace Qbe {
    static std::string FloatString(float value) {
        return fmt::format("s_{}", value);
    }

    static std::string DoubleString(double value) {
        return fmt::format("d_{}", value);
    }

    static Qbe::ValueReference CreateLiteral(int32_t value) {
        return Qbe::ValueReference(new Qbe::Literal(value));
    }

    static Qbe::ValueReference CreateLiteral(int64_t value) {
        return Qbe::ValueReference(new Qbe::Literal(value));
    }

    static Qbe::ValueReference CreateLiteral(int64_t value, bool is64Bit) {
        return Qbe::ValueReference(new Qbe::Literal(value, is64Bit));
    }

    static Qbe::ValueReference CreatePointer(int64_t value) {
        return Qbe::ValueReference(Qbe::Literal::CreatePointerLiteral(value));
    }

    static Qbe::ValueReference CreateLiteral(float value) {
        return Qbe::ValueReference(new Qbe::Literal(value));
    }

    static Qbe::ValueReference CreateLiteral(double value) {
        return Qbe::ValueReference(new Qbe::Literal(value));
    }

    inline Primitive* Int32 = new Primitive(TypeDefinitionKind::Int32);
    inline Primitive* Int64 = new Primitive(TypeDefinitionKind::Int64);
    inline Primitive* Float32 = new Primitive(TypeDefinitionKind::Float32);
    inline Primitive* Float64 = new Primitive(TypeDefinitionKind::Float64);
    inline Primitive* Pointer = new Primitive(TypeDefinitionKind::Pointer);
    inline VoidType* Void = new VoidType();
}

#endif //QBEIR_LIBRARY_H