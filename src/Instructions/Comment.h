#ifndef QBEIR_COMMENT_H
#define QBEIR_COMMENT_H

namespace Qbe::Instructions {
    class Comment : public IInstruction {
    public:
        std::string text;
        explicit Comment(std::string text) : text(std::move(text)) {}

    protected:
        std::string EmitImpl(bool is64Bit) override {
            // clean up the text so no \n or \t or \r
            text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
            text.erase(std::remove(text.begin(), text.end(), '\t'), text.end());
            text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
            return fmt::format("# {}{}", text, is64Bit);
        }
    };
}

#endif //QBEIR_COMMENT_H