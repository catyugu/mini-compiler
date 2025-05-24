#include "AST.h"

// In C++, it's good practice to provide an out-of-line definition
// for the virtual destructor of a base class, even if it's empty.
// This ensures the vtable (virtual function table) is generated correctly
// in one single place. The `ASTVisitor` also has a virtual destructor.

ASTVisitor::~ASTVisitor() = default;


// Currently, our AST node classes are simple enough that all their
// logic (constructors, accept methods) is defined directly in the
// AST.h header file.

// If any of these methods, like a constructor or an accept method,
// became more complex in the future, their full implementation would go here.
// For now, this file correctly provides the necessary definitions.