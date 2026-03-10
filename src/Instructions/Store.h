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

            // The type is either the destination, if the source is a global (since it might be a pointer), or the source otherwise.
            std::string type;
            if (source.IsGlobal())
                // We are storing a global variable, which is a pointer.
                type = Primitive(TypeDefinitionKind::Pointer).GetString(is64Bit);
            else
                type = source.GetType()->GetString(is64Bit);

            sb.Append(fmt::format("store{} {}, {}",
                                  type,
                                  source.Emit(is64Bit),
                                  destination.Emit(is64Bit)));

            return sb.ToString();
        }
    };
}

#endif //QBEIR_STORE_H