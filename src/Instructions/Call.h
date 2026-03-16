#ifndef QBEIR_CALL_H
#define QBEIR_CALL_H

#include "Function.h"
#include "NameTracker.h"

#include <fmt/ranges.h>

namespace Qbe::Instructions {
    class Call : public IInstruction {
    public:
        Function* function;
        std::vector<ValueReference> arguments;
        ValueReference result; // The result of the function call, if any
        explicit Call(Function* function, std::vector<ValueReference> arguments = {}, ValueReference result = ValueReference())
            : function(function), arguments(std::move(arguments)), result(std::move(result)) {
            if (function == nullptr) {
                throw std::runtime_error("Function cannot be null");
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
            if (function->returnType->IsVoid()) {
                // Ignore the result
                sb.Append("call ");
            } else {
                if (result.kind == ValueReferenceKind::Empty) {
                    throw std::runtime_error("Function has a return type, result cannot be empty");
                }
                sb.Append(fmt::format("{} ={} call ", result.Emit(is64Bit), function->returnType->GetString(is64Bit)));
            }

            sb.Append(fmt::format("${}(", function->identifier));

            bool addedVariadicElipses = false;
            int i = 0;
            if (!arguments.empty()) {
                std::vector<std::string> argStrings;
                for (auto& arg : arguments) {
                    if (!addedVariadicElipses && function->isVariadic && i >= static_cast<int>(function->parameters.size())) {
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