#ifndef QBEIR_EQUALITY_H
#define QBEIR_EQUALITY_H

namespace Qbe::Instructions {
    enum class EqualityOperation {
        Equal,
        NotEqual,
        GreaterThan,
        GreaterThanOrEqual,
        LessThan,
        LessThanOrEqual,
        Ordered,
        Unordered
    };
    enum class EqualityPrimitive {
        Signed,
        Unsigned,
        Float
    };

    /*
    Comparison instructions return an integer value (either a word or a long), and compare values of arbitrary types. The returned value is 1 if the two operands satisfy the comparison relation, or 0 otherwise. The names of comparisons respect a standard naming scheme in three parts.

       All comparisons start with the letter c.

       Then comes a comparison type. The following types are available for integer comparisons:
           eq for equality
           ne for inequality
           sle for signed lower or equal
           slt for signed lower
           sge for signed greater or equal
           sgt for signed greater
           ule for unsigned lower or equal
           ult for unsigned lower
           uge for unsigned greater or equal
           ugt for unsigned greater

       Floating point comparisons use one of these types:
           eq for equality
           ne for inequality
           le for lower or equal
           lt for lower
           ge for greater or equal
           gt for greater
           o for ordered (no operand is a NaN)
           uo for unordered (at least one operand is a NaN)

       Because floating point types always have a sign bit, all the comparisons available are signed.
       Finally, the instruction name is terminated with a basic type suffix precising the type of the operands to be compared.

   For example, cod (I(dd,dd)) compares two double-precision floating point numbers and returns 1 if the two floating points are not NaNs, or 0 otherwise. The csltw (I(ww,ww)) instruction compares two words representing signed numbers and returns 1 when the first argument is smaller than the second one.
 */

    // Returns 1 if the comparison is true, 0 otherwise.
    class Equality : public IInstruction {
    public:
        EqualityPrimitive primitive;
        EqualityOperation operation;
        ValueReference lhs;
        ValueReference rhs;
        ValueReference destination; // The result of the comparison, always an integer (word or long)

        explicit Equality(EqualityPrimitive primitive, EqualityOperation operation, ValueReference lhs, ValueReference rhs, ValueReference destination)
            : primitive(primitive), operation(operation), lhs(std::move(lhs)), rhs(std::move(rhs)), destination(std::move(destination)) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }
            if (lhs.GetType() == nullptr || rhs.GetType() == nullptr) {
                throw std::runtime_error("LHS and RHS types cannot be null");
            }
            if (!lhs.GetType()->IsEqual(*rhs.GetType())) {
                throw std::runtime_error("LHS and RHS types must be the same");
            }
            if (primitive == EqualityPrimitive::Float && !lhs.GetType()->IsFloat()) {
                throw std::runtime_error("LHS and RHS types must be float for float comparisons");
            }
            if ((primitive == EqualityPrimitive::Signed || primitive == EqualityPrimitive::Unsigned) && !lhs.GetType()->IsInteger()) {
                throw std::runtime_error("LHS and RHS types must be integer for signed/unsigned comparisons");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            Utilities::StringBuilder sb;

            sb.Append(fmt::format("{} ={} c{}{} {}, {}",
                                  destination.Emit(is64Bit),
                                  Qbe::Primitive(TypeDefinitionKind::Pointer).GetString(is64Bit),
                                  operationToString(operation, primitive),
                                  lhs.GetType()->GetString(is64Bit),
                                  lhs.Emit(is64Bit),
                                  rhs.Emit(is64Bit)));

            return sb.ToString();
        }

        static std::string operationToString(EqualityOperation operation, EqualityPrimitive primitive) {
            switch (operation) {
                case EqualityOperation::Equal:
                    // eq
                    return "eq";
                case EqualityOperation::NotEqual:
                    return "ne";
                case EqualityOperation::LessThanOrEqual:
                    return primitive == EqualityPrimitive::Float ? "le" : (primitive == EqualityPrimitive::Signed ? "sle" : "ule");
                case EqualityOperation::LessThan:
                    return primitive == EqualityPrimitive::Float ? "lt" : (primitive == EqualityPrimitive::Signed ? "slt" : "ult");
                case EqualityOperation::GreaterThanOrEqual:
                    return primitive == EqualityPrimitive::Float ? "ge" : (primitive == EqualityPrimitive::Signed ? "sge" : "uge");
                case EqualityOperation::GreaterThan:
                    return primitive == EqualityPrimitive::Float ? "gt" : (primitive == EqualityPrimitive::Signed ? "sgt" : "ugt");
                case EqualityOperation::Ordered:
                    return "o";
                case EqualityOperation::Unordered:
                    return "uo";
            }

            throw std::runtime_error("Unknown equality operation");
        }
    };
}

#endif //QBEIR_EQUALITY_H