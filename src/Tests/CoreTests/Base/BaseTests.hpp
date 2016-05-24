#ifndef RADIUM_BASE_TESTS_HPP_
#define RADIUM_BASE_TESTS_HPP_

#include <Tests/CoreTests/Tests.hpp>

class Foo {};
class Bar{ public: class Baz {}; };
namespace Bunny { class Fiddle{}; }


namespace RaTests {
class BaseTests: public Test
{
    void run() override
    {

        // Test the typename mechanism
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<int>()) == "int", "Typename mechanism fail for `int`" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<float>()) == "float", "Typename mechanism fail for `float`" );

        // Testing const, references and pointers.
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<const int>()) == "const int", "Typename mechanism fail for `const`" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<int*>()) == "int*", "Typename mechanism fail for pointer" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<const int*>()) == "const int*", "Typename mechanism fail for pointer to const" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<const int* const>()) == "const int* const", "Typename mechanism fail for pointer const" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<int&>()) == "int&", "Typename mechanism fail for reference" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<const int&>()) == "const int&", "Typename mechanism fail for const reference" );

        // Testing classes and namespaces.
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<Foo>()) == "Foo", "Typename mechanism fail for class" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<Bar::Baz>()) == "Bar::Baz", "Typename mechanism fail for nested class" );
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<Bunny::Fiddle>()) == "Bunny::Fiddle", "Typename mechanism fail for class in namespace" );

        // Testing typedefs
        RA_UNIT_TEST( std::string(CompileTimeUtils::GetTypeName<uint>()) == "unsigned int", "Typename mechanism fail for typedef" );
    }
};
}


#endif //RADIUM_BASE_TESTS_HPP_
