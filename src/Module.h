#ifndef MODULE_H
#define MODULE_H
#include <vector>

#include "Function.h"
#include "References.h"
#include "Instructions/Variadic.h"

namespace Qbe {

class Module {
public:
    std::vector<Global*> globals;
    std::vector<Function*> functions;
    std::vector<Function*> definedFunctions; // Functions that have been defined in this module, these have no code blocks.
    std::vector<CustomType*> types;

    bool is64Bit = false;
    Architecture architecture = Architecture::Amd64;

    Module (bool is64Bit = false, Architecture architecture = Architecture::Amd64) : is64Bit(is64Bit), architecture(architecture) {}

    [[nodiscard]] std::string emit();

    ValueReference addGlobal(Qbe::Primitive* primitive, const std::string& value);
    ValueReference addGlobal(const std::string& value);

    Function* addFunction(Function* function);
    Function* addFunction(const std::string& identifier, ITypeDefinition* returnType, std::vector<Local> arguments, bool variadic = false, FunctionFlags flags = FunctionFlags::None);

    Function* defineFunction(const std::string& identifier, ITypeDefinition* returnType, std::vector<Local> arguments, bool variadic = false);

    CustomType* addType(const std::string& identifier, std::vector<CustomTypeField> fields, int align = 0);

    Instructions::VariadicListType *getVariadicListType();
private:
    bool _moduleHasEntry = false;
    Instructions::VariadicListType _variadicListType{architecture};
};

} // Qbe

#endif //MODULE_H
