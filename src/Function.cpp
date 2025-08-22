#include "Function.h"
#include "Block.h"

namespace Qbe {
    Block* Function::addBlock(Block *block) {
        blocks.push_back(block);
        return block;
    }

    Block * Function::addBlock(const std::string &name) {
        auto block = new Block(name);
        blocks.push_back(block);
        return block;
    }

    Block* Function::entryPoint() {
        return addBlock(new Block("start"));
    }

    std::string Function::EmitImpl(bool is64Bit) {
        Utilities::StringBuilder sb;

        bool hasExport = ((int)flags & (int)FunctionFlags::Export) != (int)FunctionFlags::None;
        if (hasExport) {
            sb.Append(fmt::format("export "));
        }
        bool hasThread = ((int)flags & (int)FunctionFlags::Thread) != (int)FunctionFlags::None;
        if (hasThread) {
            sb.Append(fmt::format("thread "));
        }
        bool hasEntryPoint = ((int)flags & (int)FunctionFlags::EntryPoint) != (int)FunctionFlags::None;
        if (hasEntryPoint) {
            sb.Append(fmt::format("export "));
        }

        // (flags) function (return type) identifier((parameters)) {
        std::string parametersString;
        for (const auto& param : parameters) {
            parametersString.append(fmt::format("{} {}, ", param.type->GetString(is64Bit), "%" + param.identifier));
        }

        if (isVariadic) {
            parametersString.append("...");
        }
        sb.Append(fmt::format("function {} ${}({}) {{\n", returnType ? returnType->GetString(is64Bit) : "", identifier, parametersString));

        for (auto instruction : blocks) {
            sb.AppendLine(instruction->Emit(is64Bit));
        }
        sb.AppendLine("}");
        return sb.ToString();
    }

    bool Function::IsFunction() const {
        return true;
    }

    bool Function::IsFloat() const {
        return false;
    }

    bool Function::IsInteger() const {
        return false;
    }

    bool Function::IsEqual(const ITypeDefinition &other) const {
        if (other.IsFunction()) {
            auto* otherFunction = dynamic_cast<const Function*>(&other);
            if (otherFunction == nullptr) {
                return false;
            }

            return otherFunction->identifier == identifier;
        }

        return false;
    }

    long Function::ByteSize(bool is64Bit) const {
        return 0; // Functions do not have a byte size in the same way as data types.
    }

    std::string Function::GetString(bool is64Bit) const {
        Utilities::StringBuilder sb;
        sb.Append(fmt::format("{} {}(", returnType ? returnType->GetString(is64Bit) : "void", identifier));
        for (size_t i = 0; i < parameters.size(); ++i) {
            const auto& param = parameters[i];
            sb.Append(fmt::format("{} %{}", param.type->GetString(is64Bit), param.identifier));
            if (i < parameters.size() - 1) {
                sb.Append(", ");
            }
        }
        sb.Append(")");
        return sb.ToString();
    }

    bool Function::IsVoid() const {
        return false;
    }
}
