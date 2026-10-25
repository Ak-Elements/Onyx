#include <catch2/catch_test_macros.hpp>

#include <ftl/task_scheduler.h>
#include <onyx/thread/async/future.h>
#include <onyx/thread/threadpool/threadpool.h>
#include <TaskScheduler.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <ftl/fiber.h>
#include <iostream>
#include <numeric>
#include <vector>

namespace onyx::threading::benchmark {

constexpr int32_t Concurrency = 16;
constexpr int32_t RepostCount = 10;
constexpr int32_t SampleCount = 5; // number of timed runs per benchmark

struct Heavy {
    bool Verbose;
    std::vector< char > Resource;

    Heavy( bool verbose = false )
        : Verbose( verbose )
        , Resource( 100 * 1024 * 1024 ) {
        if( verbose ) {
            std::cout << "heavy default constructor" << std::endl;
        }
    }

    Heavy( const Heavy& o )
        : Verbose( o.Verbose )
        , Resource( o.Resource ) {
        if( Verbose ) {
            std::cout << "heavy copy constructor" << std::endl;
        }
    }

    Heavy( Heavy&& o ) noexcept
        : Verbose( o.Verbose )
        , Resource( std::move( o.Resource ) ) {
        if( Verbose ) {
            std::cout << "heavy move constructor" << std::endl;
        }
    }

    Heavy& operator=( const Heavy& o ) {
        Verbose = o.Verbose;
        Resource = o.Resource;
        if( Verbose ) {
            std::cout << "heavy copy operator" << std::endl;
        }
        return *this;
    }

    Heavy& operator=( const Heavy&& o ) noexcept {
        Verbose = o.Verbose;
        Resource = std::move( o.Resource );
        if( Verbose ) {
            std::cout << "heavy move operator" << std::endl;
        }
        return *this;
    }

    ~Heavy() {
        if( Verbose ) {
            std::cout << "heavy destructor. " << ( Resource.size() ? "Owns resource" : "Doesn't own resource" )
                      << std::endl;
        }
    }
};

namespace {

struct Stats {
    double Min;
    double Median;
    double Mean;
    double StdDev;
};

Stats ComputeStats( std::vector< double > samples ) {
    std::sort( samples.begin(), samples.end() );

    Stats stats{};
    stats.Min = samples.front();
    stats.Median = samples[ samples.size() / 2 ];

    double sum = std::accumulate( samples.begin(), samples.end(), 0.0 );
    stats.Mean = sum / static_cast< double >( samples.size() );

    double sqSum = 0.0;
    for( double s : samples ) {
        double diff = s - stats.Mean;
        sqSum += diff * diff;
    }
    stats.StdDev = std::sqrt( sqSum / static_cast< double >( samples.size() ) );

    return stats;
}

void PrintSamples( const char* label, int64_t totalTasks, const std::vector< double >& seconds ) {
    for( size_t i = 0; i < seconds.size(); ++i ) {
        double throughput = static_cast< double >( totalTasks ) / seconds[ i ];
        std::printf( "[%-20s] run=%zu time=%8.4fs  throughput=%12.0f tasks/sec\n", label, i, seconds[ i ], throughput );
    }
}

void PrintSummary( const char* label, int64_t totalTasks, const std::vector< double >& seconds ) {
    Stats s = ComputeStats( seconds );

    auto tps = [ totalTasks ]( double sec ) { return static_cast< double >( totalTasks ) / sec; };

    std::printf( "[%-20s] SUMMARY over %zu runs:\n", label, seconds.size() );
    std::printf( "    min time=%8.4fs  (best throughput=%12.0f tasks/sec)\n", s.Min, tps( s.Min ) );
    std::printf( "    median time=%8.4fs  (throughput=%12.0f tasks/sec)\n", s.Median, tps( s.Median ) );
    std::printf( "    mean time=%8.4fs  stddev=%8.4fs  (throughput=%12.0f tasks/sec)\n",
                 s.Mean,
                 s.StdDev,
                 tps( s.Mean ) );
}

} // namespace

struct Job {
    Job( onyx::threading::ThreadPool* pool, int32_t repostCount, Promise< void >* waiter )
        : m_threadPool( pool )
        , m_repostCount( repostCount )
        , m_waiter( waiter ) {}

    Job( const Job& other )
        : m_threadPool( other.m_threadPool )
        , m_repostCount( other.m_repostCount )
        , m_waiter( other.m_waiter )
        , m_currentRepostCount( other.m_currentRepostCount ) {}

    void operator()() {
        if( ++m_currentRepostCount < m_repostCount ) {
            m_threadPool->post( Job( *this ) );
        } else {
            m_waiter->setValue();
        }
    }

  private:
    Heavy heavy;
    onyx::threading::ThreadPool* m_threadPool = nullptr;
    int32_t m_repostCount = 0;
    Promise< void >* m_waiter = nullptr;

    int32_t m_currentRepostCount = 0;
};

namespace {

// Runs one full ThreadPool benchmark iteration (fresh pool, fresh waiters)
// and returns elapsed seconds for the work itself, excluding construction.
double RunThreadPoolOnce() {
    using namespace onyx;
    using namespace onyx::threading;

    ThreadPool threadPool;
    Promise< void > waiters[ Concurrency * RepostCount ];

    auto start = std::chrono::steady_clock::now();

    for( auto& waiter : waiters ) {
        Job job( &threadPool, 0, &waiter );
        threadPool.post( job );
    }

    for( auto& waiter : waiters ) {
        waiter.getFuture().wait();
    }

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration< double >( end - start ).count();
}

} // namespace

TEST_CASE( "Benchmark generic tasks", "[threading][benchmark]" ) {
    std::vector< double > samples;
    samples.reserve( SampleCount );

    for( int32_t i = 0; i < SampleCount; ++i ) {
        samples.push_back( RunThreadPoolOnce() );
    }

    int64_t totalTasks = static_cast< int64_t >( Concurrency ) * RepostCount;
    PrintSamples( "ThreadPool", totalTasks, samples );
    PrintSummary( "ThreadPool", totalTasks, samples );

    REQUIRE( true );
}

// TEST_CASE( "Idle thread pool", "[threading][benchmark]" ) {
//     using namespace onyx;
//     using namespace onyx::threading;

//     ThreadPool threadPool;
//     std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

//     REQUIRE( true );
// }

struct FtlJob {
    ftl::TaskScheduler* Scheduler;
    int32_t RepostCount;
    ftl::WaitGroup* Waiter;
    int32_t CurrentRepostCount = 0;
    Heavy Heavy;
};

void FtlJobFunc( ftl::TaskScheduler* scheduler, void* argData ) {
    auto* job = static_cast< FtlJob* >( argData );

    if( ++job->CurrentRepostCount < job->RepostCount ) {
        job->Heavy = Heavy();
        ftl::Task task{ FtlJobFunc, job };
        scheduler->AddTask( task, ftl::TaskPriority::High, job->Waiter );
    }
    // else: chain finished. WaitGroup hits zero on its own -- no setValue().
}

namespace {

// Runs one full FTL benchmark iteration (fresh scheduler, fresh jobs) and
// returns elapsed seconds for the work itself, excluding Init().
double RunFtlOnce() {
    ftl::TaskScheduler scheduler;
    ftl::TaskSchedulerInitOptions options;
    scheduler.Init( options );

    ftl::WaitGroup wg( &scheduler );

    std::vector< FtlJob > jobs( Concurrency );
    std::vector< ftl::Task > tasks;
    tasks.reserve( Concurrency );

    for( int32_t i = 0; i < Concurrency; ++i ) {
        jobs[ i ] = FtlJob{ &scheduler, RepostCount, &wg };
        tasks.push_back( ftl::Task{ FtlJobFunc, &jobs[ i ] } );
    }

    auto start = std::chrono::steady_clock::now();

    scheduler.AddTasks( static_cast< uint32_t >( tasks.size() ), tasks.data(), ftl::TaskPriority::High, &wg );
    wg.Wait();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration< double >( end - start ).count();
}

} // namespace

TEST_CASE( "Benchmark generic tasks - FTL", "[threading][benchmark][ftl]" ) {
    std::vector< double > samples;
    samples.reserve( SampleCount );

    for( int32_t i = 0; i < SampleCount; ++i ) {
        samples.push_back( RunFtlOnce() );
    }

    int64_t totalTasks = static_cast< int64_t >( Concurrency ) * RepostCount;
    PrintSamples( "FiberTaskingLib", totalTasks, samples );
    PrintSummary( "FiberTaskingLib", totalTasks, samples );

    REQUIRE( true );
}
struct EnkiJob : enki::ITaskSet {
    enki::TaskScheduler* Scheduler;
    int32_t RepostCount;
    int32_t CurrentRepostCount = 0;
    Heavy Heavy;

    void ExecuteRange( enki::TaskSetPartition range_, uint32_t threadnum_ ) override {
        if( ++CurrentRepostCount < RepostCount ) {
            // Same in-place reuse pattern as FtlJob -- no per-repost
            // allocation of the job itself, just the Heavy churn.
            Heavy = onyx::threading::benchmark::Heavy();
            Scheduler->AddTaskSetToPipe( this );
        }
        // else: chain finished. enkiTS's WaitforAll() (used below) settles
        // once every outstanding task set -- including reposted ones -- is done.
    }
};

struct EnkiPinnedJob : enki::IPinnedTask {
    EnkiPinnedJob()
        : IPinnedTask( 0 ) {}

    enki::TaskScheduler* Scheduler = nullptr;
    EnkiPinnedJob* Next = nullptr;
    Heavy Heavy;

    void Execute() override {
        // Heavy = benchmark::Heavy();

        if( Next ) {
            Scheduler->AddPinnedTask( Next );
        }
    }
};

namespace {

// Runs one full enkiTS benchmark iteration (fresh scheduler, fresh jobs)
// and returns elapsed seconds for the work itself, excluding Initialize().
double RunEnkiOnce() {
    enki::TaskScheduler scheduler;
    scheduler.Initialize();

    constexpr int32_t TotalJobs = Concurrency * RepostCount;

    std::vector< EnkiPinnedJob > jobs( TotalJobs );

    // Build Concurrency independent chains.
    // Launch the first task of each chain.
    for( int c = 0; c < TotalJobs; ++c ) {
        jobs[ c ].Scheduler = &scheduler;
    }

    auto start = std::chrono::steady_clock::now();

    // Launch the first task of each chain.
    for( int c = 0; c < TotalJobs; ++c ) {
        scheduler.AddPinnedTask( &jobs[ c ] );
    }

    scheduler.WaitforAll();

    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration< double >( end - start ).count();
}

} // namespace

TEST_CASE( "Benchmark generic tasks - enkiTS", "[threading][benchmark][enkits]" ) {
    std::vector< double > samples;
    samples.reserve( SampleCount );

    for( int32_t i = 0; i < SampleCount; ++i ) {
        samples.push_back( RunEnkiOnce() );
    }

    int64_t totalTasks = static_cast< int64_t >( Concurrency ) * RepostCount;
    PrintSamples( "enkiTS", totalTasks, samples );
    PrintSummary( "enkiTS", totalTasks, samples );

    REQUIRE( true );
}
} // namespace onyx::threading::benchmark