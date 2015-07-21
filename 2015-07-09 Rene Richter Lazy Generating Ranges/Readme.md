# loop.h - lazy generated range-based for values

This header-only library provides lazy generated values for floating-point, integral and other types in C++14.

## Easy to install and use

* Download [loop.h](https://bitbucket.org/dozric/looprange/raw/tip/loop.h)
  and `#include` it into your project.
  (A C++11 version is [here](https://bitbucket.org/dozric/looprange/raw/tip/c++11/loop.h).)
* Write range-based for loop (see below).
* Compile as C++14 or higher (`g++` or `clang` with option `-std=c++14`).

## Overview
```cpp
#include "loop.h"
// ...
using namespace loop;

// floating-point and complex types
for (auto x : linspace(0, 1.0, 4))   ... // 0 0.25 0.5 0.75 1
for (auto x : linspace(1.0, 0, 4))   ... // 1 0.75 0.5 0.25 0

using namespace std::complex_literals;
for (auto z : loop::linspace(0., 4.+2.i, 4))
                                     ... // (0,0) (1,0.5) (2,1) (3,0.5) (4,2)
// integral types
for (auto i : countdown(5))          ... // 4 3 2 1 0
for (auto i : range(5))              ... // 0 1 2 3 4
for (auto i : range(0, 5))           ... // 0 1 2 3 4
for (auto i : range(0, 6, 2))        ... // 0 2 4
for (auto i : range(0, 6, 2, true))  ... // 0 2 4 6
for (auto i : range(5, 0, -1))       ... // 5 4 3 2 1

// other types
using namespace std::string_literals;
for (auto s : generate("I"s, 5, "E")) ... // : I IE IEE IEEE IEEEE
```
See also: [loop.demo.cpp](loop.demo.cpp)

# A more detailed documentation
## Equispaced floating-point values

`linspace(a, b, n [, boundary])` creates `n+1` values $x_i \in [a,b], i = 0,...,n$ with $x_i = a + i(b-a)/n$ (Fig. 1):
```cpp
using namespace loop;

for (auto x : linspace(0, 1.0, 4)) ... // 0 0.25 0.5 0.75 1
for (auto x : linspace(1.0, 0, 4)) ... // 1 0.75 0.5 0.25 0

for (auto x : linspace(0, 1.0, 4, boundary::closed))    ... // 0 0.25 0.5 0.75 1
for (auto x : linspace(0, 1.0, 4, boundary::rightopen)) ... // 0 0.25 0.5 0.75
for (auto x : linspace(0, 1.0, 4, boundary::leftopen))  ... // 0.25 0.5 0.75 1
for (auto x : linspace(0, 1.0, 4, boundary::open))      ... // 0.25 0.5 0.75
```
When left or/and right boundary value are optionally omitted, the remaining `n` or `n-1` values, respectively, are *not* changed.
Caveat! This differs from `linspace()` in Python's NumPy library and in MATLAB: their implementations always create `n` values.

`linspace()` works for complex numbers and linear algebra types (like Vec3D) which define `u+v`, `u-v`, `u+=v`, `abs(v)`, and multiplication by scalar values:
```cpp
using namespace std::complex_literals;
for (auto z : loop::linspace(0., 4.+2.i, 4))
                                      ... // (0,0) (1,0.5) (2,1) (3,0.5) (4,2)
```
See also: Numpy, MATLAB

![equispaced float values](doc/linspace_real.png)
![equispaced complex values](doc/linspace_complex.png)
Fig. 1: Line of equispaced points `x` in R, C or 3D.

## Generic loop generator

`generate(start, count, increment)` creates `count` values beginning with `start`, successively applying `start += increment`:
```cpp
using namespace loop;

for (auto i : generate(4, 5, -1))                 ... // 4 3 2 1 0
for (auto s : generate(std::string("O"), 5, "i")) ... // O Oi Oii Oiii Oiiii
```
See also: Semenov

## Integral value ranges
`range([start,] stop [, step = 1 [, withend = false]])` creates values beginning with `start` towards `stop` with given `step` size. If `step` direction contradicts the order of `start` and `stop`, the resulting range is empty.
```cpp
using namespace loop;

for (auto i : range(5))             ... // 0 1 2 3 4
for (auto i : range(0, 5))          ... // 0 1 2 3 4
for (auto i : range(5, 0, -1))      ... // 5 4 3 2 1
for (auto i : range(5, 0, 1))       ... //
for (auto i : range(0, 6, 2, true)) ... // 0 2 4 6
for (auto i : countdown(5))         ... // 4 3 2 1 0
```
`countdown(n)` is equivalent to `range(n-1, 0, -1, true)`.
Integral ranges can create indices for standard containers like
`std::vector<T> v`:
```cpp
for (auto i : range(v.size())       ... // 0 ... v.size()-1
for (auto i : range(3, v.size())    ... // 3 ... v.size()-1
for (auto i : countdown(v.size())   ... // v.size()-1 ... 0
```
Mixed signed/unsigned parameters are allowed:
```cpp
for (auto i : range(5u, 0, -1))     ... // 5 4 3 2 1
```
See also: Boost irange(), cppitertools

## Benchmarks
A [benchmark](benchmark/bm_loop.cpp) shows no runtime overhead of lazy generated ranges over best handwritten for loops. Both timings are equal within clock resolution (Fig. 2). Noteworthy, a handwritten and inaccurate `x += dx` loop resulting in wrong loop count is slower than `x = a + i*dx` and the equivalent lazy generated range for `double` values.

![integral type range vs. conventional loop](benchmark/int_loop.png)
![floating type range vs. conventional loop](benchmark/double_loop.png)
Fig. 2: Runtime of handwritten loops and lazy generated ranges.

# TODO, limitations, known bugs

* Test other compilers
* `range(0, SIZE_MAX, 1, true)` is empty

## Links & other libraries

Related work:

* [Numpy linspace()](http://docs.scipy.org/doc/numpy/reference/generated/numpy.linspace.html),  [MATLAB linspace()](http://de.mathworks.com/help/matlab/ref/linspace.html)
* [Boost irange()](http://www.boost.org/doc/libs/1_57_0/libs/range/doc/html/range/reference/ranges/irange.html)
* Ryan Haining: [cppitertools](https://github.com/ryanhaining/cppitertools)
* Mikhail Semenov: [Convenient Constructs For Stepping Through a Range](http://www.codeproject.com/Articles/876156/Convenient-Constructs-For-Stepping-Through-a-Range).

Toolstack:

* TDM GCC 4.9.2 x64 / Debian g++ 4.9.2 and Clang 3.5 on Udoo
* [CMake](http://www.cmake.org)
* Phil Nash: [Catch](https://github.com/philsquared/Catch) Unit Tests
* Nick Athanasiou: [Benchmarking in C++](https://ngathanasiou.wordpress.com/2015/04/01/benchmarking-in-c/)
* [WinPython](http://winpython.sourceforge.net/) for benchmark diagrams.
