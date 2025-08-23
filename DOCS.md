# QbeGenerator-cpp Documentation

## Overview

This library provides a C++ interface to generate Qbe IR code programmatically.

[Qbe](https://c9x.me/compile/) is a small and simple intermediate representation (IR) designed for writing compilers.

The library allows you to create Qbe modules, define functions, declare global variables, and build instructions using a builder pattern similar to LLVM's IRBuilder.

### Modules
A `Module` is the container containing all functions, globals and type declarations.
Modules can be 32 or 64 bit, and this is specified in the constructor.  
They are self-contained, and can be emitted to a string using the `emit()` method which returns a `std::string`.

In the module you can define types, functions and globals.

```cpp
Qbe::Module module(true); // 64-bit module
```

### Types

---

#### Primitive Types
In Qbe there are only a few primitive types:
- `w` - 32-bit integer (represented by `Qbe::Int32`)
- `l` - 64-bit integer (represented by `Qbe::Int64`)
- `s` - single precision float (represented by `Qbe::Float32`)
- `d` - double precision float (represented by `Qbe::Float64`)

Additionally, there's a QbeGenerator specific `Pointer` type which represents a pointer to either `w` or `l` depending on the module's bitness.  

---

#### Custom Types
You can also define custom types using the `Module.defineType` method.  
This allows you to create custom structs!

```cpp
auto stringType = module.addType("string", {
    Qbe::CustomTypeField("length", Qbe::Int32),
    Qbe::CustomTypeField("data", Qbe::Pointer)
});

auto personType = module.addType("person", {
    Qbe::CustomTypeField("name", stringType),
    Qbe::CustomTypeField("age", Qbe::Int32)
});
```
This will generate the following Qbe code:
```qbe
type :string = { w, l }
type :person = { :string, w }
```

QbeGenerator automatically sorts types, and errors on a circular, or recursive type definition.

---

### Functions
Functions are defined using the `Module.addFunction` method.
You can specify the function name, return type, parameters and flags (like entry point).

To call a function, use the `block->addCall` method, which takes the function to call and a vector of arguments.

```cpp
auto printf = module.defineFunction("printf", Qbe::Int32, {Qbe::Local("fmt", Qbe::Pointer)}, true); // declare printf, with varargs
auto main = module.addFunction("main", Qbe::Int32, {}, false, Qbe::FunctionFlags::EntryPoint);
{
    auto block = main->entryPoint();
    block->addCall(printf, {module.addGlobal("Hello, World!\\n")});
    block->addReturn(Qbe::CreateLiteral(0));
}
```

We use `defineFunction` to declare external functions (like `printf`), and `addFunction` to define our own functions (like `main`).  
You can get parameters by using the `Function->getParameterByName` method. This returns a `ValueReference` representing the parameter.

---

### Globals
Global variables can be added using the `Module.addGlobal` method.  
It is also how you'd add string literals to your module.

```cpp
auto helloWorld = module.addGlobal("Hello, World!\\n");
```

In the above sample helloWorld is a pointer to the string literal.
If your literal is of a different type, you must load it first as `helloWorld` is a pointer.

--- 
### Blocks
A `Block` is a container for instructions.  
The entry point of a function can be accessed using the `Function->entryPoint()` method, which returns a `Block`.  
You can also create additional blocks using the `Function->addBlock` which can be used for branching and loops.  
```cpp
auto loopBlock = main->addBlock("loop");
loopBlock->addUnconditionalJump(loopBlock); // Infinite loop
```
Blocks are where you add instructions using the various methods provided.

---
### Instructions
Instructions are added to a `Block` using various `add*` methods.
For example, to add an addition instruction, you can use `addAdd`:

```cpp
auto result = block->addAdd(Qbe::CreateLiteral(1), Qbe::CreateLiteral(2));
```

Also note that for custom types, there are some extra "wrapper" instructions to make working with them easier:
```cpp
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
```
This allocates a `person` struct, gets the address of the `name` field, and stores values into the `length` and `data` fields of the `string` struct inside it, as well as the `age` field.

--- 
### Q&A
#### How do I compile the generated Qbe code?
You can use the Qbe command line tool to compile the generated code to assembly.
For example, if you have the generated code in a file called `output.qbe`, you can run:
```bash
qbe -o output.s output.qbe
```

#### Where can I find more information about Qbe?
You can find more information about Qbe on its [official website](https://c9x.me/compile/).

---

# License

this project is under the MIT license. see the LICENSE file for details.