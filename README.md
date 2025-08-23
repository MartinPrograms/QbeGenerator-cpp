# QbeGenerator-cpp

This is an LLVM style builder for [Qbe](https://c9x.me/compile/)!  

Example usage:

```cpp
#include "Qbe.h"

Qbe::Module module(true); // 64-bit module

// declare printf, with varargs
auto printf = module.defineFunction("printf", Qbe::Int32, {Qbe::Local("fmt", Qbe::Pointer)}, true); 

// declare the main function with the entry point flag
auto main = module.addFunction("main", Qbe::Int32, {}, false, Qbe::FunctionFlags::EntryPoint);
{
    auto block = main->entryPoint();
    block->addCall(printf, {module.addGlobal("Hello, World!\\n")});
    block->addReturn(Qbe::CreateLiteral(0));
}

auto qbeCode = module.emit();
```

outputs `Hello, World!` :3

Now you can use the qbe command line tool to compile the generated code to assembly!  
Check out the docs [here](https://github.com/MartinPrograms/QbeGenerator-cpp/blob/master/DOCS.md)