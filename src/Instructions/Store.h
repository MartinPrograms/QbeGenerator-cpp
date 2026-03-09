#ifndef QBEIR_STORE_H
#define QBEIR_STORE_H

namespace Qbe::Instructions {
    class Store : public IInstruction {
    public:
        ValueReference source;
        ValueReference destination;

        explicit Store(ValueReference source, ValueReference destination)
            : source(std::move(source)), destination(std::move(destination)) {
            if (!destination.GetType()->IsInteger()) {
                throw std::runtime_error("Destination must be an integer type (memory address)");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            // store(type) (source) (destination)
            Utilities::StringBuilder sb;

            sb.Append(fmt::format("store{} {}, {}",
                                  source.GetType()->GetString(is64Bit),
                                  source.Emit(is64Bit),
                                  destination.Emit(is64Bit)));

            return sb.ToString();
        }
    };
}

#endif //QBEIR_STORE_H