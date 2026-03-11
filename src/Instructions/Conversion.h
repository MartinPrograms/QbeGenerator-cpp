#ifndef QBEIR_CONVERSION_H
#define QBEIR_CONVERSION_H

namespace Qbe::Instructions {
    enum class ConversionType {
        Extend,
        Truncate,
        FloatToInt,
        IntToFloat
    };

    enum class ConversionSign {
        Signed,
        Unsigned,
        None
    };

    /*
    Conversions

    Conversion operations change the representation of a value, possibly modifying it if the target type cannot hold the value of the source type. Conversions can extend the precision of a temporary (e.g., from signed 8-bit to 32-bit), or convert a floating point into an integer and vice versa.

    extsw, extuw -- l(w)
    extsh, extuh -- I(ww)
    extsb, extub -- I(ww)
    exts -- d(s)
    truncd -- s(d)
    stosi -- I(ss)
    stoui -- I(ss)
    dtosi -- I(dd)
    dtoui -- I(dd)
    swtof -- F(ww)
    uwtof -- F(ww)
    sltof -- F(ll)
    ultof -- F(ll)

    Extending the precision of a temporary is done using the ext family of instructions. Because QBE types do not specify the signedness (like in LLVM), extension instructions exist to sign-extend and zero-extend a value. For example, extsb takes a word argument and sign-extends the 8 least-significant bits to a full word or long, depending on the return type.

    The instructions exts (extend single) and truncd (truncate double) are provided to change the precision of a floating point value. When the double argument of truncd cannot be represented as a single-precision floating point, it is truncated towards zero.

    Converting between signed integers and floating points is done using stosi (single to signed integer), stoui (single to unsigned integer, dtosi (double to signed integer), dtoui (double to unsigned integer), swtof (signed word to float), uwtof (unsigned word to float), sltof (signed long to float) and ultof (unsigned long to float).

    Because of Subtyping, there is no need to have an instruction to lower the precision of an integer temporary.
    */

    class Conversion : public IInstruction {
    public:
        ValueReference source;
        ValueReference destination;
        ConversionType conversion;
        ConversionSign sign;

        explicit Conversion(ValueReference source, ValueReference destination, ConversionType conversion, ConversionSign sign) : source(std::move(source)), destination(std::move(destination)), conversion(conversion), sign(sign) {
            if (destination.kind != ValueReferenceKind::Local) {
                throw std::runtime_error("Destination must be a local variable");
            }

            if (source.GetType() == nullptr || destination.GetType() == nullptr) {
                throw std::runtime_error("Source and destination types cannot be null");
            }

            if (source.GetType()->IsEqual(*destination.GetType())) {
                throw std::runtime_error("Source and destination types must be different for a conversion");
            }

            if (sign == ConversionSign::None && conversion == ConversionType::Extend && source.GetType()->IsInteger()) {
                throw std::runtime_error("Sign must be specified for extend conversions");
            }

            if (sign == ConversionSign::None && (conversion == ConversionType::FloatToInt || conversion == ConversionType::IntToFloat)) {
                throw std::runtime_error("Sign must be specified for float-int conversions");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            Utilities::StringBuilder sb;
            sb.Append(fmt::format("{} ={} ", destination.Emit(is64Bit), destination.GetType()->GetString(is64Bit)));
            if (conversion == ConversionType::Extend) {
                if (source.GetType()->IsFloat()) {
                    sb.Append(fmt::format("ext{} {}",
                                          source.GetType()->GetString(is64Bit),
                                          source.Emit(is64Bit)));
                }else {
                    sb.Append(fmt::format("ext{}{} {}",
                                          sign == ConversionSign::Signed ? "s" : "u",
                                          source.GetType()->GetString(is64Bit),
                                          source.Emit(is64Bit)));
                }
            }
            else if (conversion == ConversionType::Truncate) {
                sb.Append(fmt::format("truncd {}", source.Emit(is64Bit)));
            }
            else if (conversion == ConversionType::FloatToInt) {
                sb.Append(fmt::format("{}to{}i {}",
                                            source.GetType()->GetString(is64Bit),
                                            sign == ConversionSign::Signed ? "s" : "u",
                                            source.Emit(is64Bit)));
            }
            else if (conversion == ConversionType::IntToFloat) {
                sb.Append(fmt::format("{}{}tof {}",
                                      sign == ConversionSign::Signed ? "s" : "u",
                                      source.GetType()->GetString(is64Bit),
                                      source.Emit(is64Bit)));
            }

            return sb.ToString();
        }
    };
}
#endif //QBEIR_CONVERSION_H