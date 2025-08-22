#include <filesystem>
#include <fstream>
#include <iostream>
#include <Qbe.h>

#include "runner.h"
#include "spdlog/spdlog.h"

int main() {
    Qbe::Module module(true);

    auto stringType = module.addType("string", {
        Qbe::CustomTypeField("length", Qbe::Int32),
        Qbe::CustomTypeField("data", Qbe::Pointer)
    });

    auto personType = module.addType("person", {
        Qbe::CustomTypeField("name", stringType),
        Qbe::CustomTypeField("age", Qbe::Int32)
    });

    auto printf = module.defineFunction("printf", Qbe::Int32, {}, true);
    auto scanf = module.defineFunction("scanf", Qbe::Int32, {}, true);
    auto strlen = module.defineFunction("strlen", Qbe::Pointer, { Qbe::Local("stringPointer", Qbe::Pointer) }, false);
    auto malloc = module.defineFunction("malloc", Qbe::Pointer, { Qbe::Local("size", Qbe::Int64) }, false);

    auto readIntegerFunction = module.addFunction("readInteger", Qbe::Int32, {}, false, Qbe::FunctionFlags::None);
    {
        auto block = readIntegerFunction->entryPoint();
        auto integerLocal = block->addAllocate(Qbe::CreateLiteral((Qbe::Int32)->ByteSize(module.is64Bit)));
        block->addCall(scanf, {module.addGlobal("%d"), integerLocal});
        auto loadedInteger = block->addLoad(integerLocal, Qbe::Int32);
        block->addReturn(loadedInteger);
    }

    auto readStringFunction = module.addFunction("readString", Qbe::Pointer, {}, false, Qbe::FunctionFlags::None);
    {
        auto block = readStringFunction->entryPoint();
        auto bufferSize = block->addCopy(Qbe::CreateLiteral((int64_t)256));
        auto buffer = block->addCall(malloc, {bufferSize});
        block->addCall(scanf, {module.addGlobal("%s"), buffer});
        block->addReturn(buffer);
    }

    auto printPersonFunction = module.addFunction("printPerson", Qbe::Void, { Qbe::Local("personPointer", Qbe::Pointer) }, false, Qbe::FunctionFlags::None);
    {
        auto block = printPersonFunction->entryPoint();
        auto personParameter = &printPersonFunction->parameters[0];
        auto personPointer = Qbe::ValueReference(personParameter);

        auto age = block->loadFieldAddress(personPointer, personType, "age", module.is64Bit);
        auto stringPointer = block->getFieldAddress(personPointer, personType, "name", module.is64Bit);
        auto nameLength = block->loadFieldAddress(stringPointer, stringType, "length", module.is64Bit);
        auto nameData = block->loadFieldAddress(stringPointer, stringType, "data", module.is64Bit);

        block->addCall(printf, {module.addGlobal("Name: %.*s, Age: %d\\n"), nameLength, nameData, age});

        block->addReturn();
    }

    auto variadicExample = module.addFunction("variadicExample", Qbe::Void, {}, true, Qbe::FunctionFlags::None);
    {
        auto block = variadicExample->entryPoint();

        auto list = block->addVariadicList(module.getVariadicListType(), module.is64Bit);
        block->addVariadicStart(list);

        // We now loop until we hit a null pointer, until that we printf the string pointers!
        auto ncb = variadicExample->addBlock("null_check_block");
        auto nnb = variadicExample->addBlock("non_null_block");
        auto end = variadicExample->addBlock("end");

        auto arg1 = ncb->addVariadicArgument(list, Qbe::Pointer);

        auto isNull = ncb->addEquality(arg1, Qbe::CreatePointer(0), Qbe::Instructions::EqualityOperation::NotEqual, Qbe::Instructions::EqualityPrimitive::Unsigned);
        ncb->addConditionalJump(nnb, isNull, end);
        nnb->addCall(printf, {module.addGlobal("Arg: %s\\n"), arg1});
        nnb->addUnconditionalJump(ncb);

        end->addCall(printf, {module.addGlobal("End of variadic example.\\n")});
        end->addReturn();
    }

    auto main = module.addFunction("main", Qbe::Int32, {}, false, Qbe::FunctionFlags::EntryPoint);
    {
        auto block = main->entryPoint();

        auto personPointer = block->allocateType(personType, module.is64Bit);
        auto namePointer = block->getFieldAddress(personPointer, personType, "name", module.is64Bit); // Of type 'string'

        block->addCall(printf, {module.addGlobal("Enter name: ")});
        auto nameData = block->addCall(readStringFunction);
        auto nameLength = block->addCall(strlen, {nameData});

        block->addCall(printf, {module.addGlobal("Enter age: ")});
        auto age = block->addCall(readIntegerFunction);

        block->storeFieldAddress(namePointer, stringType, "length", nameLength, module.is64Bit);
        block->storeFieldAddress(namePointer, stringType, "data", nameData, module.is64Bit);
        block->storeFieldAddress(personPointer, personType, "age", age, module.is64Bit);

        block->addCall(printf, {module.addGlobal("Now printing person...\\n")});
        block->addCall(printPersonFunction, {personPointer});

        block->addCall(variadicExample, {module.addGlobal("First"), module.addGlobal("Second"), module.addGlobal("Third")});

        block->addReturn(Qbe::CreateLiteral(0));
    }

    auto output = module.emit();
    spdlog::info("QBE output: \n{}", output);

    runner run(output);
    if (run.run()) {
        spdlog::error("Failed to run the QBE module.");
        return 1;
    }
    spdlog::info("QBE module executed successfully.");

    return 0;
}
