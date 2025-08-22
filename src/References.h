#ifndef REFERENCES_H
#define REFERENCES_H

#include <sstream>
#include <string>
#include "Types.h"
#include "Interfaces.h"
#include <fmt/core.h>

namespace Qbe {

    class Value : public IEmitter {
    public:
        ITypeDefinition *type = nullptr;
    };

    class Literal : public Value {
    public:
        union {
            int32_t i32;
            int64_t i64;
            float f32;
            double f64;
        } value{};

        explicit Literal(int32_t value) {
            this->value.i32 = value;
            type = new Primitive(TypeDefinitionKind::Int32);
        }

        explicit Literal(int64_t value) {
            this->value.i64 = value;
            type = new Primitive(TypeDefinitionKind::Int64);
        }

        static Literal* CreatePointerLiteral(int64_t value) {
            Literal* literal = new Literal(value, true);
            literal->type = new Primitive(TypeDefinitionKind::Pointer);
            return literal;
        }

        explicit Literal(float value) {
            this->value.f32 = value;
            type = new Primitive(TypeDefinitionKind::Float32);
        }

        explicit Literal(double value) {
            this->value.f64 = value;
            type = new Primitive(TypeDefinitionKind::Float64);
        }

        explicit Literal(uint64_t value, bool is64Bit) {
            if (is64Bit) {
                this->value.i64 = static_cast<int64_t>(value);
                type = new Primitive(TypeDefinitionKind::Int64);
            } else {
                this->value.i32 = static_cast<int32_t>(value);
                type = new Primitive(TypeDefinitionKind::Int32);
            }
        }

    protected:
        [[nodiscard]] std::string EmitImpl(bool is64Bit) override {
            auto* primitive = dynamic_cast<Primitive*>(type);
            std::ostringstream oss;
            switch (primitive->kind) {
                case TypeDefinitionKind::Int32:
                    oss << value.i32;
                    break;
                case TypeDefinitionKind::Int64:
                    oss << value.i64;
                    break;
                case TypeDefinitionKind::Float32:
                    oss << fmt::format("s_{}", value.f32);
                    break;
                case TypeDefinitionKind::Float64:
                    oss << fmt::format("d_{}", value.f64);
                    break;
                case TypeDefinitionKind::Pointer:
                    oss << value.i64;
                    break;
                default:
                    return "unknown";
            }
            return oss.str();
        }
    };

    class Global : public Value {
    public:
        std::string identifier;
        std::string value; // For string values, just the string. For integers, the string representation of the integer. For floats, s_ or d_ prefix.
        bool isString;

        Global(std::string identifier, ITypeDefinition *type, std::string value, bool isString = false)
            : identifier(std::move(identifier)), value(std::move(value)), isString(isString) {
            this->type = type;
        }

        Global(std::string identifier, std::string value): identifier(std::move(identifier)), value(std::move(value)), isString(true) {
            this->type = new Primitive(TypeDefinitionKind::Pointer);
        }

    protected:
        [[nodiscard]] std::string EmitImpl(bool is64Bit) override {
            if (isString) {
                return fmt::format("data ${} = {{ b \"{}\", b 0 }}", identifier, value);
            }

            return fmt::format("data ${} = {{ {} {} }}", identifier, type->GetString(is64Bit), value);
        }
    };

    class Local : public Value {
    public:
        std::string identifier;
        bool isParameter = false;

        explicit Local(std::string identifier, ITypeDefinition *type = nullptr, bool isParameter = false)
            : identifier(std::move(identifier)), isParameter(isParameter) {
            this->type = type;
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("%{}", identifier);
        }
    };

    enum class ValueReferenceKind {
        Local,
        Global,
        Literal,
        Empty
    };

    class ValueReference : public IEmitter {
    public:
        union {
            Local *local;
            Global *global;
            Literal *literal;
        } value{};
        ValueReferenceKind kind;

        explicit ValueReference(Local *local) : kind(ValueReferenceKind::Local) {
            value.local = local;
        }
        explicit ValueReference(Global *global) : kind(ValueReferenceKind::Global) {
            value.global = global;
        }
        explicit ValueReference(Literal *literal) : kind(ValueReferenceKind::Literal) {
            value.literal = literal;
        }
        explicit ValueReference() : kind(ValueReferenceKind::Empty) {
            value.local = nullptr; // Initialize to null for empty reference
        }

        [[nodiscard]] bool IsLocal() const {
            return kind == ValueReferenceKind::Local;
        }

        [[nodiscard]] bool IsGlobal() const {
            return kind == ValueReferenceKind::Global;
        }

        [[nodiscard]] bool IsLiteral() const {
            return kind == ValueReferenceKind::Literal;
        }

        [[nodiscard]] ITypeDefinition *GetType() const {
            if (kind == ValueReferenceKind::Local) {
                return value.local->type;
            }
            if (kind == ValueReferenceKind::Literal) {
                return value.literal->type;
            }
            if (kind == ValueReferenceKind::Global) {
                return value.global->type;
            }
            if (kind == ValueReferenceKind::Empty) {
                return nullptr;
            }
            throw std::runtime_error("Invalid ValueReference kind"); // should never happen but whatever I AM GOING INSANE !!!!! someone tie me up and abuse me plz
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            // If global prefix it with $, if local prefix it with %
            if (kind == ValueReferenceKind::Local) {
                return fmt::format("%{}", value.local->identifier);
            }
            if (kind == ValueReferenceKind::Global) {
                return fmt::format("${}", value.global->identifier);
            }
            if (kind == ValueReferenceKind::Literal) {
                return value.literal->Emit(is64Bit);
            }
            if (kind == ValueReferenceKind::Empty) {
                return ""; // Just return empty string for empty references
            }
            throw std::runtime_error("Invalid ValueReference kind");
        }
    };
}

#endif //REFERENCES_H
