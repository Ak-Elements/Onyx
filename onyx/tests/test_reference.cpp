#include <catch2/catch_test_macros.hpp>
#include <onyx/reference.h>

class TestObject : public Onyx::RefCounted
{
public:
    explicit TestObject(int value) : m_Value(value) {}
    int GetValue() const { return m_Value; }

private:
    int m_Value;
};

TEST_CASE("Reference class functionality", "[Reference]")
{
    SECTION("Reference initialization")
    {
        auto ref = Onyx::Reference<TestObject>::Create(42);
        REQUIRE(ref.IsValid());
        REQUIRE(ref->GetValue() == 42);
        REQUIRE(ref->GetRefCount() == 1);
    }

    SECTION("Copy constructor increases reference count")
    {
        auto ref1 = Onyx::Reference<TestObject>::Create(10);
        REQUIRE(ref1->GetRefCount() == 1);

        auto ref2 = ref1;
        REQUIRE(ref1->GetRefCount() == 2);
        REQUIRE(ref2->GetRefCount() == 2);
    }

    SECTION("Move constructor transfers ownership")
    {
        auto ref1 = Onyx::Reference<TestObject>::Create(20);
        REQUIRE(ref1->GetRefCount() == 1);

        auto ref2 = std::move(ref1);
        REQUIRE_FALSE(ref1.IsValid());
        REQUIRE(ref2.IsValid());
        REQUIRE(ref2->GetRefCount() == 1);
    }

    SECTION("Assignment operator works correctly")
    {
        auto ref1 = Onyx::Reference<TestObject>::Create(15);
        REQUIRE(ref1->GetRefCount() == 1);

        auto ref2 = Onyx::Reference<TestObject>::Create(30);
        REQUIRE(ref2->GetRefCount() == 1);

        ref1 = ref2;
        REQUIRE(ref1->GetRefCount() == 2);
        REQUIRE(ref2->GetRefCount() == 2);
        REQUIRE(ref1->GetValue() == 30);
    }

    SECTION("Move assignment transfers ownership")
    {
        auto ref1 = Onyx::Reference<TestObject>::Create(25);
        REQUIRE(ref1->GetRefCount() == 1);

        auto ref2 = Onyx::Reference<TestObject>::Create(50);
        REQUIRE(ref2->GetRefCount() == 1);

        ref1 = std::move(ref2);
        REQUIRE(ref1.IsValid());
        REQUIRE_FALSE(ref2.IsValid());
        REQUIRE(ref1->GetRefCount() == 1);
        REQUIRE(ref1->GetValue() == 50);
    }

    SECTION("Reset releases previous reference")
    {
        auto ref = Onyx::Reference<TestObject>::Create(5);
        REQUIRE(ref->GetRefCount() == 1);

        ref.Reset();
        REQUIRE_FALSE(ref.IsValid());
    }

    SECTION("Release returns raw pointer and invalidates reference")
    {
        auto ref = Onyx::Reference<TestObject>::Create(60);
        REQUIRE(ref->GetRefCount() == 1);

        TestObject* rawPtr = ref.Release();
        REQUIRE_FALSE(ref.IsValid());
        REQUIRE(rawPtr != nullptr);
        REQUIRE(rawPtr->GetValue() == 60);

        delete rawPtr; // Clean up to avoid a memory leak
    }

    SECTION("Invalid reference behaves as expected")
    {
        auto invalidRef = Onyx::Reference<TestObject>::Invalid();
        REQUIRE_FALSE(invalidRef.IsValid());
    }

    SECTION("Reference comparison operators")
    {
        auto ref1 = Onyx::Reference<TestObject>::Create(100);
        auto ref2 = Onyx::Reference<TestObject>::Create(200);
        auto ref3 = ref1;

        REQUIRE(ref1 == ref3);
        REQUIRE(ref1 != ref2);
    }

    SECTION("Polymorphic casting")
    {
        class DerivedObject : public TestObject
        {
        public:
            explicit DerivedObject(int value) : TestObject(value) {}
        };

        auto derivedRef = Onyx::Reference<DerivedObject>::Create(80);
        Onyx::Reference<TestObject> baseRef = derivedRef;

        REQUIRE(baseRef.IsValid());
        REQUIRE(baseRef->GetValue() == 80);

        auto& backToDerived = baseRef.As<DerivedObject>();
        REQUIRE(backToDerived.GetValue() == 80);
    }
}

TEST_CASE("Double-delete prevention", "[Reference]")
{
    struct TestObjectWithDestructor : public Onyx::RefCounted
    {
        TestObjectWithDestructor(bool& val) : destroyed(&val) {}
        ~TestObjectWithDestructor() { *destroyed = true; }

        bool* destroyed = nullptr;
    };

    SECTION("No double delete with multiple references")
    {
        bool destroyed = false;
        auto ref1 = Onyx::Reference<TestObjectWithDestructor>::Create(destroyed);
        REQUIRE(ref1.IsValid());
        REQUIRE_FALSE(destroyed);

        auto ref2 = ref1; // Shared ownership
        REQUIRE(ref1->GetRefCount() == 2);
        REQUIRE(ref2->GetRefCount() == 2);

        ref1.Reset(); // Decrement ref count
        REQUIRE(ref2->GetRefCount() == 1);
        REQUIRE_FALSE(destroyed);

        ref2.Reset(); // Fully release the object
        REQUIRE_FALSE(ref2.IsValid());
        REQUIRE(destroyed); // Ensure the destructor was called only once
    }

    SECTION("No double delete with move semantics")
    {
        bool destroyed = false;
        auto ref1 = Onyx::Reference<TestObjectWithDestructor>::Create(destroyed);
        REQUIRE(ref1.IsValid());
        REQUIRE_FALSE(destroyed);

        auto ref2 = std::move(ref1); // Ownership transfer
        REQUIRE_FALSE(ref1.IsValid());
        REQUIRE(ref2.IsValid());
        REQUIRE_FALSE(destroyed);

        ref2.Reset(); // Fully release the object
        REQUIRE_FALSE(ref2.IsValid());
        REQUIRE(destroyed); // Destructor called once
    }

    SECTION("No double delete with release")
    {
        bool destroyed = false;
        auto ref = Onyx::Reference<TestObjectWithDestructor>::Create(destroyed);
        REQUIRE(ref.IsValid());
        REQUIRE_FALSE(destroyed);

        TestObjectWithDestructor* rawPtr = ref.Release();
        REQUIRE_FALSE(ref.IsValid());
        REQUIRE_FALSE(destroyed);

        delete rawPtr; // Manually delete the object
        REQUIRE(destroyed);
    }

    SECTION("No double delete with polymorphic casting")
    {
        bool destroyed = false;

        struct DerivedObject : public TestObjectWithDestructor
        {
            DerivedObject(bool& val) : TestObjectWithDestructor(val){}

        };

        auto derivedRef = Onyx::Reference<DerivedObject>::Create(destroyed);
        REQUIRE(derivedRef.IsValid());
        REQUIRE_FALSE(destroyed);

        Onyx::Reference<TestObjectWithDestructor> baseRef = derivedRef; // Polymorphic assignment
        REQUIRE(baseRef.IsValid());
        REQUIRE(derivedRef->GetRefCount() == 2);

        derivedRef.Reset(); // Decrease ref count but object still held by baseRef
        REQUIRE(baseRef->GetRefCount() == 1);
        REQUIRE_FALSE(destroyed);

        baseRef.Reset(); // Fully release the object
        REQUIRE_FALSE(baseRef.IsValid());
        REQUIRE(destroyed);
    }

    SECTION("Multiple moves and resets do not double delete")
    {
        bool destroyed = false;

        auto ref = Onyx::Reference<TestObjectWithDestructor>::Create(destroyed);
        REQUIRE(ref.IsValid());
        REQUIRE_FALSE(destroyed);

        auto ref2 = std::move(ref); // Ownership transfer
        REQUIRE_FALSE(ref.IsValid());
        REQUIRE(ref2.IsValid());
        REQUIRE_FALSE(destroyed);

        ref.Reset(); // Resetting an invalid reference does nothing
        REQUIRE_FALSE(ref.IsValid());

        ref2.Reset(); // Fully release the object
        REQUIRE_FALSE(ref2.IsValid());
        REQUIRE(destroyed);
    }

    SECTION("Manually new'd object wrapped in Reference")
    {
        bool destroyed = false;

        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor(destroyed);
        REQUIRE_FALSE(destroyed);

        Onyx::Reference<TestObjectWithDestructor> ref(rawPtr);
        REQUIRE(ref.IsValid());
        REQUIRE(ref->GetRefCount() == 1);

        ref.Reset(); // Should delete the raw pointer
        REQUIRE_FALSE(ref.IsValid());
        REQUIRE(rawPtr->destroyed);
    }

    SECTION("Multiple References to manually new'd object")
    {
        bool destroyed = false;
        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor(destroyed);
        REQUIRE_FALSE(destroyed);

        Onyx::Reference<TestObjectWithDestructor> ref1(rawPtr);
        REQUIRE(ref1.IsValid());
        REQUIRE(ref1->GetRefCount() == 1);

        {
            Onyx::Reference<TestObjectWithDestructor> ref2(ref1); // Shared ownership
            REQUIRE(ref1->GetRefCount() == 2);
            REQUIRE(ref2->GetRefCount() == 2);
            REQUIRE_FALSE(destroyed);
        } // ref2 goes out of scope

        REQUIRE(ref1->GetRefCount() == 1);
        REQUIRE_FALSE(destroyed);

        ref1.Reset(); // Should delete the raw pointer
        REQUIRE_FALSE(ref1.IsValid());
        REQUIRE(destroyed);
    }

    SECTION("Manually new'd object moved into Reference")
    {
        bool destroyed = false;
        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor(destroyed);
        REQUIRE_FALSE(destroyed);

        Onyx::Reference<TestObjectWithDestructor> ref(std::move(Onyx::Reference<TestObjectWithDestructor>(rawPtr)));
        REQUIRE(ref.IsValid());
        REQUIRE(ref->GetRefCount() == 1);

        ref.Reset(); // Should delete the raw pointer
        REQUIRE_FALSE(ref.IsValid());
        REQUIRE(destroyed);
    }

    SECTION("Double wrapping of manually new'd object is disallowed")
    {
        bool destroyed = false;
        TestObjectWithDestructor* rawPtr = new TestObjectWithDestructor(destroyed);
        REQUIRE_FALSE(destroyed);

        Onyx::Reference<TestObjectWithDestructor> ref1(rawPtr);
        REQUIRE(ref1.IsValid());
        REQUIRE(ref1->GetRefCount() == 1);

        // Attempting to wrap the raw pointer again in a new Reference
       Onyx::Reference<TestObjectWithDestructor> ref2(rawPtr);
       REQUIRE(ref2.IsValid());
       REQUIRE(ref1->GetRefCount() == 2);
       REQUIRE(ref2->GetRefCount() == 2);

        ref1.Reset();
        REQUIRE_FALSE(ref1.IsValid());
        REQUIRE_FALSE(destroyed);

        ref2.Reset(); // Should delete the raw pointer
        REQUIRE_FALSE(ref2.IsValid());
        REQUIRE(destroyed);
    }
}