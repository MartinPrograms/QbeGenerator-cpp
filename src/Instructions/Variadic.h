#ifndef QBEIR_VARIADIC_H
#define QBEIR_VARIADIC_H

namespace Qbe::Instructions{

    class VariadicStart : public IInstruction {
        // vastart (list)
    public:
        ValueReference list;
        explicit VariadicStart(ValueReference list) : list(std::move(list)) {
            if (list.GetType() == nullptr || !list.GetType()->IsEqual(Primitive(TypeDefinitionKind::Pointer))) {
                throw std::runtime_error("List must be of pointer type");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("vastart {}", list.Emit(is64Bit));
        }
    };

    class VariadicArgument : public IInstruction {
    // %result =(type) vaarg (list)
    public:
        ValueReference list;
        ValueReference result;
        explicit VariadicArgument(ValueReference list, ValueReference result) : list(std::move(list)), result(std::move(result)) {
            if (list.GetType() == nullptr || !list.GetType()->IsEqual(Primitive(TypeDefinitionKind::Pointer))) {
                throw std::runtime_error("List must be of pointer type");
            }
            if (result.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Result must be a local variable");
            }
            if (result.GetType() == nullptr) {
                throw std::runtime_error("Result type cannot be null");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("{} ={} vaarg {}",
                               result.Emit(is64Bit),
                               result.GetType()->GetBodyString(is64Bit),
                               list.Emit(is64Bit));
        }
    };

    class VariadicListType : public ITypeDefinition, IEmitter {
    public:
        Architecture architecture;
        explicit VariadicListType(Architecture architecture) : architecture(architecture) {}

        uint64_t GetAlignment(bool is64Bit) const {
            switch (architecture) {
                case Architecture::Amd64:
                case Architecture::Arm64:
                case Architecture::RiscV64:
                    return 8;
                default:
                    throw std::runtime_error("Unsupported architecture for variadic list type");
            }
        }

        uint64_t GetSize(bool is64Bit) const {
            switch (architecture) {
                case Architecture::Amd64:
                    return 24;
                case Architecture::Arm64:
                    return 32;
                case Architecture::RiscV64:
                    return 8;
                default:
                    throw std::runtime_error("Unsupported architecture for variadic list type");
            }
        }

    protected:

        std::string EmitImpl(bool is64Bit) override {
            switch (architecture) {
                case Architecture::Amd64:
                    return "type :valist = align 8 { 24 }";
                case Architecture::Arm64:
                    return "type :valist = align 8 { 32 }";
                case Architecture::RiscV64:
                    return "type :valist = align 8 { 8 }";
                default:
                    throw std::runtime_error("Unsupported architecture for variadic list type");
            }
        }

        [[nodiscard]] std::string GetString(bool is64Bit) const override {
            return ":valist";
        }

        [[nodiscard]] bool IsInteger() const override {
            return false;
        }

        [[nodiscard]] bool IsFloat() const override  {
            return false;
        }

        [[nodiscard]] long ByteSize(bool is64Bit) const override  {
            return false;
        }

        [[nodiscard]] bool IsEqual(const ITypeDefinition& other) const override  {
            return false;
        }

        [[nodiscard]] bool IsFunction() const override  {
            return false;
        }

        [[nodiscard]] bool IsVoid() const override  {
            return false;
        }

        [[nodiscard]] bool IsSigned() const override  {
            return false;
        }

    };
}

#endif //QBEIR_VARIADIC_H