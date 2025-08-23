#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include <fmt/core.h>
#include "Interfaces.h"
#include "Types.h"
#include "References.h"

namespace Qbe {
    class Block;

    class Function : public IEmitter, public ITypeDefinition, public IIdentifiable {
    public:
        std::string identifier;

        ITypeDefinition *returnType;
        std::vector<Local> parameters;
        FunctionFlags flags = FunctionFlags::None;
        std::vector<Block*> blocks;  // Pointer is fine with forward declaration
        bool isVariadic = false;

        explicit Function(std::string identifier, ITypeDefinition *returnType, std::vector<Local> parameters, bool isVariadic = false, FunctionFlags flags = FunctionFlags::None)
            : identifier(std::move(identifier)),
              returnType(returnType), parameters(std::move(parameters)), flags(flags), isVariadic(isVariadic) {}

        Block* addBlock(Block *block);
        Block* addBlock(const std::string &name);
        Block* entryPoint();

        Local* getParameterByName(const std::string &name) {
            for (auto &param : parameters) {
                if (param.identifier == name) {
                    return &param;
                }
            }
            return nullptr;
        }
    
    protected:
        std::string EmitImpl(bool is64Bit) override;
        [[nodiscard]] bool IsFunction() const override;
        [[nodiscard]] bool IsFloat() const override;
        [[nodiscard]] bool IsInteger() const override;
        [[nodiscard]] bool IsEqual(const ITypeDefinition &other) const override;
        [[nodiscard]] long ByteSize(bool is64Bit) const override;
        [[nodiscard]] std::string GetString(bool is64Bit) const override;
        [[nodiscard]] std::string GetIdentifier() const override {
            return identifier;
        }
        [[nodiscard]] bool IsVoid() const override;
    };
}

#endif //FUNCTION_H
