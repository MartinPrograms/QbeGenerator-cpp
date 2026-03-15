#ifndef QBEIR_BLIT_H
#define QBEIR_BLIT_H

// Blit (m,m,w)
/*
The blit instruction copies in-memory data from its first address argument to its second address argument. The third argument is the number of bytes to copy. The source and destination spans are required to be either non-overlapping, or fully overlapping (source address identical to the destination address). The byte count argument must be a nonnegative numeric constant; it cannot be a temporary.

One blit instruction may generate a number of instructions proportional to its byte count argument, consequently, it is recommended to keep this argument relatively small. If large copies are necessary, it is preferable that frontends generate calls to a supporting memcpy function.
*/

namespace Qbe::Instructions {
    class Blit : public IInstruction {
    public:
        ValueReference sourceAddress;
        ValueReference destinationAddress;
        uint64_t byteCount;

        explicit Blit(ValueReference sourceAddress, ValueReference destinationAddress, uint64_t byteCount) : sourceAddress(std::move(sourceAddress)), destinationAddress(std::move(destinationAddress)), byteCount(byteCount) {
            if (sourceAddress.GetType() == nullptr || !sourceAddress.GetType()->IsEqual(Primitive(TypeDefinitionKind::Pointer))) {
                throw std::runtime_error("Source address must be of pointer type");
            }
            if (destinationAddress.GetType() == nullptr || !destinationAddress.GetType()->IsEqual(Primitive(TypeDefinitionKind::Pointer))) {
                throw std::runtime_error("Destination address must be of pointer type");
            }
        }

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("blit {}, {}, {}", sourceAddress.Emit(is64Bit), destinationAddress.Emit(is64Bit), byteCount);
        }
    };
}

#endif //QBEIR_BLIT_H