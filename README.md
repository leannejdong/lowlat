# Modern C++ Features for high performance low latency systems

This reference consolidate key C++11 to C++26 and library features that improve performance, safety and predictability
for latency sensitive systems.

## Memory management \& Data Handling

* C++11

    - Move semantic (`std::move`, rvalue reference \&\&) -- eliminate copies

    - Smart pointers (`std::unique_ptr`, `std::shared_ptr`) -- RAII for memory safety

    - `std::array` -- fixed sized containers with zero overhead

    - `constexpr` -- compile time computation to reduce runtime work

* C++17

    - std::string_view - zero copy string operations (critical for low latency)

    - structured bindings -- auto \[x, y\] = pair; cleaner decomposition

    - std::optional -- express nullable values without heap allocation

* C++20

    - `std::span` -- lightweight non-owning array-view (zero-copy, bound-safe)

    - Concepts -- constrain templates for better error messages and optimization hints

    - Ranges -- composable algorithms that can be optimized better by compilers

* C++23

    - `std::mdspan` -- multidimensional arrat views SoA patterns

    - `std::expected` -- error handling without exception (critical for latency)

## Concurrentcy and lockfree programming

* C++11

    - std::atomic - lockfree atomic operations

    - Memory ordering (`memory_order_relaxed`, `acquire`, `release`) - fine-grained control

    - `std::thread`, `std::mutex`, `std::conditional_variable` -standard threading

    - Thread-local storage (`thread_local`)

* C++20

    - `std::atomic_ref` -- atomic operations on existing objects

    - `std::jthread` --  auto-joining threads with stop tokens

    - Coroutines (`co_await`, `co_yield`) - for async I/O without blocking

* C++26

    - Hazard pointers - safe lockfree memory reclamation

    - RCU (Read-Copy-Update) - concurrent reads without locks


## Performance optimization

* C++11

    - `noexcept` -- enable more optimization, eliminate exception overlhead

    - `final` -- enable devirtualization optimization

    - Lambda expression -- inline function objects, can be optimized better

* C++14

    - Generic lambda -- `auto` parameters for  template functions

    - `std::make_unique` -- exception safe allocation

* C++17

    - `if constexpr` -- compile-time branching (zeri runtime cost)

    - Fold expression -- variadic template optimization

    - Inline variable -- header only optimization

    - `[[nodiscard]]` -- prevent silent errors with return values

* C++20

    - consteval -- force compile-time evaluation

    - constinit -- ensure static initialization happens at compile-time

    - `[[likely]]` and `[[unlikely]]` -- branch prediction hints

    - three-way comparison `<=>` -- compiler-generated comparison

* C++23

    - `[[assume(expr)]]` -- optimization hints to compiler 

    - `if consteval` -- detect compile-time context


