#ifndef ARITHMETICINSTRUCTION_H
#define ARITHMETICINSTRUCTION_H

#include "Interfaces.h"
#include "References.h"
#include "StringBuilder.h"

namespace Qbe::Instructions {
    enum class ArithmeticOperation {
        Add, // add
        Subtract, // sub
        Divide, // div/udiv
        Multiply, // mul
        Remainder, // rem/urem
        Negate
    };

    enum class ArithmeticSign {
        Signed, // s
        Unsigned // u
    };

    class Arithmetic : public IInstruction {
    public:
        ValueReference lhs;
        ValueReference rhs; // Might be empty for unary operations
        ValueReference destination;
        ArithmeticOperation operation;
        ArithmeticSign sign;

        explicit Arithmetic(ValueReference lhs, ValueReference destination,
                                  ArithmeticOperation operation, ArithmeticSign sign = ArithmeticSign::Signed,
                                  ValueReference rhs = ValueReference())
            : lhs(std::move(lhs)), rhs(std::move(rhs)), destination(std::move(destination)),
              operation(operation), sign(sign) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
            if (lhs.GetType() == nullptr || (rhs.GetType() == nullptr && operation != ArithmeticOperation::Negate)) {
                throw std::runtime_error("LHS and RHS types cannot be null");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            // (Destination) =(Type) (Operation) (Lhs) (Rhs)
            Utilities::StringBuilder sb;
            sb.Append(fmt::format("{} ={} {} {}",
                                  destination.Emit(is64Bit),
                                  lhs.GetType()->GetBodyString(is64Bit),
                                  operationToString(operation, sign),
                                  lhs.Emit(is64Bit)));

            if (operation != ArithmeticOperation::Negate) {
                sb.Append(fmt::format(", {}", rhs.Emit(is64Bit)));
            }

            return sb.ToString();
        }

        static std::string operationToString(ArithmeticOperation operation, ArithmeticSign sign) {
            switch (operation) {
                case ArithmeticOperation::Add:
                    return "add";
                case ArithmeticOperation::Subtract:
                    return "sub";
                case ArithmeticOperation::Divide:
                    return sign == ArithmeticSign::Signed ? "div" : "udiv";
                case ArithmeticOperation::Multiply:
                    return "mul";
                case ArithmeticOperation::Remainder:
                    return sign == ArithmeticSign::Signed ? "rem" : "urem";
                case ArithmeticOperation::Negate:
                    return "neg";
            }
            return "";
        }
    };
}

#endif //ARITHMETICINSTRUCTION_H
