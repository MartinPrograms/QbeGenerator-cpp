#ifndef QBEIR_BITWISE_H
#define QBEIR_BITWISE_H

// Or, xor, and!
namespace Qbe::Instructions {
    enum class BitwiseOperation {
        Or,
        Xor,
        And
    };

    class Bitwise : public IInstruction {
    public:
        ValueReference lhs;
        ValueReference rhs;
        ValueReference destination;
        BitwiseOperation operation;

        explicit Bitwise(ValueReference lhs, ValueReference rhs, ValueReference destination, BitwiseOperation operation)
            : lhs(std::move(lhs)), rhs(std::move(rhs)), destination(std::move(destination)), operation(operation) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
            if (lhs.GetType() == nullptr || rhs.GetType() == nullptr) {
                throw std::runtime_error("LHS and RHS types cannot be null");
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

        static std::string operationToString(BitwiseOperation operation) {
            switch (operation) {
                case BitwiseOperation::Or:
                    return "or";
                case BitwiseOperation::Xor:
                    return "xor";
                case BitwiseOperation::And:
                    return "and";
            }
            throw std::runtime_error("Unknown bitwise operation");
        }
    };
}

#endif //QBEIR_BITWISE_H