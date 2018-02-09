
# Types as Values and Values as Types

NOTE: internal setup…

https://godbolt.org

Compiler: 'x86-64 clang (trunk)'
Options: -O3 -std=c++2a -Wall -Wextra -pedantic

Add Output Window to the bottom!
…

Meta programming is a magic world!

Open your mind.

Maybe we find something useful…

## Types as Values

### Who knows C++ templates?

… Wait for response.

### Let's start easy!

A value that represents a type.

Ok we need a struct.

```cpp
struct Type {};
```

So for this is useless. Just an empty struct.

### But if we make it a template…

```cpp
template<class T> struct Type {};
```

…it should become all magic!

```cpp
auto testType() {
  Type<int> someInt;
}
```

Ok compiles. Where is the magic?

### Let's do some operations.

Like compare two types.

```cpp
template<class A, class B>
bool operator==(Type<A>, Type<B>);
```

Note: The arguments need no names. These are just empty structs anyways.

Ok…

### How to implement this?

use STL

```cpp
#include <type_traits>
```

is has the tools.

```cpp
template<class A, class B>
bool operator==(Type<A>, Type<B>) {
    return std::is_same_v<A, B>;
}
```

### Let's use this…

```cpp
#include <cinttypes>

auto testType() {
  Type<int> someInt;
  return someInt == Type<int32_t>{};
}
```

Nice…

### Those curly braces are annoying…

Make a helper…

```cpp
template<class T> auto type = Type<T>{};
```

```cpp
auto testType() {
    Type<int> someInt;
    return someInt == type<int32_t>;
}
```

### Can we do more?

Let's try to remove pointer

```cpp
template<class A>
auto removePointer(Type<A>);
```

### How?

The result again is a type.
STL type_traits has what is takes.

```cpp
template<class A>
auto removePointer(Type<A>)
        -> Type<std::remove_pointer_t<A>> {
    return {};
}
```

### Let's use this

```cpp
auto testPointer() {
    auto voidPointer = type<void*>;
    return type<void> == removePointer(voidPointer);
}
```

This looks like normal C++ code, but it actually works on types.

So we can pack any type into out value wrapper.
Even `void` which would not be instantiatable.

We can up this a notch…

### Force run at compile time.

```cpp
auto testPointer() {
    auto voidPointer = type<void*>;
    static_assert(type<void> == removePointer(voidPointer));
}
```

How would we to this for normal values?

C++11 introduced `constexpr` for that.

(add constexpr until it works…)
```cpp
template<class T> constexpr auto type = Type<T>{};

constexpr bool operator==(Type<A>, Type<B>)

constexpr auto removePointer(Type<A>)
```

### Not equal anyone?

```cpp
    static_assert(type<void> != removePointer(voidPointer));
```

```cpp
template<class A, class B>
constexpr auto operator!=(Type<A> a, Type<B> b) { 
    return !(a == b);
}
```

static_assert should fail!

### sizeOf?

```cpp
template<class T>
constexpr auto sizeOf(Type<T>) { return sizeof(T); }
```

```cpp
    static_assert(sizeOf(type<int32_t>) == 4);
```

Kind of easy now…

### Type<T> is a type again

We should prevent the recursion!

```cpp
template<class T> struct Type<Type<T>> {
    static_assert((Type<T>{}, false), "Recursion on Type<>!");
};
```

### What if need the wrapped type?

```cpp
template<class T> constexpr auto unwrap(Type<T>) -> T;
```

### Try it…

```cpp
auto testUnwrap() {
  auto intType = type<int>;
  auto value = decltype(unwrap(intType)){23};
  return value;
}
```


### Questions on this first part?

(Wait to let it sink!)

…





## How about many types?

Let's use type packs. a list.

```cpp
///////////////////////////////////////////////////////////////////

template<class... T> struct TypePack {};
```

The same thing, now with some dots :)

### Do the tool trick again.

```cpp
template<class... T> constexpr auto typePack = TypePack<T...>{};
```

### Let's try it again

```cpp
auto typePackTest() {
  auto intFloatChar = typePack<int, float, char>;
}
```

### Append to it like a list of types?

```cpp
template<class... A, class B>
auto append(TypePack<A...>, Type<B>);
```

Result is just another TypePack.

```cpp
template<class... A, class B>
constexpr auto append(TypePack<A...>, Type<B>)
        -> TypePack<A..., B> {
    return {};
}
```

### Try it!

```cpp
auto typePackTest() {
  auto intFloatChar = typePack<int, float, char>;
  auto intFloatCharDouble = append(intFloatChar, type<double>);
}
```

It compiles but is it working?

### Compare two packs

```cpp
template<class... A, class.. B>
constexpr bool operator==(TypePack<A...>, TypePack<B...>);
```

So the size has to be equal and all types.

```cpp
template<class... A, class... B>
constexpr bool operator==(TypePack<A...>, TypePack<B...>) {
    if constexpr (sizeof...(A) != sizeof...(B)) {
        return false;
    } 
    else {
        return ((type<A> == type<B>) && ...);
    }
}
```

### Use it

```cpp
auto typePackTest() {
    auto intFloatChar = typePack<int, float, char>;
    auto intFloatCharDouble = append(intFloatChar, type<double>);
    return intFloatCharDouble == typePack<int, float, char, double>;
}
```

### transform all types in a pack

```cpp
template<class... T, class F>
constexpr auto transform(TypePack<T...>, F&& f);
```

f should be `Type<A>` -> `Type<B>`.
* so we wrap each `T` into a `Type<T>`.
* and use `decltype(unwrap())` to get B.

```cpp
template<class... T, class F>
constexpr auto transform(TypePack<T...>, F&& f)
        -> TypePack<decltype(unwrap(f(type<T>))) ...> {
    return {};
}
```

### Try it out!

```cpp
auto testTransform() {
    auto input = typePack<int, void*>;
    auto output = transform(input, [](auto t) {
        return removePointer(t);
    });
    return output == typePack<int, void>;
}
```

### Summary

So we can use types and pack of types as values in most scenarios.

Questions?





## Values as Types

### integral_constant

STL contains integral_constant.

Looks a bit like this.

```cpp
template<class T, T V>
struct integral_constant {
    constexpr static T value = V;
    using value_type = T;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator() () const noexcept { return value; }
};
```

### Better with C++17

We get `auto` for template arguments with C++17.

```cpp
///////////////////////////////////////////////////////////////////

template<auto V>
struct Value {
    static constexpr auto v = V;
    using value_type = decltype(V);
    constexpr operator auto() const noexcept { return v; }
};
```

### Using this

Effectively values are now types.

```cpp
using One = Value<1>;
using FortyTwo = Value<42>;
```

### Let's do some adding

```cpp
template<class A, class B>
using Add = Value<A::v + B::v>;
```

### Test it

```cpp
auto testAdd() -> int {
    return Add<One, FortyTwo>{};
}
```

### Let's try different values

Pi anybody?

```cpp
using Pi = Value<3.14>;
```

… or Hello

```cpp
using Hello = Value<"Hello">;
```

Oh. No!

Only some types are allowed as template arguments.
Too bad!

### Any Solutions?

1. Function pointer are allowed.
2. Lambdas without capture deduce to function pointers.

Use this as a specialisation:

```cpp
template<auto (*F)()>
struct Value<F> {
    constexpr static auto v = F();
    constexpr operator auto() const noexcept { return v; }
};
```

### Use it.

```cpp
using Pi = Value<+[] { return 3.142; }>;
using Hello = Value<+[]{ return "Hello"; }>;
```

```cpp
auto testHello() -> const char* {
  return Hello{};
}
```

### Now we can build a generic sum.

```cpp
template<class... A>
using Sum = Value<+[]{ return (A::v + ...); }>;
```

### Test it

```cpp
auto testSum() -> double {
    using V2 = Value<+[]{ return -23; }>;
    return Sum<FortyTwo, V2, Pi>{};
}
```

### Summary

A new world opens with these tools.

Questions so far? - Ready for the finale?





## Usage examples

### packed optional

One value marks the optional invalid.

```cpp
///////////////////////////////////////////////////////////////////

template<class T>
struct Optional { /*…*/ };

template<auto V>
struct Optional<Value<V>> {
    constexpr static auto invalidValue = Value<V>::v;
    using value_type = decltype(invalidValue);

    constexpr auto get() const { return value; }

    constexpr operator bool() const {
        return value != invalidValue;
    }

    constexpr auto reset() { value = invalidValue; }
    constexpr auto set(value_type v) { value = v; }

private:
    value_type value{invalidValue};
};
```

Test this.

```cpp
auto testOptional() {
  using DefaultName = Value<+[]{ return "Max Mustermann"; }>;
  using OptName = Optional<DefaultName>;

  constexpr auto defaulted = OptName{};
  static_assert(!defaulted);
  static_assert(defaulted.get() == DefaultName::v);

  return defaulted;
}
```

### memory callable

Make any function callable with a plain memory buffer.
Foundation of every custom ABI (often used for Script-Interfaces).

```cpp
#include <tuple>
#include <utility>
#include <vector>

using Memory = std::vector<uint8_t>;

template<auto F>
struct Callable {
    constexpr auto operator()(Memory& m) const {
        return std::apply(F, …);
    }
};

auto testCall() {
    constexpr auto inc = Callable<+[](int& i) { return i += 23; }>{};
    auto m = Memory{};
    return inc(m);
}
```

`std::apply` works with `std::tuple`

```cpp
template<class R, class... Args>
constexpr auto argumentTuple(R (*)(Args...)) -> std::tuple<Args...> {
    return {};
}

template<auto F>
struct Callable {
    using Tuple = decltype(argumentTuple(F));

    constexpr static auto size = sizeof(Tuple);

    constexpr static auto asArguments(Memory& m) -> Tuple& {
        assert(m.size() == size);
        return reinterpret_cast<Tuple&>(m[0]);
    }

    constexpr auto operator()(Memory& m) const {
        return std::apply(F, asArguments(m));
    }
};

auto testCall() {
    constexpr auto lambda = [](int& i) { return i += 23; };
    static_assert(1 == sizeof(lambda));

    constexpr auto inc = Callable<+lambda>{};
    static_assert(1 == sizeof(inc));

    auto m = Memory{};
    m.resize(inc.size);

    int i = 0;
    *reinterpret_cast<int**>(m.data()) = &i;

    return inc(m);
}
```

* In a real world, we cannot use `std::tuple`
  * We more control over the memory layout.
