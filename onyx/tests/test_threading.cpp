#include <catch2/catch_test_macros.hpp>

#include <onyx/thread/async/future.h>
#include <onyx/thread/threadpool/threadpool.h>

#include <iostream>

namespace onyx::threading::Benchmark {

constexpr onyx::int32_t CONCURRENCY = 16;
constexpr onyx::int32_t REPOST_COUNT = 1000000;

struct Heavy {
    bool verbose;
    std::vector< char > resource;

    Heavy( bool verbose = false )
        : verbose( verbose )
        , resource( 100 * 1024 * 1024 ) {
        if ( verbose ) {
            std::cout << "heavy default constructor" << std::endl;
        }
    }

    Heavy( const Heavy& o )
        : verbose( o.verbose )
        , resource( o.resource ) {
        if ( verbose ) {
            std::cout << "heavy copy constructor" << std::endl;
        }
    }

    Heavy( Heavy&& o ) noexcept
        : verbose( o.verbose )
        , resource( std::move( o.resource ) ) {
        if ( verbose ) {
            std::cout << "heavy move constructor" << std::endl;
        }
    }

    Heavy& operator=( const Heavy& o ) {
        verbose = o.verbose;
        resource = o.resource;
        if ( verbose ) {
            std::cout << "heavy copy operator" << std::endl;
        }
        return *this;
    }

    Heavy& operator=( const Heavy&& o ) noexcept {
        verbose = o.verbose;
        resource = std::move( o.resource );
        if ( verbose ) {
            std::cout << "heavy move operator" << std::endl;
        }
        return *this;
    }

    ~Heavy() {
        if ( verbose ) {
            std::cout << "heavy destructor. " << ( resource.size() ? "Owns resource" : "Doesn't own resource" )
                      << std::endl;
        }
    }
};

struct Job {
    Job( onyx::threading::ThreadPool* pool, onyx::int32_t repostCount, Promise< void >* waiter )
        : m_ThreadPool( pool )
        , m_RepostCount( repostCount )
        , m_Waiter( waiter ) {}

    Job( const Job& other )
        : m_ThreadPool( other.m_ThreadPool )
        , m_RepostCount( other.m_RepostCount )
        , m_Waiter( other.m_Waiter )
        , m_CurrentRepostCount( other.m_CurrentRepostCount ) {}

    // Heavy h;
    void operator()() {
        if ( ++m_CurrentRepostCount < m_RepostCount ) {
            m_ThreadPool->Post( *this );
        } else {
            m_Waiter->SetValue();
        }
    }

  private:
    onyx::threading::ThreadPool* m_ThreadPool = nullptr;
    onyx::int32_t m_RepostCount = 0;
    Promise< void >* m_Waiter = nullptr;

    onyx::int32_t m_CurrentRepostCount = 0;
};

TEST_CASE( "Benchmark generic tasks", "[threading][benchmark]" ) {
    using namespace onyx;
    using namespace onyx::Threading;

    ThreadPool threadPool;
    Promise< void > waiters[ CONCURRENCY ];

    for ( auto& waiter : waiters ) {
        Job job( &threadPool, REPOST_COUNT, &waiter );
        threadPool.Post( job );
    }

    for ( auto& waiter : waiters ) {
        waiter.GetFuture().Wait();
    }

    REQUIRE( true );
}

TEST_CASE( "Idle thread pool", "[threading][benchmark]" ) {
    using namespace onyx;
    using namespace onyx::Threading;

    ThreadPool threadPool;
    std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

    REQUIRE( true );
}

} // namespace onyx::threading::Benchmark
