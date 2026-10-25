#include <catch2/catch_test_macros.hpp>
#include <onyx/reference.h>

class TestObject : public onyx::RefCounted {
  public:
    explicit TestObject( int value )
        : m_Value( value ) {}
    int getValue() const { return m_Value; }

  private:
    int m_Value;
};

TEST_CASE( "Reference class functionality", "[Reference]" ) {
    SECTION( "Reference initialization" ) {
        auto ref = onyx::Reference< TestObject >::create( 42 );
        REQUIRE( ref.isValid() );
        REQUIRE( ref->getValue() == 42 );
        REQUIRE( ref->getRefCount() == 1 );
    }

    SECTION( "Copy constructor increases reference count" ) {
        auto ref1 = onyx::Reference< TestObject >::create( 10 );
        REQUIRE( ref1->getRefCount() == 1 );

        auto ref2 = ref1;
        REQUIRE( ref1->getRefCount() == 2 );
        REQUIRE( ref2->getRefCount() == 2 );
    }

    SECTION( "Move constructor transfers ownership" ) {
        auto ref1 = onyx::Reference< TestObject >::create( 20 );
        REQUIRE( ref1->getRefCount() == 1 );

        auto ref2 = std::move( ref1 );
        REQUIRE_FALSE( ref1.isValid() );
        REQUIRE( ref2.isValid() );
        REQUIRE( ref2->getRefCount() == 1 );
    }

    SECTION( "Assignment operator works correctly" ) {
        auto ref1 = onyx::Reference< TestObject >::create( 15 );
        REQUIRE( ref1->getRefCount() == 1 );

        auto ref2 = onyx::Reference< TestObject >::create( 30 );
        REQUIRE( ref2->getRefCount() == 1 );

        ref1 = ref2;
        REQUIRE( ref1->getRefCount() == 2 );
        REQUIRE( ref2->getRefCount() == 2 );
        REQUIRE( ref1->getValue() == 30 );
    }

    SECTION( "Move assignment transfers ownership" ) {
        auto ref1 = onyx::Reference< TestObject >::create( 25 );
        REQUIRE( ref1->getRefCount() == 1 );

        auto ref2 = onyx::Reference< TestObject >::create( 50 );
        REQUIRE( ref2->getRefCount() == 1 );

        ref1 = std::move( ref2 );
        REQUIRE( ref1.isValid() );
        REQUIRE_FALSE( ref2.isValid() );
        REQUIRE( ref1->getRefCount() == 1 );
        REQUIRE( ref1->getValue() == 50 );
    }

    SECTION( "Reset releases previous reference" ) {
        auto ref = onyx::Reference< TestObject >::create( 5 );
        REQUIRE( ref->getRefCount() == 1 );

        ref.reset();
        REQUIRE_FALSE( ref.isValid() );
    }

    SECTION( "Release returns raw pointer and invalidates reference" ) {
        auto ref = onyx::Reference< TestObject >::create( 60 );
        REQUIRE( ref->getRefCount() == 1 );

        TestObject* rawPtr = ref.release();
        REQUIRE_FALSE( ref.isValid() );
        REQUIRE( rawPtr != nullptr );
        REQUIRE( rawPtr->getValue() == 60 );

        delete rawPtr; // Clean up to avoid a memory leak
    }

    SECTION( "Invalid reference behaves as expected" ) {
        auto invalidRef = onyx::Reference< TestObject >::invalid();
        REQUIRE_FALSE( invalidRef.isValid() );
    }

    SECTION( "Reference comparison operators" ) {
        auto ref1 = onyx::Reference< TestObject >::create( 100 );
        auto ref2 = onyx::Reference< TestObject >::create( 200 );
        auto ref3 = ref1;

        REQUIRE( ref1 == ref3 );
        REQUIRE( ref1 != ref2 );
    }

    SECTION( "Polymorphic casting" ) {
        class DerivedObject : public TestObject {
          public:
            explicit DerivedObject( int value )
                : TestObject( value ) {}
        };

        auto derivedRef = onyx::Reference< DerivedObject >::create( 80 );
        onyx::Reference< TestObject > baseRef = derivedRef;

        REQUIRE( baseRef.isValid() );
        REQUIRE( baseRef->getValue() == 80 );

        auto& backToDerived = baseRef.as< DerivedObject >();
        REQUIRE( backToDerived.getValue() == 80 );
    }
}

TEST_CASE( "Double-delete prevention", "[Reference]" ) {
    struct TestObjectWithDestructor : public onyx::RefCounted {
        TestObjectWithDestructor( bool& val )
            : destroyed( &val ) {}
        ~TestObjectWithDestructor() { *destroyed = true; }

        bool* destroyed = nullptr;
    };

    SECTION( "No double delete with multiple references" ) {
        bool destroyed = false;
        auto ref1 = onyx::Reference< TestObjectWithDestructor >::create( destroyed );
        REQUIRE( ref1.isValid() );
        REQUIRE_FALSE( destroyed );

        auto ref2 = ref1; // Shared ownership
        REQUIRE( ref1->getRefCount() == 2 );
        REQUIRE( ref2->getRefCount() == 2 );

        ref1.reset(); // Decrement ref count
        REQUIRE( ref2->getRefCount() == 1 );
        REQUIRE_FALSE( destroyed );

        ref2.reset(); // Fully release the object
        REQUIRE_FALSE( ref2.isValid() );
        REQUIRE( destroyed ); // Ensure the destructor was called only once
    }

    SECTION( "No double delete with move semantics" ) {
        bool destroyed = false;
        auto ref1 = onyx::Reference< TestObjectWithDestructor >::create( destroyed );
        REQUIRE( ref1.isValid() );
        REQUIRE_FALSE( destroyed );

        auto ref2 = std::move( ref1 ); // Ownership transfer
        REQUIRE_FALSE( ref1.isValid() );
        REQUIRE( ref2.isValid() );
        REQUIRE_FALSE( destroyed );

        ref2.reset(); // Fully release the object
        REQUIRE_FALSE( ref2.isValid() );
        REQUIRE( destroyed ); // Destructor called once
    }

    SECTION( "No double delete with release" ) {
        bool destroyed = false;
        auto ref = onyx::Reference< TestObjectWithDestructor >::create( destroyed );
        REQUIRE( ref.isValid() );
        REQUIRE_FALSE( destroyed );

        TestObjectWithDestructor* rawPtr = ref.release();
        REQUIRE_FALSE( ref.isValid() );
        REQUIRE_FALSE( destroyed );

        delete rawPtr; // Manually delete the object
        REQUIRE( destroyed );
    }

    SECTION( "No double delete with polymorphic casting" ) {
        bool destroyed = false;

        struct DerivedObject : public TestObjectWithDestructor {
            DerivedObject( bool& val )
                : TestObjectWithDestructor( val ) {}
        };

        auto derivedRef = onyx::Reference< DerivedObject >::create( destroyed );
        REQUIRE( derivedRef.isValid() );
        REQUIRE_FALSE( destroyed );

        onyx::Reference< TestObjectWithDestructor > baseRef = derivedRef; // Polymorphic assignment
        REQUIRE( baseRef.isValid() );
        REQUIRE( derivedRef->getRefCount() == 2 );

        derivedRef.reset(); // Decrease ref count but object still held by baseRef
        REQUIRE( baseRef->getRefCount() == 1 );
        REQUIRE_FALSE( destroyed );

        baseRef.reset(); // Fully release the object
        REQUIRE_FALSE( baseRef.isValid() );
        REQUIRE( destroyed );
    }

    SECTION( "Multiple moves and resets do not double delete" ) {
        bool destroyed = false;

        auto ref = onyx::Reference< TestObjectWithDestructor >::create( destroyed );
        REQUIRE( ref.isValid() );
        REQUIRE_FALSE( destroyed );

        auto ref2 = std::move( ref ); // Ownership transfer
        REQUIRE_FALSE( ref.isValid() );
        REQUIRE( ref2.isValid() );
        REQUIRE_FALSE( destroyed );

        ref.reset(); // Resetting an invalid reference does nothing
        REQUIRE_FALSE( ref.isValid() );

        ref2.reset(); // Fully release the object
        REQUIRE_FALSE( ref2.isValid() );
        REQUIRE( destroyed );
    }

    SECTION( "Manually new'd object wrapped in Reference" ) {
        bool destroyed = false;

        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor( destroyed );
        REQUIRE_FALSE( destroyed );

        onyx::Reference< TestObjectWithDestructor > ref( rawPtr );
        REQUIRE( ref.isValid() );
        REQUIRE( ref->getRefCount() == 1 );

        ref.reset(); // Should delete the raw pointer
        REQUIRE_FALSE( ref.isValid() );
        REQUIRE( rawPtr->destroyed );
    }

    SECTION( "Multiple References to manually new'd object" ) {
        bool destroyed = false;
        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor( destroyed );
        REQUIRE_FALSE( destroyed );

        onyx::Reference< TestObjectWithDestructor > ref1( rawPtr );
        REQUIRE( ref1.isValid() );
        REQUIRE( ref1->getRefCount() == 1 );

        {
            onyx::Reference< TestObjectWithDestructor > ref2( ref1 ); // Shared ownership
            REQUIRE( ref1->getRefCount() == 2 );
            REQUIRE( ref2->getRefCount() == 2 );
            REQUIRE_FALSE( destroyed );
        } // ref2 goes out of scope

        REQUIRE( ref1->getRefCount() == 1 );
        REQUIRE_FALSE( destroyed );

        ref1.reset(); // Should delete the raw pointer
        REQUIRE_FALSE( ref1.isValid() );
        REQUIRE( destroyed );
    }

    SECTION( "Manually new'd object moved into Reference" ) {
        bool destroyed = false;
        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor( destroyed );
        REQUIRE_FALSE( destroyed );

        onyx::Reference< TestObjectWithDestructor > otherRef( rawPtr );
        onyx::Reference< TestObjectWithDestructor > ref( std::move( otherRef ) );
        REQUIRE( ref.isValid() );
        REQUIRE( ref->getRefCount() == 1 );

        ref.reset(); // Should delete the raw pointer
        REQUIRE_FALSE( ref.isValid() );
        REQUIRE( destroyed );
    }

    SECTION( "Double wrapping of manually new'd object is disallowed" ) {
        bool destroyed = false;
        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor( destroyed );
        REQUIRE_FALSE( destroyed );

        onyx::Reference< TestObjectWithDestructor > ref1( rawPtr );
        REQUIRE( ref1.isValid() );
        REQUIRE( ref1->getRefCount() == 1 );

        // Attempting to wrap the raw pointer again in a new Reference
        onyx::Reference< TestObjectWithDestructor > ref2( rawPtr );
        REQUIRE( ref2.isValid() );
        REQUIRE( ref1->getRefCount() == 2 );
        REQUIRE( ref2->getRefCount() == 2 );

        ref1.reset();
        REQUIRE_FALSE( ref1.isValid() );
        REQUIRE_FALSE( destroyed );

        ref2.reset(); // Should delete the raw pointer
        REQUIRE_FALSE( ref2.isValid() );
        REQUIRE( destroyed );
    }
}