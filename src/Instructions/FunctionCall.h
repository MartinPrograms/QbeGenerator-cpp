#ifndef QBEIR_FUNCTIONCALL_H
#define QBEIR_FUNCTIONCALL_H

namespace Qbe::Instructions {
    class FunctionCall : public IInstruction {
    public:
        ValueReference function;
        std::vector<ValueReference> arguments;
        ValueReference destination; // Optional destination for the return value

        explicit FunctionCall(ValueReference function, std::vector<ValueReference> arguments, ValueReference destination = ValueReference())
            : function(std::move(function)), arguments(std::move(arguments)), destination(std::move(destination)) {
            if (function.GetType() == nullptr || !function.GetType()->IsFunction()) {
                throw std::runtime_error("Function reference must be a valid function type");
            }

            if (destination.kind != ValueReferenceKind::Local && destination.kind == ValueReferenceKind::Empty) {
                throw std::runtime_error("Destination must be a local variable or empty");
            }
        }
    };
}

#endif //QBEIR_FUNCTIONCALL_H