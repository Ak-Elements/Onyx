#include <catch2/catch_test_macros.hpp>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/async/future.h>
#include <onyx/thread/threadpool/threadpool.h>

namespace onyx::threading::Async {

TEST_CASE( "AsyncTask - Basic", "[threading][async]" ) {
    ThreadPool executor;

    AsyncTask< void() > task( []() { std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ); } );

    Future< void > future = task.GetFuture();

    executor.Post( std::move( task ) );

    future.Wait();
    REQUIRE( future.IsPending() == false );
    REQUIRE( future.IsCancelled() == false );
    REQUIRE( future.IsCompleted() );
}

TEST_CASE( "AsyncTask - Complete callback", "[threading][async]" ) {
    using namespace onyx;
    using namespace onyx::Threading;

    ThreadPool executor;
    Atomic< bool > taskFinished = false;
    Atomic< bool > completeCallbackCalled = false;

    AsyncTask< void() > task( [ & ]() {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        taskFinished = true;
    } );

    Future< void > future = task.GetFuture();

    future.Then( [ & ]() {
        REQUIRE( taskFinished == true );
        completeCallbackCalled = true;
    } );

    executor.Post( std::move( task ) );

    future.Wait();
    REQUIRE( future.IsPending() == false );
    REQUIRE( future.IsCancelled() == false );
    REQUIRE( future.IsCompleted() );
    REQUIRE( taskFinished == true );
    REQUIRE( completeCallbackCalled == true );
}

TEST_CASE( "AsyncTask cancellation", "[threading][async]" ) {
    using namespace onyx;
    using namespace onyx::Threading;

    ThreadPool executor;
    AsyncTask< void( std::stop_token ) > task( []( std::stop_token token ) {
        while ( token.stop_requested() == false ) {
        }
    } );

    Future< void > future = task.GetFuture();

    executor.Post( task );

    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

    future.Cancel();
    future.Wait();
    REQUIRE( future.IsPending() == false );
    REQUIRE( future.IsCancelled() );
    REQUIRE( future.IsCompleted() == false );
}

TEST_CASE( "AsyncTask cancelation callback", "[threading][async]" ) {
    using namespace onyx;
    using namespace onyx::Threading;

    ThreadPool executor;

    Atomic< bool > taskFinished = false;
    Atomic< bool > completeCallbackCalled = false;
    Atomic< bool > cancelCallbackCalled = false;

    AsyncTask< void( std::stop_token ) > task( [ & ]( std::stop_token token ) {
        while ( token.stop_requested() == false ) {
        }
        taskFinished = token.stop_requested() == false; // will always set TaskFinished to false
    } );

    Future< void > future = task.GetFuture();
    future.Then( [ & ]() {
        // should not be called
        REQUIRE( taskFinished == true );
        completeCallbackCalled = true;
    } );

    future.OnCancel( [ & ]() {
        REQUIRE( taskFinished == false );
        REQUIRE( completeCallbackCalled == false );
        cancelCallbackCalled = true;
    } );

    executor.Post( task );

    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

    future.Cancel();
    future.Wait();

    REQUIRE( future.IsPending() == false );
    REQUIRE( future.IsCancelled() );
    REQUIRE( future.IsCompleted() == false );
    REQUIRE( taskFinished == false );
    REQUIRE( completeCallbackCalled == false );
    REQUIRE( cancelCallbackCalled == true );
}

TEST_CASE( "AsyncTask continuation", "[threading][async]" ) {
    using namespace onyx;
    using namespace Threading;

    ThreadPool executor;
    AsyncTask< void() > task( []() { std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) ); } );

    Future< void > future = task.GetFuture();
    AsyncTask< int32_t() > task2( []() {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        return 1;
    } );
    Future< int32_t > future2 = task2.GetFuture();

    future.Then( [ & ]() { executor.Post( task2 ); } );

    executor.Post( task );

    int32_t result = future2.Get();

    REQUIRE( result == 1 );
    REQUIRE( future2.IsPending() == false );
    REQUIRE( future2.IsCancelled() == false );
    REQUIRE( future2.IsCompleted() );
    REQUIRE( future.IsPending() == false );
    REQUIRE( future.IsCancelled() == false );
    REQUIRE( future.IsCompleted() );
}

} // namespace onyx::threading::Async