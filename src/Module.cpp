#include "Module.h"

#include <utility>

#include "CustomTypeSorter.h"
#include "Function.h"
#include "StringBuilder.h"
#include "NameTracker.h"

namespace Qbe {
    std::string Module::emit() {
        Utilities::StringBuilder sb;
        sb.AppendLine(fmt::format("# Qbe IR Module ({}-bit)", is64Bit ? "64" : "32"));
        sb.AppendLine();

        auto sortedTypes = CustomTypeSorter::SortTypes(types);

        sb.AppendLine(fmt::format("# Types ({}):", types.size()));
        for (auto type : sortedTypes) {
            sb.AppendLine(type->Emit(is64Bit));
        }

        sb.AppendLine(fmt::format("# Globals ({}):", globals.size()));
        for (auto global : globals) {
            sb.AppendLine(global->Emit(is64Bit));
        }

        sb.AppendLine();

        sb.AppendLine(fmt::format("# Functions ({}):", functions.size()));
        for (auto function : functions) {
            sb.AppendLine(function->Emit(is64Bit));
        }

        sb.AppendLine();
        sb.AppendLine("# End of Qbe IR Module");


        return sb.ToString();
    }

    ValueReference Module::addGlobal(Qbe::Primitive* primitive, const std::string& value) {
        if (value.empty()) {
            throw std::runtime_error("Global variable value cannot be empty");
        }

        // Check if a global with the same name already exists
        for (const auto& global : globals) {
            if (global->identifier == value) {
                throw std::runtime_error("Global variable with identifier '" + value + "' already exists");
            }
        }

        auto identifier = NameTracker::getNextName();

        auto* global = new Global(identifier,primitive, value);
        globals.push_back(global);
        return ValueReference(global);
    }

    ValueReference Module::addGlobal(Qbe::Literal* literal) {
        if (literal == nullptr) {
            throw std::runtime_error("Literal cannot be null");
        }

        auto identifier = NameTracker::getNextName();
        auto* global = new Global(identifier, literal->type, literal->Emit(is64Bit));
        globals.push_back(global);
        return ValueReference(global);
    }

    ValueReference Module::addGlobal(const std::string& value) {
        if (value.empty()) {
            throw std::runtime_error("Global variable value cannot be empty");
        }

        auto identifier = NameTracker::getNextName();
        auto* global = new Global(identifier, value);
        globals.push_back(global);
        return ValueReference(global);
    }

    Function* Module::addFunction(Function* function) {
        if (function->returnType == nullptr) {
            throw std::runtime_error("Function return type cannot be null");
        }

        if (function->identifier.empty()) {
            throw std::runtime_error("Function identifier cannot be empty");
        }

        if (function->identifier == "main" && !_moduleHasEntry) {
            // Allow main as an identifier for the entry point function, but only if we don't already have an entry point.
            // We will rename it to main later if it is indeed the entry point.
        } else {
            throw std::runtime_error("Function identifier must be empty for auto-generated names or 'main' for the entry point");
        }

        for (const auto& existingFunction : functions) {
            if (existingFunction->identifier == function->identifier) {
                throw std::runtime_error("Function with identifier '" + function->identifier + "' already exists");
            }
        }

        bool hasEntryPoint = ((int)function->flags & (int)FunctionFlags::EntryPoint) != (int)FunctionFlags::None;
        if (_moduleHasEntry) {
            throw std::runtime_error("Module already has an entry point function");
        }

        if (hasEntryPoint) {
            function->identifier = "main";
            _moduleHasEntry = true;
        }

        functions.push_back(function);
        return function;
    }

    Function * Module::addFunction(const std::string &identifier, ITypeDefinition *returnType,
        std::vector<Local> arguments, bool variadic, FunctionFlags flags) {
        return addFunction(new Function(identifier, returnType,std::move(arguments),variadic,flags));
    }

    Function* Module::defineFunction(const std::string &identifier, ITypeDefinition *primitive, std::vector<Local> arguments, bool variadic) {
        if (identifier.empty()) {
            throw std::runtime_error("Function identifier cannot be empty");
        }

        for (const auto& existingFunction : definedFunctions) {
            if (existingFunction->identifier == identifier) {
                throw std::runtime_error("Function with identifier '" + identifier + "' already exists");
            }
        }

        for (const auto& existingFunction : functions) {
            if (existingFunction->identifier == identifier) {
                throw std::runtime_error("Function with identifier '" + identifier + "' already exists in the module");
            }
        }

        auto* function = new Function(identifier, primitive,std::move(arguments),variadic,FunctionFlags::None);

        definedFunctions.push_back(function);
        return function;
    }

    CustomType * Module::addType(const std::string &identifier, std::vector<CustomTypeField> fields, int align) {
        if (identifier.empty()) {
            throw std::runtime_error("Type identifier cannot be empty");
        }

        for (const auto& existingType : types) {
            if (existingType->identifier == identifier) {
                throw std::runtime_error("Type with identifier '" + identifier + "' already exists");
            }
        }

        auto* type = new CustomType(identifier, align, std::move(fields));
        types.push_back(type);
        return type;
    }

    Instructions::VariadicListType * Module::getVariadicListType() {
        return &_variadicListType;
    }
} // Qbe