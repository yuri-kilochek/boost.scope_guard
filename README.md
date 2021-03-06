# Boost.ScopeGuard

Fully emulates [D][D]'s [scope guard statement][D.ScopeGuardStatement] feature
via C++17's [`std::uncaught_exceptions()`][C++.UncaughtExceptions] with no
dynamic memory allocation and terse syntax via
[class template deduction][C++.ClassTemplateDeduction].

Andrei Alexandrescu explains scope guards in detail in his CppCon 2015 talk
[_Declarative Control Flow_][YouTube.AlexandrescuTalk].

This library is meant to supersede [Boost.ScopeExit][Boost.ScopeExit].

> **NOT YET PART OF BOOST**

## Introduction

Given a resource that lacks built-in [RAII][RAII] capability, e.g. a C-style 
`FILE*`, [`boost::scope_guard`](#ApiReference.Classes) lets you manage its
lifetime in an exception-safe and localized manner:

```C++
FILE* f = std::fopen("my/file", "r");
if (!f) { throw std::runtime_error("failed to open file"); }
boost::scope_guard my_guard = [&]{
    // Invoked when `my_guard` leaves scope, exactly when `f`'s destructor
    // would have been called if it had one.
    std::fclose(f);
};

// ...some code that can potentially throw exceptions...
```

A scope guard can also accept any number of additional arguments that will be
passed to its function upon invocation. This means you can often use 
cleanup functions directly, without wrapping them in lambdas:

```C++
boost::scope_guard my_guard{std::fclose, f};
```

Function and arguments are stored by value by default, you can store them by
reference via [`std::ref`][C++.Ref] and [`std::cref`][C++.Ref]:

```C++
std::thread my_thread(compute_something);
boost::scope_guard my_guard{&std::thread::join, std::ref(my_thread)};
```

Having to name all your scope guard objects so that they don't conflict with
each other quickly gets tiresome, so there is a
[`BOOST_SCOPE_GUARD`](#ApiReference.Macros) macro that does this automatically
(however be aware of its [limitations](#ApiReference.MacroLimitations)):

```C++
BOOST_SCOPE_GUARD {std::fclose, f};
```

Naturally, lambdas are supported as well:

```C++
BOOST_SCOPE_GUARD [&]{ my_thread.join(); };
```

Regular [`boost::scope_guard`](#ApiReference.Classes) always invokes its
stored cleanup function upon destruction, which may not be desirable. There is
also [`boost::scope_guard_failure`](#ApiReference.Classes) that invokes its
stored cleanup function _only_ when it is being destroyed due to
[stack unwinding][C++.StackUnwinding] (i.e. when an exception is thrown) and
[`boost::scope_guard_success`](#ApiReference.Classes) that invokes its stored
cleanup function _only_ when it is being destroyed due to flow of control
leaving the scope normally. Naturally, there are corresponding
[`BOOST_SCOPE_GUARD_FAILURE`](#ApiReference.Macros) and
[`BOOST_SCOPE_GUARD_SUCCESS`](#ApiReference.Macros)

## API Reference

###### Headers

Everything is accessible via

```C++
#include <boost/scope_guard.hpp>
```

#### <a name="ApiReference.Classes">`boost::scope_guard`, `boost::scope_guard_failure` and `boost::scope_guard_success`</a>

###### Constructors

```C++
template <typename Fn, typename... Args>
constexpr
boost::scope_guard::scope_guard(Fn&& fn, Args&&... args)
noexcept(/* see description */);

template <typename Fn, typename... Args>
constexpr
boost::scope_guard_failure::scope_guard_failure(Fn&& fn, Args&&... args)
noexcept(/* see description */);

template <typename Fn, typename... Args>
constexpr
boost::scope_guard_success::scope_guard_success(Fn&& fn, Args&&... args)
noexcept(/* see description */);
```

Stores cleanup function `fn` and its arguments `args...` after
[decaying][C++.Decay] them and unwrapping
[`std::reference_wrapper`s][C++.ReferenceWrapper] (the same way
[`std::make_tuple`][C++.MakeTuple] does). Is defined iff decayed and unwrapped
stored cleanup function and its argments are initializable with `fn` and `args`
respectively and stored cleanup function is [`Callable`][C++.Callable] with its
stored arguments. Is `noexcept` iff initialization of stored cleanup function
and its stored arguments with forwarded `fn` and `args...` respectively is
`noexcept`.

###### Destructors

```C++
// (1)
boost::scope_guard::~scope_guard()
noexcept(/* see description */);

// (2)
boost::scope_guard_failure::~scope_guard_failure()
noexcept(/* see description */);

// (3)
boost::scope_guard_success::~scope_guard_success()
noexcept(/* see description */);
```

Forwards and invokes stored cleanup function with its forwarded stored
arguments,

1. Always.
2. Iff the destructor was called due to [stack unwinding][C++.StackUnwinding]
    (i.e. when an exception is thrown).
3. Iff the destructor was called due to flow of control leaving the scope
    normally.

Any exception thrown by the invocation of stored cleanup function will
propagate out of the destructor (and cause [`std::terminate()`][C++.Terminate]
to be called if the destructor was called due to
[stack unwinding][C++.StackUnwinding]). Is `noexcept`
iff invocation of forwarded stored cleanup function with its forwarded stored
arguments is `noexcept`.

#### <a name="ApiReference.Macros">`BOOST_SCOPE_GUARD`, `BOOST_SCOPE_GUARD_FAILURE` and `BOOST_SCOPE_GUARD_SUCCESS`</a>

```C++
#define BOOST_SCOPE_GUARD
    ::boost::scope_guard $some_name$ =
#define BOOST_SCOPE_GUARD_FAILURE
    ::boost::scope_guard_failure $some_name$ =
#define BOOST_SCOPE_GUARD_SUCCESS
    ::boost::scope_guard_success $some_name$ =
```

where `$some_name$` is an unspecified unique identifier.

###### <a name="ApiReference.MacroLimitations">Limitations</a>

As of C++17, generation of truly unique identifiers in all situations in
standard-conforming and safe manner is not possible. Current implementation
concatenates unspecified token with the value of [`__LINE__`][C++.Line] macro,
so you won't be able to use these macros inside other macros more than once per
scope, since enclosing macro would expand into a single line, generating
identical names.

When used at namespace scope, it is possible that two scope guards appear in 
the same namespace on the same lines of their corresponding source files,
which, too, will generate identical names.

[Boost.ScopeExit]: http://www.boost.org/doc/libs/release/libs/scope_exit/doc/html/index.html
[D]: https://dlang.org/
[D.ScopeGuardStatement]: https://dlang.org/spec/statement.html#scope-guard-statement
[C++.UncaughtExceptions]: http://en.cppreference.com/w/cpp/error/uncaught_exception
[C++.ClassTemplateDeduction]: http://en.cppreference.com/w/cpp/language/class_template_deduction
[YouTube.AlexandrescuTalk]: https://youtu.be/WjTrfoiB0MQ
[RAII]: http://en.cppreference.com/w/cpp/language/raii
[C++.Ref]: http://en.cppreference.com/w/cpp/utility/functional/ref
[C++.StackUnwinding]: http://en.cppreference.com/w/cpp/language/throw#Stack_unwinding
[C++.Decay]: http://en.cppreference.com/w/cpp/types/decay
[C++.ReferenceWrapper]: http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
[C++.MakeTuple]: http://en.cppreference.com/w/cpp/utility/tuple/make_tuple
[C++.Callable]: http://en.cppreference.com/w/cpp/concept/Callable
[C++.Terminate]: http://en.cppreference.com/w/cpp/error/terminate
[C++.Line]: http://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
