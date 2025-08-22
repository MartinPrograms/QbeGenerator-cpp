#ifndef QBEIR_SHIFT_H
#define QBEIR_SHIFT_H

namespace Qbe::Instructions {
    enum class ShiftOperation {
        ShiftLeft, // shl
        ShiftRight, // shr
        ShiftRightArithmetic // sar
    };

    class Shift : public IInstruction {
    public:
        ValueReference lhs;
        ValueReference rhs;
        ValueReference destination;
        ShiftOperation operation;

        explicit Shift(ValueReference lhs, ValueReference rhs, ValueReference destination, ShiftOperation operation)
            : lhs(std::move(lhs)), rhs(std::move(rhs)), destination(std::move(destination)), operation(operation) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
            if (lhs.GetType() == nullptr || rhs.GetType() == nullptr) {
                throw std::runtime_error("LHS and RHS types cannot be null");
            }
            if (!lhs.GetType()->IsInteger() || !rhs.GetType()->IsInteger()) {
                throw std::runtime_error("LHS and RHS must be integer types for shift operations");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            // (Destination) =(Type) (Operation) (Lhs) (Rhs)
            Utilities::StringBuilder sb;
            sb.Append(fmt::format("{} ={} {} {}",
                                  destination.Emit(is64Bit),
                                  lhs.GetType()->GetString(is64Bit),
                                  operationToString(operation),
                                  lhs.Emit(is64Bit)));

            sb.Append(fmt::format(", {}", rhs.Emit(is64Bit)));

            return sb.ToString();
        }

        static std::string operationToString(ShiftOperation operation) {
            switch (operation) {
                case ShiftOperation::ShiftLeft:
                    return "shl";
                case ShiftOperation::ShiftRight:
                    return "shr";
                case ShiftOperation::ShiftRightArithmetic:
                    return "sar";
            }
            throw std::runtime_error("Unknown shift operation");
        }
    };
}

#endif //QBEIR_SHIFT_H