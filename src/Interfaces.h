#ifndef INTERFACES_H
#define INTERFACES_H
#include <string>
#include <vector>

namespace Qbe {
    // This exposes the interfaces used by Qbe IR elements.
    class IEmitter {
    public:
        virtual ~IEmitter() = default;

        [[nodiscard]] std::string Emit(bool is64Bit) {
            return EmitImpl(is64Bit);
        }

    protected:
        [[nodiscard]] virtual std::string EmitImpl(bool is64Bit) = 0;
    };


    class IIdentifiable {
    public:
        virtual ~IIdentifiable() = default;

        [[nodiscard]] virtual std::string GetIdentifier() const = 0;
    };

    enum class TypeDefinitionKind {
        Int32,
        Int64,
        Float32,
        Float64,
        Pointer // This gets turned into 32 or 64 int depending on the architecture.
    };


    enum class Architecture {
        Amd64,
        Arm64,
        RiscV64,
    };

    class ITypeDefinition {
    public:
        virtual ~ITypeDefinition() = default;

        [[nodiscard]] virtual std::string GetString(bool is64Bit) const = 0;
        [[nodiscard]] virtual bool IsInteger() const = 0;
        [[nodiscard]] virtual bool IsSigned() const = 0;
        [[nodiscard]] virtual bool IsFloat() const = 0;
        [[nodiscard]] virtual long ByteSize(bool is64Bit) const = 0;
        [[nodiscard]] virtual bool IsEqual(const ITypeDefinition& other) const = 0;
        [[nodiscard]] virtual bool IsFunction() const = 0;
        [[nodiscard]] virtual bool IsVoid() const = 0;
        [[nodiscard]] bool IsCustomType() const {
            return !IsInteger() && !IsFloat() && !IsVoid() && !IsFunction();
        }
    };

    class IInstruction : public IEmitter {

    };

    enum class FunctionFlags {
        Export = 1 << 0,
        Thread = 1 << 1,
        EntryPoint = 1 << 2,
        None = 0,
    };
}

#endif //INTERFACES_H
