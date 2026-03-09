#ifndef QBEIR_LOAD_H
#define QBEIR_LOAD_H

namespace Qbe::Instructions {
    class Load : public IInstruction {
    public:
        ValueReference source;
        ValueReference destination;
        ArithmeticSign sign; // Matters for words, ignored for d,s,l

        explicit Load(ValueReference source, ValueReference destination, ArithmeticSign sign = ArithmeticSign::Signed)
            : source(std::move(source)), destination(std::move(destination)), sign(sign) {
            if (source.GetType() == nullptr || !source.GetType()->IsInteger()) {
                throw std::runtime_error("Source must be an integer type (memory address)");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            // (Destination) =(Type) load (Source)
            Utilities::StringBuilder sb;

            if (destination.GetType()->IsEqual(Primitive(TypeDefinitionKind::Int32))) {
                sb.Append(fmt::format("{} ={} load{}{} {}",
                                      destination.Emit(is64Bit),
                                      destination.GetType()->GetString(is64Bit),
                                      sign == ArithmeticSign::Signed ? "s" : "u",
                                      destination.GetType()->GetString(is64Bit),
                                      source.Emit(is64Bit)));
            }
            else {
                sb.Append(fmt::format("{} ={} load{} {}",
                                      destination.Emit(is64Bit),
                                      destination.GetType()->GetString(is64Bit),
                                      destination.GetType()->GetString(is64Bit),
                                      source.Emit(is64Bit)));
            }

            return sb.ToString();
        }
    };
}

#endif //QBEIR_LOAD_H