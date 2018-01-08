# Boost.ScopeGuard

> **NOT YET PART OF BOOST**

Library meant to supersede [Boost.ScopeExit][Boost.ScopeExit].

Fully emulates [D][D]'s [scope guard statement][D.ScopeGuardStatement] feature
via C++17's [`std::uncaught_exceptions()`][C++.UncaughtExceptions] with no
dynamic memory allocation and terse syntax via
[class template deduction][C++.ClassTemplateDeduction].

Andrei Alexandrescu explains scope guards in detail in his CppCon 2015 talk
[_Declarative Control Flow_][YouTube.AlexandrescuTalk].

## Introduction

Given a resource that lacks built-in [RAII][RAII] capability, e.g. a C-style 
`FILE*`, [`boost::scope_guard`](#ApiReference.ScopeGuard) lets you manage its
lifetime in an exception-safe and localized manner:

```C++
FILE* f = std::fopen("my/file", "r");
if (!f) { throw std::runtime_error("failed to open file"); }
boost::scope_guard my_guard = [&]{
    // Invoked when `my_guard` leaves scope, exactly when `f`'s destructor
    // would have been called if it had one.
    std::fclose(f);
};

// ...some code than can potentially throw exceptions...
```

A scope guard can also accept any number of additional arguments that will be
passed to its function object upon invocation. This means you can often use 
cleanup functions directy, without wrapping them in lambdas:

```C++
boost::scope_guard my_guard{std::fclose, f};
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
BOOST_SCOPE_GUARD [&]{ std::fclose(f); };
```

Regular [`boost::scope_guard`](#ApiReference.ScopeGuard) always
invokes its function object upon destruction, which may not be desirable. There
is also ([`boost::scope_guard_failure`](#ApiReference.ScopeGuardFailure)) that
invokes its function object _only_ when it is being destroyed due to 
[stack unwinding][C++.StackUnwinding] (i.e. when an exception is thrown) and
([`boost::scope_guard_success`](#ApiReference.ScopeGuardSuccess)) that invokes
its function object _only_ when it is being destroyed due to flow of control
leaving the scope normally. These naturally have correspoinding
[`BOOST_SCOPE_GUARD_FAILURE`](#ApiReference.Macros) and
[`BOOST_SCOPE_GUARD_SUCCESS`](#ApiReference.Macros)

## API Reference

#### <a name="ApiReference.ScopeGuard">`boost::scope_guard`</a>

```C++
boost::scope_guard my_guard{function_object, arguments...};     // (1)
boost::scope_guard my_guard = {function_object, arguments...};  // (2)
boost::scope_guard my_guard = function_object;                  // (3)
```

Declares a scope guard object named `my_guard` that will invoke given
`function_object` with `arguments...` (if any) as if via

1. <i>[INVOKE_UNWRAP_DECAY](#ApiReference.InvokeUnwrapDecay)(function_object,
    arguments...)</i>
2. Same as (1).
3. Same as (1), when `arguments...` sequence is empty.

when it is destroyed. Any exception thrown by this invocation will propagate 
out of the guard's destructor (and cause [`std::terminate()`][C++.Terminate] to
be called if the guard is being destroyed due to
[stack unwinding][C++.StackUnwinding]).

#### <a name="ApiReference.ScopeGuardFailure">`boost::scope_guard_failure`</a>

```C++
boost::scope_guard_failure my_guard{function_object, arguments...};     // (1)
boost::scope_guard_failure my_guard = {function_object, arguments...};  // (2)
boost::scope_guard_failure my_guard = function_object;                  // (3)
```

Declares a scope guard object named `my_guard` that will invoke given
`function_object` with `arguments...` (if any) as if via

1. <i>[INVOKE_UNWRAP_DECAY](#ApiReference.InvokeUnwrapDecay)(function_object,
    arguments...)</i>
2. Same as (1).
3. Same as (1), when `arguments...` sequence is empty.

when it is destroyed due to [stack unwinding][C++.StackUnwinding] (i.e. when an
exception is thrown). Any exception thrown by this invocation will propagate 
out of the guard's destructor (and cause [`std::terminate()`][C++.Terminate] to
be called).

#### <a name="ApiReference.ScopeGuardSuccess">`boost::scope_guard_success`</a>

```C++
boost::scope_guard_success my_guard{function_object, arguments...};     // (1)
boost::scope_guard_success my_guard = {function_object, arguments...};  // (2)
boost::scope_guard_success my_guard = function_object;                  // (3)
```

Declares a scope guard object named `my_guard` that will invoke given
`function_object` with `arguments...` (if any) as if via

1. <i>[INVOKE_UNWRAP_DECAY](#ApiReference.InvokeUnwrapDecay)(function_object,
    arguments...)</i>
2. Same as (1).
3. Same as (1), when `arguments...` sequence is empty.

when it is destroyed due to flow of control leaving the scope normally. Any
exception thrown by this invocation will propagate out of the guard's
destructor.

#### <a name="ApiReference.Macros">`BOOST_SCOPE_GUARD`, `BOOST_SCOPE_GUARD_FAILURE` and `BOOST_SCOPE_GUARD_SUCCESS`</a>

```C++
#define BOOST_SCOPE_GUARD
    ::boost::scope_guard some_name =
#define BOOST_SCOPE_GUARD_FAILURE
    ::boost::scope_guard_failure some_name =
#define BOOST_SCOPE_GUARD_SUCCESS
    ::boost::scope_guard_success some_name =
```

where `some_name` is an unspecified unique identifier.

###### <a name="ApiReference.MacroLimitations"></a>

Generation of truly unique identifiers in all situations in a purely 
standard manner is not currently possible. The current pure standard
implementation concatenates unspecified token with the value of
[`__LINE__`][C++.Line] macro, so you wont be able to use these macros inside
other macros, since each macro expands into a single line.

When used at namespace scope, it is possible that two scope guards appear in 
the same namespace on the same lines of their corresponsing source files, which
will generate identical names.

On compilers that support it `__COUNTER__` macro is used instead
([GCC][C++.Counter.GCC], [Clang][C++.Counter.CLANG] and
[MSVC][C++.Counter.MSVC] for example, all do). On such compilers these macros
can be used inside other macros safely (at namespace scope collisions are
still possible, if very unlikely).

##### <a name="ApiReference.InvokeUnwrapDecay"><i>INVOKE_UNWRAP_DECAY</i></a>

> TODO



[Boost.ScopeExit]: http://www.boost.org/doc/libs/release/libs/scope_exit/doc/html/index.html
[D]: https://dlang.org/
[D.ScopeGuardStatement]: https://dlang.org/spec/statement.html#scope-guard-statement
[C++.UncaughtExceptions]: http://en.cppreference.com/w/cpp/error/uncaught_exception
[C++.ClassTemplateDeduction]: http://en.cppreference.com/w/cpp/language/class_template_deduction
[YouTube.AlexandrescuTalk]: https://youtu.be/WjTrfoiB0MQ
[RAII]: http://en.cppreference.com/w/cpp/language/raii
[C++.StackUnwinding]: http://en.cppreference.com/w/cpp/language/throw#Stack_unwinding
[C++.Terminate]: http://en.cppreference.com/w/cpp/error/terminate
[C++.Line]: http://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
[C++.Counter.GCC]: https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
[C++.Counter.CLANG]: https://clang.llvm.org/docs/LanguageExtensions.html#builtin-macros
[C++.Counter.MSVC]: https://msdn.microsoft.com/en-us/library/b0084kay.aspx
