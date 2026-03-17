#ifndef QBEIR_CALL_H
#define QBEIR_CALL_H

#include "Function.h"
#include "NameTracker.h"

#include <fmt/ranges.h>

#include <utility>

namespace Qbe::Instructions {
    class Call : public IInstruction {
    public:
        std::string identifier;
        Qbe::ITypeDefinition *returnType;
        bool isVariadic;
        size_t functionArgumentCount;
        std::vector<ValueReference> arguments;
        ValueReference result; // The result of the function call, if any

        explicit Call(std::string identifier, ITypeDefinition* returnType, bool isVariadic, size_t functionArgumentCount, std::vector<ValueReference> arguments = {}, ValueReference result = ValueReference())
            : identifier(std::move(identifier)), returnType(returnType), isVariadic(isVariadic), functionArgumentCount(functionArgumentCount), arguments(std::move(arguments)), result(std::move(result)) {

            if (returnType == nullptr) {
                throw std::runtime_error("Function return type cannot be null");
            }

            if (result.kind != ValueReferenceKind::Empty && result.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Result must be a local variable or empty");
            }
        }

    protected:
        // %result =(type) call $function((arguments))
        // if empty, %result can be omitted, but ONLY if function returns nothing.

        std::string EmitImpl(bool is64Bit) override {
            Utilities::StringBuilder sb;

            // If the function has a return type, we MUST have a result variable (even if unused)
            if (returnType->IsVoid()) {
                // Ignore the result
                sb.Append("call ");
            } else {
                if (result.kind == ValueReferenceKind::Empty) {
                    throw std::runtime_error("Function has a return type, result cannot be empty");
                }
                sb.Append(fmt::format("{} ={} call ", result.Emit(is64Bit), returnType->GetString(is64Bit)));
            }

            sb.Append(fmt::format("${}(", identifier));

            bool addedVariadicElipses = false;
            int i = 0;
            if (!arguments.empty()) {
                std::vector<std::string> argStrings;
                for (auto& arg : arguments) {
                    if (!addedVariadicElipses && isVariadic && i >= static_cast<int>(functionArgumentCount)) {
                        argStrings.push_back("...");
                        addedVariadicElipses = true;
                    }
                    argStrings.push_back(fmt::format("{} {}", arg.GetType()->GetBodyString(is64Bit), arg.Emit(is64Bit)));
                    i++;
                }
                sb.Append(fmt::format("{}", fmt::join(argStrings, ", ")));
            }
            sb.Append(")");
            return sb.ToString();
        }
    };
}

#endif //QBEIR_CALL_H