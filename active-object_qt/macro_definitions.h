#ifndef MACRO_DEFINITIONS_H
#define MACRO_DEFINITIONS_H

/// A macro to disallow the copy constructor and operator= functions
/// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)
/*  Example:
===========
class Foo {
 public:
  Foo(int f);
  ~Foo();

 private:
  DISALLOW_COPY_AND_ASSIGN(Foo);
};


is Equal to Example:
====================
class Foo {
 public:
   Foo(int f);
   ~Foo();

 private:
   Foo(const Foo&);
   void operator=(const Foo&);

};*/


#endif // MACRO_DEFINITIONS_H
