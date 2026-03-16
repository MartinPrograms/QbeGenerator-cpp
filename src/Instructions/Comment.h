#ifndef QBEIR_COMMENT_H
#define QBEIR_COMMENT_H

namespace Qbe::Instructions {
    class Comment : public IInstruction {
    public:
        std::string text;
        explicit Comment(std::string text) : text(std::move(text)) {}

    protected:
        std::string EmitImpl(bool is64Bit) override {
            return fmt::format("# {}", text);
        }
    };
}

#endif //QBEIR_COMMENT_H