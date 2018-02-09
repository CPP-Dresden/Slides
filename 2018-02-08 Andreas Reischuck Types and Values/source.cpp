#include <cinttypes>
#include <type_traits>

// Types as Values and Values as Types

template<class T>
struct Type {};

template<class T>
struct Type<Type<T>> {
    static_assert((Type<T>{}, false), "Recursion on Type<>!");
};

template<class T>
auto type = Type<T>{};

template<class A, class B>
constexpr bool operator==(Type<A>, Type<B>) {
    return std::is_same_v<A, B>;
}

template<class A, class B>
constexpr bool operator!=(Type<A> a, Type<B> b) {
    return !(a == b);
}

template<class A>
constexpr auto removePointer(Type<A>) -> Type<std::remove_pointer_t<A>> {
    return {};
}

template<class A>
constexpr auto sizeOf(Type<A>) -> size_t {
    return sizeof(A);
}

auto testType() {
    Type<int> someInt;
    static_assert(someInt == type<int32_t>);
    static_assert(sizeOf(someInt) == 4);
}

auto testPointer() {
    auto voidPointer = type<void*>;
    static_assert(type<void> == removePointer(voidPointer));
}

template<class T>
auto unwrap(Type<T>) -> T;

auto testUnwrap() {
    auto intType = type<int>;
    auto value = decltype(unwrap(intType)){23};
    return value;
}

//////////////

template<class... T>
struct TypePack {};

template<class... T>
auto typePack = TypePack<T...>{};

template<class... A, class B>
constexpr auto append(TypePack<A...>, Type<B>) -> TypePack<A..., B> {
    return {};
}

template<class... A, class... B>
constexpr bool operator==(TypePack<A...>, TypePack<B...>) {
    if constexpr (sizeof...(A) != sizeof...(B)) {
        return false;
    }
    else {
        return ((type<A> == type<B>)&&...);
    }
}

auto typePackTest() {
    auto intFloatChar = typePack<int, float, char>;
    auto intFloatCharDouble = append(intFloatChar, type<double>);
    static_assert(
        intFloatCharDouble == typePack<int, float, char, double>);
}

template<class... T, class F>
constexpr auto transform(TypePack<T...>, F&& f)
    -> TypePack<decltype(unwrap(f(type<T>)))...> {
    return {};
}

auto testTransform() {
    auto input = typePack<int, void*>;
    auto output =
        transform(input, [](auto t) { return removePointer(t); });
    static_assert(output == typePack<int, void>);
}

////////////////

template<auto V>
struct Value {
    constexpr static auto v = V;
    using type = decltype(V);

    constexpr operator auto() const { return v; }
};

template<auto (*F)()>
struct Value<F> {
    constexpr static auto v = F();
    using type = decltype(v);

    constexpr operator auto() const { return v; }
};

using One = Value<1>;
using FortyTwo = Value<42>;

template<class A, class B>
using Add = Value<A::v + B::v>;

auto testAdd() { static_assert(Add<One, FortyTwo>::v == 43); }

using Pi = Value<+[] { return 3.14; }>;
using Hello = Value<+[] { return "Hello"; }>;

auto testHello() -> const char* { return Hello{}; }

template<class... A>
using Sum = Value<+[] { return (A::v + ...); }>;

auto testSum() -> double {
    using V2 = Value<+[] { return -23; }>;
    return Sum<FortyTwo, V2, Pi>{};
}

//////////////////

template<class T>
struct Optional { /*…*/
};

template<auto V>
struct Optional<Value<V>> {
    constexpr static auto invalidValue = Value<V>::v;
    using value_type = decltype(invalidValue);

    constexpr auto get() const { return value; }

    constexpr operator bool() const { return value != invalidValue; }

    constexpr auto reset() { value = invalidValue; }
    constexpr auto set(value_type v) { value = v; }

private:
    value_type value{invalidValue};
};

auto testOptional() {
    using DefaultName = Value<+[] { return "Max Mustermann"; }>;
    using OptName = Optional<DefaultName>;

    constexpr auto defaulted = OptName{};
    static_assert(!defaulted);
    static_assert(defaulted.get() == DefaultName::v);
}

///////////////////////

#include <tuple>
#include <utility>
#include <vector>

using Memory = std::vector<uint8_t>;

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

auto increment(int& i) { return i += 23; }

auto testCall() {
    constexpr auto inc = Callable<Ptr(&increment)>(&increment);

    auto m = Memory{};
    m.resize(inc.size);

    int i = 0;
    reinterpret_cast<int*&>(m[0]) = &i;

    inc(m);
    return inc(m);
}

#include <iostream>

int main() { std::cout << testCall() << '\n'; }

enum Token { Hello, World };

auto func() -> CoEnumerator<Token> {
    co_yield Token::Hello;
    co_yield Token::World;
}
