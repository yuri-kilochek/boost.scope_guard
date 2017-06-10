# Boost.ScopeGuard

> **NOT YET PART OF BOOST**

Library meant to supersede [Boost.ScopeExit][Boost.ScopeExit].

Fully emulates [D][D]'s [scope guard statement][D.ScopeGuardStatement] feature
via C++17's [`std::uncaught_exceptions()`][C++.UncaughtExceptions] with no
dynamic memory allocation and terse syntax via
[class template deduction][C++.ClassTemplateDeduction].

[Boost.ScopeExit]: http://www.boost.org/doc/libs/release/libs/scope_exit/doc/html/index.html
[D]: https://dlang.org/
[D.ScopeGuardStatement]: https://dlang.org/spec/statement.html#scope-guard-statement
[C++.UncaughtExceptions]: http://en.cppreference.com/w/cpp/error/uncaught_exception
[C++.ClassTemplateDeduction]: http://en.cppreference.com/w/cpp/language/class_template_deduction
