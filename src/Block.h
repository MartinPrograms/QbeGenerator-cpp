#ifndef BLOCK_H
#define BLOCK_H

#include "Interfaces.h"
#include "StringBuilder.h"
#include <fmt/core.h>

#include "References.h"
#include "Instructions/Instructions.h"

#include "NameTracker.h"

namespace Qbe {
    // A block is the actual code that gets executed.
    class Block : public IEmitter, IIdentifiable {
    public:

        std::string identifier;
        [[nodiscard]] std::string GetIdentifier() const override {
            return identifier;
        }

        std::vector<IInstruction*> instructions;

        explicit Block(std::string identifier) : identifier(std::move(identifier)) {}

        void addInstruction(IInstruction* instruction) {
            if (instruction == nullptr) {
                throw std::runtime_error("Instruction cannot be null");
            }
            instructions.push_back(instruction);
        }

        void addReturn(ValueReference value) {
            addInstruction(new Instructions::Return(std::move(value)));
        }

        void addReturn() {
            addInstruction(new Instructions::Return(ValueReference()));
        }

        ValueReference addCopy(ValueReference value) {
            auto valueReference = nextValueReference(value.GetType());
            auto copy = new Instructions::Copy(value, valueReference);
            addInstruction(copy);
            return valueReference;
        }

        ValueReference addCopy(ValueReference source, ValueReference destination) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
            auto* copyInstruction = new Instructions::Copy(source, destination);
            addInstruction(copyInstruction);
            return destination;
        }

        ValueReference addArithmetic(ValueReference lhs, ValueReference rhs, Instructions::ArithmeticSign sign, Instructions::ArithmeticOperation operation) {
            auto valueReference = nextValueReference(lhs.GetType());
            auto* arithmeticInstruction = new Instructions::Arithmetic(lhs, valueReference, operation, sign, rhs);
            addInstruction(arithmeticInstruction);
            return valueReference;
        }

        ValueReference addArithmetic(ValueReference lhs, ValueReference rhs, Instructions::ArithmeticOperation operation) {
            Instructions::ArithmeticSign sign = lhs.GetType()->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addArithmetic(lhs, rhs, sign, operation);
        }

        ValueReference addAdd(ValueReference lhs, ValueReference rhs, Instructions::ArithmeticSign sign) {
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Add);
        }

        ValueReference addSub(ValueReference lhs, ValueReference rhs) {
            Instructions::ArithmeticSign sign = lhs.GetType()->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Subtract);
        }

        ValueReference addSub(ValueReference lhs, ValueReference rhs, Instructions::ArithmeticSign sign) {
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Subtract);
        }

        ValueReference addAdd(ValueReference lhs, ValueReference rhs) {
            Instructions::ArithmeticSign sign = lhs.GetType()->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Add);
        }

        ValueReference addDiv(ValueReference lhs, ValueReference rhs, Instructions::ArithmeticSign sign) {
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Divide);
        }

        ValueReference addDiv(ValueReference lhs, ValueReference rhs) {
            Instructions::ArithmeticSign sign = lhs.GetType()->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Divide);
        }

        ValueReference addMul(ValueReference lhs, ValueReference rhs) {
            return addArithmetic(lhs, rhs, Instructions::ArithmeticSign::Signed, Instructions::ArithmeticOperation::Multiply);
        }

        ValueReference addRem(ValueReference lhs, ValueReference rhs, Instructions::ArithmeticSign sign) {
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Remainder);
        }

        ValueReference addRem(ValueReference lhs, ValueReference rhs) {
            Instructions::ArithmeticSign sign = lhs.GetType()->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addArithmetic(lhs, rhs, sign, Instructions::ArithmeticOperation::Remainder);
        }

        ValueReference addNegate(ValueReference value) {
            Instructions::ArithmeticSign sign = value.GetType()->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addArithmetic(value, ValueReference(), sign, Instructions::ArithmeticOperation::Negate);
        }

        ValueReference addBitwise(ValueReference lhs, ValueReference rhs, Instructions::BitwiseOperation operation) {
            auto valueReference = nextValueReference(lhs.GetType());
            auto* bitwiseInstruction = new Instructions::Bitwise(lhs, rhs, valueReference, operation);
            addInstruction(bitwiseInstruction);
            return valueReference;
        }

        ValueReference addOr(ValueReference lhs, ValueReference rhs) {
            return addBitwise(lhs, rhs, Instructions::BitwiseOperation::Or);
        }

        ValueReference addXor(ValueReference lhs, ValueReference rhs) {
            return addBitwise(lhs, rhs, Instructions::BitwiseOperation::Xor);
        }

        ValueReference addAnd(ValueReference lhs, ValueReference rhs) {
            return addBitwise(lhs, rhs, Instructions::BitwiseOperation::And);
        }

        ValueReference addShift(ValueReference lhs, ValueReference rhs, Instructions::ShiftOperation operation) {
            auto valueReference = nextValueReference(lhs.GetType());
            auto* shiftInstruction = new Instructions::Shift(lhs, rhs, valueReference, operation);
            addInstruction(shiftInstruction);
            return valueReference;
        }

        ValueReference addShiftLeft(ValueReference lhs, ValueReference rhs) {
            return addShift(lhs, rhs, Instructions::ShiftOperation::ShiftLeft);
        }

        ValueReference addShiftRight(ValueReference lhs, ValueReference rhs) {
            return addShift(lhs, rhs, Instructions::ShiftOperation::ShiftRight);
        }

        ValueReference addShiftRightArithmetic(ValueReference lhs, ValueReference rhs) {
            return addShift(lhs, rhs, Instructions::ShiftOperation::ShiftRightArithmetic);
        }

        ValueReference addLoad(ValueReference source, ITypeDefinition* type, Instructions::ArithmeticSign sign) {
            auto* local = createLocal(NameTracker::getNextName(), type);
            auto valueReference = toValueReference(local);
            auto* loadInstruction = new Instructions::Load(source, valueReference, sign);
            addInstruction(loadInstruction);
            return valueReference;
        }

        ValueReference addLoad(ValueReference source, ITypeDefinition* type) {
            Instructions::ArithmeticSign sign = type->IsSigned() ? Instructions::ArithmeticSign::Signed : Instructions::ArithmeticSign::Unsigned;
            return addLoad(source, type, sign);
        }

        void addStore(ValueReference source, ValueReference destination) {
            auto* storeInstruction = new Instructions::Store(source, destination);
            addInstruction(storeInstruction);
        }

        ValueReference addAllocate(ValueReference size, int alignment = 4) {
            auto destination = toValueReference(createLocal(NameTracker::getNextName(), new Primitive(TypeDefinitionKind::Pointer)));
            auto* allocateInstruction = new Instructions::Allocate(destination, size, alignment);
            addInstruction(allocateInstruction);
            return destination;
        }

        void addUnconditionalJump(Block *target) {
            if (target == nullptr) {
                throw std::runtime_error("Jump target cannot be null");
            }
            auto* jumpInstruction = new Instructions::Jump(Instructions::JumpType::Unconditional, target);
            addInstruction(jumpInstruction);
        }

        void addConditionalJump(Block *target, ValueReference condition, Block *falseTarget = nullptr) {
            if (target == nullptr || condition.kind == ValueReferenceKind::Empty) {
                throw std::runtime_error("Conditional jump target and condition cannot be null");
            }
            if (falseTarget == nullptr) {
                throw std::runtime_error("False target for conditional jump cannot be null");
            }
            auto* jumpInstruction = new Instructions::Jump(Instructions::JumpType::Conditional, target, std::move(condition), falseTarget);
            addInstruction(jumpInstruction);
        }

        void addHalt() {
            auto* jumpInstruction = new Instructions::Jump(Instructions::JumpType::Halt, nullptr);
            addInstruction(jumpInstruction);
        }

        ValueReference addCall(Function* function, std::vector<ValueReference> arguments = {}) {
            if (function == nullptr) {
                throw std::runtime_error("Function cannot be null");
            }
            if (function->returnType == nullptr) {
                throw std::runtime_error("Function return type cannot be null");
            }

            auto valueReference = toValueReference(createLocal(NameTracker::getNextName(), function->returnType));
            auto* callInstruction = new Instructions::Call(function, std::move(arguments), valueReference);
            addInstruction(callInstruction);
            return valueReference;
        }

        // Adds an equality instruction that compares two values and returns a result.

        // The result is 1 if the comparison is true, 0 otherwise.

        ValueReference addEquality(ValueReference lhs, ValueReference rhs, Instructions::EqualityOperation operation, Instructions::EqualityPrimitive primitive) {
            if (lhs.GetType() == nullptr || rhs.GetType() == nullptr) {
                throw std::runtime_error("LHS and RHS types cannot be null");
            }
            if (!lhs.GetType()->IsEqual(*rhs.GetType())) {
                throw std::runtime_error("LHS and RHS types must be the same");
            }

            auto valueReference = toValueReference(createLocal(NameTracker::getNextName(), new Qbe::Primitive(TypeDefinitionKind::Pointer))); // Use a pointer type for equality results (32 bit or 64 bit, depending on architecture)
            auto* equalityInstruction = new Instructions::Equality(primitive, operation, lhs, rhs, valueReference);
            addInstruction(equalityInstruction);
            return valueReference;
        }

        ValueReference addEquality(ValueReference lhs, ValueReference rhs, Instructions::EqualityOperation operation) {
            Instructions::EqualityPrimitive primitive;
            if (lhs.GetType()->IsFloat()) {
                primitive = Instructions::EqualityPrimitive::Float;
            } else if (lhs.GetType()->IsInteger()) {
                primitive = lhs.GetType()->IsSigned() ? Instructions::EqualityPrimitive::Signed : Instructions::EqualityPrimitive::Unsigned;
            } else {
                throw std::runtime_error("LHS and RHS types must be either integer or float for equality comparisons");
            }
            return addEquality(lhs, rhs, operation, primitive);
        }

        ValueReference allocateType(CustomType* type, bool is64Bit) {
            auto size = addCopy(ValueReference(new Literal(static_cast<ITypeDefinition*>(type)->ByteSize(is64Bit), true))); // Force a 64-bit literal for size
            return addAllocate(size, 4);
        }

        ValueReference allocateType(CustomType* type, ValueReference count, bool is64Bit) {
            auto typeSize = static_cast<ITypeDefinition*>(type)->ByteSize(is64Bit);
            auto totalSize = addMul(count, addCopy(ValueReference(new Literal(typeSize, true)))); // Force a 64-bit literal for size
            return addAllocate(totalSize, 4);
        }

        ValueReference getFieldAddress(ValueReference structAddress, CustomType* type, const std::string& fieldName, bool is64Bit) {
            if (structAddress.GetType() == nullptr) {
                throw std::runtime_error("Struct address type cannot be null");
            }

            auto offset = type->GetByteOffsetOfField(fieldName, is64Bit);
            ValueReference offsetValue = ValueReference(new Literal((int64_t)offset, false));

            return addArithmetic(structAddress, offsetValue, Instructions::ArithmeticSign::Unsigned, Instructions::ArithmeticOperation::Add);
        }

        ValueReference getFieldAddress(ValueReference structAddress, CustomType* type, int fieldIndex, bool is64Bit) {
            if (structAddress.GetType() == nullptr) {
                throw std::runtime_error("Struct address type cannot be null");
            }
            if (fieldIndex < 0 || fieldIndex >= static_cast<int>(type->fields.size())) {
                throw std::runtime_error("Field index out of bounds");
            }

            uint64_t offset = 0;
            for (int i = 0; i < fieldIndex; i++) {
                offset += type->fields[i].value->ByteSize(is64Bit);
            }
            ValueReference offsetValue = ValueReference(new Literal((int64_t)offset, false));

            return addArithmetic(structAddress, offsetValue, Instructions::ArithmeticSign::Unsigned, Instructions::ArithmeticOperation::Add);
        }

        ValueReference loadFieldAddress(ValueReference structAddress, CustomType* type, const std::string& fieldName, bool is64Bit) {
            auto fieldAddress = getFieldAddress(structAddress, type, fieldName, is64Bit);

            auto fieldType = type->GetFieldByName(fieldName).value;
            return addLoad(fieldAddress, fieldType, Instructions::ArithmeticSign::Unsigned);
        }

        ValueReference storeFieldAddress(ValueReference structAddress, CustomType* type, const std::string& fieldName, ValueReference value, bool is64Bit) {
            auto fieldAddress = getFieldAddress(structAddress, type, fieldName, is64Bit);
            addStore(value, fieldAddress);
            return fieldAddress;
        }

        ValueReference addVariadicList(Instructions::VariadicListType* architecture, bool is64Bit) {
            return addAllocate(Qbe::ValueReference(new Literal((int64_t)architecture->GetSize(is64Bit), false)), 4);
        }

        void addVariadicStart(ValueReference list) {
            auto* variadicStartInstruction = new Instructions::VariadicStart(list);
            addInstruction(variadicStartInstruction);
        }

        ValueReference addVariadicArgument(ValueReference list, ITypeDefinition* type) {
            auto* local = createLocal(NameTracker::getNextName(), type);
            auto valueReference = toValueReference(local);
            auto* variadicArgInstruction = new Instructions::VariadicArgument(list, valueReference);
            addInstruction(variadicArgInstruction);
            return valueReference;
        }

        ValueReference addConversion(ValueReference source, Primitive* targetType, Instructions::ConversionType type, Instructions::ConversionSign sign) {
            auto *local = createLocal(NameTracker::getNextName(), targetType);
            auto valueReference = toValueReference(local);
            auto* instruction = new Instructions::Conversion(source, valueReference, type, sign);
            addInstruction(instruction);
            return valueReference;
        }

        ValueReference addConversion(ValueReference source, Primitive* targetType) {
            Instructions::ConversionType type;

            // we must decide if its Extend, Truncate, IntToFloat or FloatToInt
            if (source.GetType()->IsInteger() && targetType->IsInteger()) {
                auto sourceSize = source.GetType()->ByteSize(false);
                auto targetSize = targetType->ByteSize(false);
                if (sourceSize < targetSize) {
                    type = Instructions::ConversionType::Extend;
                } else if (sourceSize > targetSize) {
                    type = Instructions::ConversionType::Truncate;
                } else {
                    // No conversion needed if sizes are the same, but we will still emit a copy to ensure the correct type is used.
                    type = Instructions::ConversionType::Extend; // Treat same size integer conversion as an extend, it will just copy the value and change the type.
                }
            } else if (source.GetType()->IsFloat() && targetType->IsInteger()) {
                type = Instructions::ConversionType::FloatToInt;
            } else if (source.GetType()->IsInteger() && targetType->IsFloat()) {
                type = Instructions::ConversionType::IntToFloat;
            } else if (source.GetType()->IsFloat() && targetType->IsFloat()) {
                auto sourceSize = source.GetType()->ByteSize(false);
                auto targetSize = targetType->ByteSize(false);
                if (sourceSize < targetSize) {
                    type = Instructions::ConversionType::Extend;
                } else if (sourceSize > targetSize) {
                    type = Instructions::ConversionType::Truncate;
                } else {
                    // No conversion needed if sizes are the same, but we will still emit a copy to ensure the correct type is used.
                    type = Instructions::ConversionType::Extend; // Treat same size float conversion as an extend, it will just copy the value and change the type.
                }
            } else {
                throw std::runtime_error("Invalid conversion types");
            }

            Instructions::ConversionSign sign = source.GetType()->IsSigned() ? Instructions::ConversionSign::Signed : Instructions::ConversionSign::Unsigned;

            return addConversion(source, targetType, type, sign);
        }

        [[nodiscard]] bool isTerminated() const {
            if (instructions.empty()) {
                return false;
            }
            auto* lastInstruction = instructions.back();
            return dynamic_cast<Instructions::Return*>(lastInstruction) != nullptr ||
                   (dynamic_cast<Instructions::Jump*>(lastInstruction) != nullptr &&
                    (dynamic_cast<Instructions::Jump*>(lastInstruction))->type != Instructions::JumpType::Unconditional);
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            Utilities::StringBuilder sb;
            sb.AppendLine(fmt::format("@{}", identifier));

            for (const auto& instruction : instructions) {
                sb.AppendLine(fmt::format("\t{}",instruction->Emit(is64Bit)));
            }

            return sb.ToString();
        }

    private:
        static ValueReference nextValueReference(ITypeDefinition* type) {
            auto new_identifier = NameTracker::getNextName();
            auto* local = createLocal(new_identifier, type);
            return ValueReference(local);
        }

        static ValueReference toValueReference(Local* local) {
            if (local == nullptr) {
                throw std::runtime_error("Local cannot be null");
            }
            return ValueReference(local);
        }

        static Local* createLocal(const std::string& identifier, ITypeDefinition* type) {
            if (type == nullptr) {
                throw std::runtime_error("Local type cannot be null");
            }
            auto* local = new Local(identifier, type);
            local->isParameter = false; // Local variables are not parameters by default
            return local;
        }
    };
}


#endif //BLOCK_H
