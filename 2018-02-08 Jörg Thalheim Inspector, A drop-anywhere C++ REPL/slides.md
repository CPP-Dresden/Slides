---
title: 'Inspector: A drop-anywhere C++ REPL'
tags: presentation
slideOptions:
  theme: 'simple'
  transition: 'none'
---

# Inspector

A drop-anywhere C++ REPL

Jörg Thalheim (github.com/Mic92)

Source: https://github.com/inspector-repl/inspector

Note:
- Hi, I am Jörg Thalheim and today I am going you pet project Inspector, a drop-anywhere C++ REPL...
- ...first some words about my person

---

## Me

- PhD in systems research (dependability) in University of Edinburgh (Scotland)
- Projects:
    - [Cntr](https://github.com/Mic92/cntr): Container debugging tool
    - Upcoming: Time-travel debugging

---

## REPL

- Interactive programming environment
- Live cycle
  1. Read
  2. Eval
  3. Print
  4. Loop -> #1
- Examples: ipython, matlab, LISP...

---

## Drop-anywhere REPL 

- like a breakpoint in debugger
- Interact with running program state/local variables

Python:

```python
import pdb; pdb.set_trace()
```

Ruby:

```ruby
require "pry"; binding.pry`
```

Now C/C++:

```c++
#include INSPECTOR
```

---

## Motivating Example

```cpp
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    int a = 1;
    std::string b = "hello world";
#include INSPECTOR
    std::cout << "second break." << std::endl;
#include INSPECTOR
}
```

---

## Inspector Demo

```console
$ ./inspector prebuild main.cpp 
$ clang++ $(./inspector print-cflags) main.cpp -o main 
$ ./inspector repl
$ ./main
```

---

## Inspector

- Product of three-day hackathon
- C++17 REPL, that can be embedded into running programs
- Under the hood: Just-In-Time compiler as a library
- Standing on the shoulder of giants: Cling

---

## Why not $debugger

- Works with default optimization flags
- Access to all variables in its scope (no optimized variables)
- Full programming language support (C++17)
- Live coding, not just poking in memory
- Supports multiple processes

---

## Inspector Design: 1. Pre-Build

Generate hidden header file containing a REPL invocation call based on libclang

```console
$ ./inspector prebuild main.cpp
```

Generated code:

```c++
inspectorRunRepl("main.cpp",
  "<compilation-unit>",
  "int& v1 = *(int*) %p; int& v2= *(int*) %p;", 
  &v1, // <- maybe not side-effect free
  &v2)
```

---

## Inspector Design: 2. Build

```console
$ clang++ $(./inspector print-cflags) main.cpp -o main
```

Inject generic code:
```c
-DINSPECTOR=<inspector/__FILE__-__LINE__>
```

Links dynamic library: libinspector

---

## Inspector Design: 3. Run

Start local prompt server listening on localhost:
```console
$ ./inspector repl
```

Library connects to Server:
```console
$ ./main
```

Prompt server and client library exchange simple JSON messages over TCP

→ Remote debugging possible & multiple threads/processes

---

## Project status

- Works for on my machine(TM)
- Packaging still a pain (requires llvm/clang fork for cling)
- Build system/IDE integration missing
- Looking for contributors/co-maintainers: Maybe you?
- Happy to give presentations

---

# Questions?

Source: https://github.com/inspector-repl/inspector
