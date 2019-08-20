# MCL

Modern Component Library v0.1 By CrownSoft.

MCL Framework is a branch of the RFC framework which allows you to create Windows GUI applications using events and properties. 

The use of events will radically change the way C++ programs are written. Events are alternative to inheritance. It's a trick to allow you to receive the same benefits as inheritance but with less work.

When using events, the user does not need to sub-class in order to customize a component and therefore does not have to be familiar with object-oriented programming concepts such as inheritance and polymorphism. Events are easier than inheritance. More importantly, it enables programmers to get their work done quickly without a lot of repetitive typing.

The components of MCL are modeless, meaning it does not have modes where setting certain properties or calling certain methods is not valid or indeterminate. If a component has a writeable property, it can be changed at any time, and any method should be callable at anytime and produce the expected result.

copy mcl_amalgamated.h and mcl_amalgamated.cpp files into your project and start coding!
Checkout example dir for usage of this framework.

Send your bug reports/suggestions to support@crownsoft.net

Requirements: C++14 compiler with __declspec(property) extension support! (Visual Studio, CLang)
