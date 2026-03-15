#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <variant>

#include "Interfaces.h"
#include "StringBuilder.h"

#include <fmt/core.h>

#include "NameTracker.h"

namespace Qbe {
    class Primitive : public ITypeDefinition {
    public:
        TypeDefinitionKind kind;
        bool isSigned;

        explicit Primitive(TypeDefinitionKind kind, bool isSigned = true) : kind(kind), isSigned(isSigned) {}

        [[nodiscard]] std::string GetString(bool is64Bit) const override {
            switch (kind) {
                case TypeDefinitionKind::Int32:
                    return "w";
                case TypeDefinitionKind::Int64:
                    return "l";
                case TypeDefinitionKind::Float32:
                    return "s";
                case TypeDefinitionKind::Float64:
                    return "d";
                case TypeDefinitionKind::Pointer:
                    return is64Bit ? "l" : "w";
            }
            return "";
        }

        [[nodiscard]] bool IsInteger() const override {
            return kind == TypeDefinitionKind::Int32 || kind == TypeDefinitionKind::Int64
                   || kind == TypeDefinitionKind::Pointer;
        }

        [[nodiscard]] bool IsFloat() const override {
            return kind == TypeDefinitionKind::Float32 || kind == TypeDefinitionKind::Float64;
        }

        [[nodiscard]] long ByteSize(bool is64Bit) const override {
            switch (kind) {
                case TypeDefinitionKind::Int32:
                case TypeDefinitionKind::Float32:
                    return 4;
                case TypeDefinitionKind::Int64:
                case TypeDefinitionKind::Float64:
                case TypeDefinitionKind::Pointer:
                    return is64Bit ? 8 : 4;
            }
            return 0;
        }

        [[nodiscard]] bool IsEqual(const ITypeDefinition& other) const override {
            if (const auto* p = dynamic_cast<const Primitive*>(&other)) {
                return kind == p->kind;
            }
            return false;
        }

        [[nodiscard]] bool IsFunction() const override {
            return false; // Primitive types are not functions.
        }

        [[nodiscard]] bool IsVoid() const override {
            return false;
        }

        [[nodiscard]] bool IsSigned() const override {
            return  isSigned;
        }
    };

    class VoidType : public ITypeDefinition {
        // Just return an empty string for  void type.
    public:
        [[nodiscard]] std::string GetString(bool is64Bit) const override {
            return "";
        }

        [[nodiscard]] bool IsInteger() const override {
            return false;
        }

        [[nodiscard]] bool IsFloat() const override {
            return false;
        }

        [[nodiscard]] long ByteSize(bool is64Bit) const override {
            return 0; // Void type has no size.
        }

        [[nodiscard]] bool IsEqual(const ITypeDefinition& other) const override {
            return dynamic_cast<const VoidType*>(&other) != nullptr;
        }

        [[nodiscard]] bool IsFunction() const override {
            return false; // Void type is not a function.
        }

        [[nodiscard]] bool IsVoid() const override {
            return true;
        }

        [[nodiscard]] bool IsSigned() const override {
            return false;
        }
    };

    class CustomTypeField {
    public:
        /// Unused by the compiler, but can be used in custom code generation.
        /// This can help you get the byte offset later, if needed.
        std::string name;
        ITypeDefinition* value; // Either a primitive type or another custom type.
        CustomTypeField(std::string name, ITypeDefinition* value) : name(std::move(name)), value(value) {
            if (this->name.empty()) {
                throw std::runtime_error("CustomTypeField name cannot be empty");
            }
            if (this->value == nullptr) {
                throw std::runtime_error("CustomTypeField value cannot be null");
            }
        }
    };

    class CustomType : public ITypeDefinition, public IEmitter {
    public:
        std::string identifier;
        int align = 0;
        std::vector<CustomTypeField> fields;

        explicit CustomType(std::string identifier, int align = 0, std::vector<CustomTypeField> fields = {})
            : identifier(std::move(identifier)), align(align), fields(std::move(fields)) {
            if (this->identifier.empty()) {
                throw std::runtime_error("CustomType identifier cannot be empty");
            }
        }

        [[nodiscard]] uint64_t GetByteOffsetOfField(const std::string& fieldName, bool is64Bit) const {
            uint64_t offset = 0;
            for (const auto& field : fields) {
                if (field.name == fieldName) {
                    return offset;
                }
                offset += field.value->ByteSize(is64Bit);
            }
            throw std::runtime_error("Field '" + fieldName + "' not found in custom type '" + identifier + "'");
        }

        [[nodiscard]] const CustomTypeField& GetFieldByName(const std::string& fieldName) const {
            for (const auto& field : fields) {
                if (field.name == fieldName) {
                    return field;
                }
            }
            throw std::runtime_error("Field '" + fieldName + "' not found in custom type '" + identifier + "'");
        }

    protected:

        std::string EmitImpl(bool is64Bit) override {
            // Emit the custom type definition.
            // Example: type :name = align 4 { w, w, :otherStruct, { w, w } }
            Utilities::StringBuilder sb;
            sb.Append(fmt::format("type :{} = ", identifier));
            if (align > 0) {
                sb.Append(fmt::format("align {} ", align));
            }
            sb.Append("{ ");
            for (size_t i = 0; i < fields.size(); i++) {
                sb.Append(fields[i].value->GetString(is64Bit));
                if (i < fields.size() - 1) {
                    sb.Append(", ");
                }
            }
            sb.Append(" }");
            return sb.ToString();
        }

        [[nodiscard]] bool IsFunction() const override {
            return false;
        }

        [[nodiscard]] bool IsFloat() const override {
            return false;
        }

        [[nodiscard]] bool IsInteger() const override {
            return false;
        }

        [[nodiscard]] bool IsSigned() const override {
            return false;
        }

        [[nodiscard]] bool IsEqual(const ITypeDefinition& other) const override {
            if (const auto* custom = dynamic_cast<const CustomType*>(&other)) {
                return identifier == custom->identifier; // TODO: implement field comparison, no two types should have the same identifier, though.
            }
            return false;
        }

        [[nodiscard]] std::string GetString(bool is64Bit) const override {
            return ":" + identifier;
        }

        [[nodiscard]] std::string GetBodyString(bool is64Bit) const override {
            return Primitive(TypeDefinitionKind::Pointer).GetString(is64Bit); // Custom types are represented as pointers in the IR body.
        }

        [[nodiscard]] long ByteSize(bool is64Bit) const override {
            long size = 0;
            for (const auto& field : fields) {
                auto s = NameTracker::pushStackIndex();
                if (s > 1000) {
                    throw std::runtime_error("Possible infinite recursion detected in CustomType::ByteSize for type '" + identifier + "'. Did you create a recursive type?");
                }
                size += field.value->ByteSize(is64Bit);
                NameTracker::popStackIndex();
            }
            return size;
        }

        [[nodiscard]]
        bool IsVoid() const override {
            return false;
        }
    };
}

#endif //TYPES_H
