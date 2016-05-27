#ifndef RADIUM_BASE_TESTS_HPP_
#define RADIUM_BASE_TESTS_HPP_

#include <Tests/CoreTests/Tests.hpp>

class Foo {};
class Bar{ public: class Baz {}; };
namespace Bunny { class Fiddle{}; }

template <typename T, int N> class Frob {};

// Helper macro
// Note  : doesn't work with templates with multiple arguments because of the coma
#define STATIC_TYPE_STR( T )  std::string(CompileTimeUtils::GetTypeName<T>()) 
namespace RaTests {
class BaseTests: public Test
{
    void run() override
    {

        // Test the typename mechanism
        RA_UNIT_TEST( STATIC_TYPE_STR(int)   == "int", "Typename mechanism fail for `int`" );
        RA_UNIT_TEST( STATIC_TYPE_STR(float) == "float", "Typename mechanism fail for `float`" );



        // Test pointers and references. Account for different spacing convention
        RA_UNIT_TEST( STATIC_TYPE_STR(int*) == "int*" || STATIC_TYPE_STR(int*) == "int *", "Typename mechanism fail for pointer" );
        RA_UNIT_TEST( STATIC_TYPE_STR(int&) == "int&" || STATIC_TYPE_STR(int&) == "int &" ,"Typename mechanism fail for reference" );

        // Testing const. We have to account for both conventions : const T (gcc) and T const (Visual Studio)
        RA_UNIT_TEST( STATIC_TYPE_STR(const int)  == "const int"  || STATIC_TYPE_STR(const int)  == "int const ", "Typename mechanism fail for `const`" );

        // Testing classes and namespaces.
        RA_UNIT_TEST( STATIC_TYPE_STR (Foo) == "Foo" || STATIC_TYPE_STR(Foo) == "class Foo", "Typename mechanism fail for class" );
        RA_UNIT_TEST( STATIC_TYPE_STR(Bar::Baz) == "Bar::Baz" || STATIC_TYPE_STR(Bar::Baz) == "class Bar::Baz", "Typename mechanism fail for nested class" );
        RA_UNIT_TEST( STATIC_TYPE_STR(Bunny::Fiddle) == "Bunny::Fiddle" || STATIC_TYPE_STR(Bunny::Fiddle) == "class Bunny::Fiddle", "Typename mechanism fail for nested class" );

        // Testing typedefs
        RA_UNIT_TEST( STATIC_TYPE_STR(uint) == "unsigned int", "Typename mechanism fail for typedef" );

        // Testing templates
        std::cout << "||" << CompileTimeUtils::GetTypeName<Frob<int, 42>>() << "||" << std::endl;
        RA_UNIT_TEST( (CompileTimeUtils::GetTypeName< Frob<int, 42> >()) == "class Frob<int,42> ", "Typename mechanism fail for template");
    }
}; 
}


#endif //RADIUM_BASE_TESTS_HPP_
