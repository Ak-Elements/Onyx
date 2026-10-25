#include <catch2/catch_test_macros.hpp>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/async/future.h>
#include <onyx/thread/threadpool/threadpool.h>

namespace onyx::threading::Async {

TEST_CASE( "AsyncTask - Basic", "[threading][async]" ) {
    ThreadPool executor;

    AsyncTask< void() > task( []() { std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ); } );

    Future< void > future = task.getFuture();

    executor.post( std::move( task ) );

    future.wait();
    REQUIRE( future.isPending() == false );
    REQUIRE( future.isCancelled() == false );
    REQUIRE( future.isCompleted() );
}

TEST_CASE( "AsyncTask - Complete callback", "[threading][async]" ) {
    using namespace onyx;
    using namespace onyx::threading;

    ThreadPool executor;
    Atomic< bool > taskFinished = false;
    Atomic< bool > completeCallbackCalled = false;

    AsyncTask< void() > task( [ & ]() {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        taskFinished = true;
    } );

    Future< void > future = task.getFuture();

    future.then( [ & ]() {
        REQUIRE( taskFinished == true );
        completeCallbackCalled = true;
    } );

    executor.post( std::move( task ) );

    future.wait();
    REQUIRE( future.isPending() == false );
    REQUIRE( future.isCancelled() == false );
    REQUIRE( future.isCompleted() );
    REQUIRE( taskFinished == true );
    REQUIRE( completeCallbackCalled == true );
}

TEST_CASE( "AsyncTask cancellation", "[threading][async]" ) {
    using namespace onyx;
    using namespace onyx::threading;

    ThreadPool executor;
    AsyncTask< void( std::stop_token ) > task( []( std::stop_token token ) {
        while( token.stop_requested() == false ) {
        }
    } );

    Future< void > future = task.getFuture();

    executor.post( task );

    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

    future.cancel();
    future.wait();
    REQUIRE( future.isPending() == false );
    REQUIRE( future.isCancelled() );
    REQUIRE( future.isCompleted() == false );
}

TEST_CASE( "AsyncTask cancelation callback", "[threading][async]" ) {
    using namespace onyx;
    using namespace onyx::threading;

    ThreadPool executor;

    Atomic< bool > taskFinished = false;
    Atomic< bool > completeCallbackCalled = false;
    Atomic< bool > cancelCallbackCalled = false;

    AsyncTask< void( std::stop_token ) > task( [ & ]( std::stop_token token ) {
        while( token.stop_requested() == false ) {
        }
        taskFinished = token.stop_requested() == false; // will always set TaskFinished to false
    } );

    Future< void > future = task.getFuture();
    future.then( [ & ]() {
        // should not be called
        REQUIRE( taskFinished == true );
        completeCallbackCalled = true;
    } );

    future.onCancel( [ & ]() {
        REQUIRE( taskFinished == false );
        REQUIRE( completeCallbackCalled == false );
        cancelCallbackCalled = true;
    } );

    executor.post( task );

    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );

    future.cancel();
    future.wait();

    REQUIRE( future.isPending() == false );
    REQUIRE( future.isCancelled() );
    REQUIRE( future.isCompleted() == false );
    REQUIRE( taskFinished == false );
    REQUIRE( completeCallbackCalled == false );
    REQUIRE( cancelCallbackCalled == true );
}

TEST_CASE( "AsyncTask continuation", "[threading][async]" ) {
    using namespace onyx;
    using namespace threading;

    ThreadPool executor;
    AsyncTask< void() > task( []() { std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) ); } );

    Future< void > future = task.getFuture();
    AsyncTask< int32_t() > task2( []() {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        return 1;
    } );
    Future< int32_t > future2 = task2.getFuture();

    future.then( [ & ]() { executor.post( task2 ); } );

    executor.post( task );

    int32_t result = future2.get();

    REQUIRE( result == 1 );
    REQUIRE( future2.isPending() == false );
    REQUIRE( future2.isCancelled() == false );
    REQUIRE( future2.isCompleted() );
    REQUIRE( future.isPending() == false );
    REQUIRE( future.isCancelled() == false );
    REQUIRE( future.isCompleted() );
}

} // namespace onyx::threading::Async