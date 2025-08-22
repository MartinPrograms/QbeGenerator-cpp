#ifndef QBEIR_JUMP_H
#define QBEIR_JUMP_H
#include "Interfaces.h"


namespace Qbe::Instructions {
    enum class JumpType {
        Unconditional, // jmp @label
        Conditional, // jnz %value, @false, @true
        Halt, // hlt
    };

    class Jump : public IInstruction {
    public:
        JumpType type;
        ValueReference condition; // Only used for conditional jumps
        IIdentifiable* target; // The target block for the jump
        IIdentifiable* falseTarget; // The target block for the false condition, if applicable

        Jump(JumpType type, IIdentifiable* target, ValueReference condition = ValueReference(), IIdentifiable* falseTarget = nullptr)
            : type(type), condition(std::move(condition)), target(target), falseTarget(falseTarget) {
            if (type == JumpType::Conditional && (condition.kind == ValueReferenceKind::Empty || falseTarget == nullptr)) {
                throw std::runtime_error("Conditional jump must have a condition and a false target");
            }
            if (target == nullptr) {
                throw std::runtime_error("Jump target cannot be null");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            Utilities::StringBuilder sb;
            switch (type) {
                case JumpType::Unconditional:
                    sb.Append(fmt::format("jmp @{}", target->GetIdentifier()));
                    break;
                case JumpType::Conditional:
                    sb.Append(fmt::format("jnz {}, @{}, @{}",
                                          condition.Emit(is64Bit),
                                          target->GetIdentifier(),
                                          falseTarget->GetIdentifier()));
                    break;
                case JumpType::Halt:
                    sb.Append("hlt");
                    break;
            }

            return sb.ToString();
        }
    };
}

#endif //QBEIR_JUMP_H