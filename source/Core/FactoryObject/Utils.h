#ifndef UTILS_H
#define UTILS_H

// some special target platform features
#ifndef __ALG_PLATFORM

#define DEBUG

#ifdef _MALLOC_H
#include "malloc.h"
#endif

#ifndef _GLIBCXX_CHRONO
#include <chrono>
#endif

// STL
#include <float.h>
#include <bitset>
#include <limits.h>

// STL math and algorithms
#include <algorithm>
#include <random>
#include <cmath>
#include <iomanip>

// STL containers
#include "vector"
#include "list"
#include "map"
#include <set>

// STL input/output streams
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include <codecvt>

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <cassert>

#endif

// define custom types
#ifndef STRING_LIBS_INCLUDED
#define STRING_LIBS_INCLUDED defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD)
#endif

#ifndef __TO_DEGREES_CONVERSION_MULTIPLYER__
#define __TO_DEGREES_CONVERSION_MULTIPLYER__ 57.295779513082320876798154814105
#endif

#ifndef __TO_RADIANS_CONVERSION_MULTIPLYER__
#define __TO_RADIANS_CONVERSION_MULTIPLYER__ 0.01745329251994329576923690768489
#endif

#ifndef PI0
#define PI0 3.1415926535897932384626433832795
#endif

#ifndef PI2
#define PI2 6.283185307179586476925286766559
#endif

#ifndef PI_2
#define PI_2 1.5707963267948966192313216916398
#endif

#ifndef PI_4
#define PI_4 0.78539816339744830961566084581988
#endif

#ifndef STRINGIFY
#define STRINGIFY(INPUT) #INPUT
#endif

#ifndef CONCATENATE
#define CONCATENATE(X,Y) X##_##Y
#endif

/*! \defgroup <UTILITY> ( Utility functions and classes )
 *  \brief the module collaborates all the utility classes and functions
    @{
*/

template< typename __type >
struct Vector2D
{
    __type x = 0;
    __type y = 0;
};

namespace TUPLE_PACKAGE
{
    // Tuple
    template< typename ... T >
    struct Tuple;

    template< typename T1, typename ... T2 >
    struct Tuple<T1, T2 ...> : public Tuple<T2...>
    {
    protected:

        T1 m_Data;

        const Tuple<T2...>* m_Parent = static_cast< Tuple<T2...>* >( this );

        template< uint64_t Index, typename Head, typename ... Tail >
        friend struct TupleUnpacker;

    public:

        typedef T1 return_type;

        // constructors
        Tuple() : m_Data( T1() ){}

        Tuple( T1 _Data, T2 ... _Tail ) : Tuple<T2...>(_Tail...), m_Data( _Data ){}

        // virtual destructor
        virtual ~Tuple(){}
    };

    // Tuple
    template<>
    struct Tuple<>{};

    // Tuple unpacker
    template< uint64_t I, typename T1, typename ... T2 >
    struct TupleUnpacker
    {
        typedef typename TupleUnpacker<I-1, T2...>::return_type return_type;
        static return_type& get( Tuple< T1, T2... >& _Input )
        {
            return TupleUnpacker<I-1, T2...>::get(_Input);
        }
    };

    template< typename T1, typename ... T2 >
    struct TupleUnpacker< 0, T1, T2... >
    {
        typedef typename Tuple< T1, T2... >::return_type return_type;
        static return_type& get( Tuple< T1, T2... >& _Input )
        {
            return _Input.m_Data;
        }
    };

    template< uint64_t I, typename T1, typename ... T2 >
    typename TupleUnpacker<I, T1, T2...>::return_type& get( Tuple< T1, T2... >& _Input )
    {
        return TupleUnpacker< I, T1, T2... >::get( _Input );
    }
}

using namespace TUPLE_PACKAGE;

#if defined(_STDINT_H) || defined(_STDINT_H_) || defined(_STDINT)

namespace BIT_UTILS
{
    template<typename __type>
    void __set_bit__(__type& _M, uint_fast8_t _N )
    {
        if( _N < sizeof (_M) * 8UL )
        {
            _M |= 1UL << _N;
        }
    }

    template<typename __type>
    void __reset_bit__(__type& _M, uint_fast8_t _N )
    {
        if( _N < sizeof( _M ) * 8UL )
        {
            _M &= ~( 1UL << _N );
        }
    }

    template<typename __type>
    uint_fast32_t __get_bit__( const __type& _M, uint_fast8_t _N )
    {
        return _N < sizeof (_M) * 8UL ? (uint_fast32_t)(( _M >> _N ) & 1) : 0;
    }

    template<typename __type>
    __type __bit_reverse__(__type M, uint_fast8_t MSB)
    {
        __type result = 0;
        for ( uint_fast32_t i = 0 ; i < MSB ; ++i)
        {
            if ( M & ( 1 << i ) ) // look for the set bits
            {
                result |= 1 << ( MSB - 1 - i ); // set the bits at the end of the resulting bit mask
            }
        }

        return result;
    }

    template< typename __type >
    uint_fast32_t __get_msb__( const __type& _M )
    {
        uint_fast32_t MSB = 0;

        while ( ( 1 << MSB ) < _M )
        {
            MSB++;
        }

        return MSB;
    }

    template<typename T1, typename T2>
    T2 __retrieve_bits__( const T1& _Input, uint_fast8_t _S, uint_fast8_t _E )
    {
        // check
        if( _E > sizeof(T1) * 8 )
            return 0;

        // main code
        T2 output = 0;

        for( uint_fast32_t i = _S, j = 0 ; i < _E ; i++, j++ )
        {
            T1 bit = BIT_UTILS::__get_bit__<T1>( _Input, i );

            if( bit > 0 )
            {
                BIT_UTILS::__set_bit__( output, j );
            }
        }

        return output;
    }

    inline uint_fast64_t __lfsr64__ ( uint_fast64_t _INPUT )
    {
        static uint_fast64_t S = _INPUT;
        S = ((((S >> 63) ^ (S >> 62) ^ (S >> 61) ^ (S >> 59) ^ (S >> 57) ^ S ) & (uint64_t)1 ) << 63 ) | (S >> 1);
        return S;
    }

#if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD) || defined(_IOSFWD_)

    template< typename __type >
    std::string __to_hex_string__( const __type& _M, uint_fast8_t _N = 0 )
    {
        // initialize stream
        std::stringstream stream;
        stream << std::hex;

        // generate hex string
        uint_fast32_t N = _N < 4 ? sizeof(_M) * 8 : (uint_fast8_t)_N;

        for( uint_fast8_t i = 0 ; i < N ; i += 4 )
        {
            stream << BIT_UTILS::__retrieve_bits__< __type, uint_fast64_t >( _M, i, i + 4 );
        }

        return stream.str();
    }

#endif
}

using namespace BIT_UTILS;

#if defined(_GLIBCXX_BITSET) || defined(_LIBCPP_BITSET) || defined(_BITSET_)

namespace BITSET_UTILS
{
    template< std::size_t N >
    void __read_bits_from_uint__( std::bitset<N>& _Target, const uint_fast64_t& _Source, uint_fast8_t _Start, uint_fast8_t _End )
    {
        // check
        if( N < _Start || N < _End )
            return;

        // main code
        for( uint_fast8_t i = _Start, j = 0 ; i < _End ; i++, j++ )
        {
            _Target[i] = BIT_UTILS::__get_bit__( _Source, j );
        }
    }

    template< std::size_t N1, std::size_t N2 >
    std::bitset<N2> __extract_bits__( const std::bitset<N1>& _Source, uint_fast8_t _Start, uint_fast8_t _End )
    {
        // generate output
        std::bitset<N2> output;

        // check
        if( N1 < _Start || N1 < _End )
            return output;

        // fill output
        for( uint_fast64_t i = _Start, j = 0 ; i < _End ; i++, j++ )
        {
            output[j] = _Source[i];
        }

        return output;
    }

    template< std::size_t N >
    uint_fast8_t __get_msb__( std::bitset<N>& _Source )
    {
        uint_fast8_t MSB = 0;

        for( uint_fast8_t i = 0 ; i < N ; i++ )
        {
            if( _Source[i] > 0 )
            {
                MSB = i;
            }
        }

        return MSB;
    }

    template< std::size_t N >
    bool __big_unsigned_numbers_equal__( std::bitset<N> _A, std::bitset<N> _B )
    {
        return !( _A ^ _B ).any();
    }

    template< std::size_t N >
    bool __big_unsigned_number_is_greater__( std::bitset<N> _A, std::bitset<N> _B )
    {
        std::bitset<N> reference;
        reference[N-1] = 1;

        if( __big_unsigned_numbers_equal__( _A, _B ) )
            return false;

        while ( !( (_A ^ _B) & reference).any() )
        {
            _A <<= 1;
            _B <<= 1;
        }

        return (_A & reference).any();
    }

    template< std::size_t N >
    bool __big_unsigned_number_is_less__( std::bitset<N> _A, std::bitset<N> _B )
    {
        std::bitset<N> reference;
        reference[N-1] = 1;

        if( __big_unsigned_numbers_equal__( _A, _B ) )
            return false;

        while ( !( (_A ^ _B) & reference).any() )
        {
            _A <<= 1;
            _B <<= 1;
        }

        return !(_A & reference).any();
    }

    template< std::size_t N >
    struct BitSetsComparator
    {
        using is_transparent = std::bitset<N>;

    public:

        bool operator()( const std::bitset<N>& _A, const std::bitset<N>& _B ) const
        {
            return __big_unsigned_number_is_less__( _A, _B );
        }
    };

#if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD) || defined(_IOSFWD_)

    template< std::size_t N >
    std::string __write_to_hex_string__( const std::bitset<N>& _Set )
    {
        // initialize stream
        std::stringstream stream;
        stream << std::hex;

        // generate hex string
        for( uint_fast64_t i = 0 ; i < N ; i += 4 )
        {
            uint_fast64_t hexNumber = 0;

            for( uint_fast64_t j = 0 ; j < 4 ; j++ )
            {
                if( _Set[ i + j ] )
                {
                    BIT_UTILS::__set_bit__<uint_fast64_t>( hexNumber, j );
                }
            }

            stream << hexNumber;
        }

        return stream.str();
    }

#endif
}

using namespace BITSET_UTILS;

#endif

#endif

#if defined(_GLIBCXX_NUMERIC_LIMITS) || defined(_LIBCPP_LIMITS) || defined(_LIMITS_)

#undef max
#undef min

    // Fortran analogues functions
    template< typename __type > __type __digits__()  { return std::numeric_limits<__type>::digits; }

    template< typename __type > __type __epsilon__() { return std::numeric_limits<__type>::epsilon(); }

    template< typename __type > __type __huge__() { return std::numeric_limits<__type>::max(); };

    template< typename __type > __type __maxexponent__() { return std::numeric_limits<__type>::max_exponent; }

    template< typename __type > __type __minexponent__() { return std::numeric_limits<__type>::min_exponent; }

    template< typename __type > __type __radix__() { return std::numeric_limits<__type>::radix; }

    template< typename __type > __type __tiny__() { return std::numeric_limits<__type>::min(); }

    template< typename __type >
    class PseudoRandomNumberGenerator
    {
    private:

        std::mt19937_64 m_PseudoRandomNumberGenerator = std::mt19937_64( (uint_fast64_t)this );

    public:

        // constructors
        PseudoRandomNumberGenerator(){}

        // virtual destructor
        ~PseudoRandomNumberGenerator(){}

        // getters
        std::mt19937_64& get_pseudo_random_number_generator()
        {
            return m_PseudoRandomNumberGenerator;
        }

        // operators
        __type operator()(uint_fast64_t _Min = 0, uint_fast64_t _Max = __huge__<__type>() )
        {            
            long double integer  = (long double)( _Min + m_PseudoRandomNumberGenerator() % ( ( _Max + 1 ) - _Min ) );
            long double floating = m_PseudoRandomNumberGenerator() % 1024;
            while( floating > 1.0 )
                floating /= 1024;
            return (__type)( integer + floating );
        }

    };

#endif

template< typename Head >
inline  Head __max__( Head _A, Head _B )
{
    return _A > _B ? _A : _B;
}

template< typename Head, typename ... Args >
Head __max__( Head _A, Head _B, Args... _Args )
{
    return __max__( __max__( _A, _B ), _Args... );
}

template< typename __type > inline __type
__max__( __type* _Input , uint64_t _Size )
{
    __type vmax = _Input[0];

    for( uint64_t i = 1 ; i < _Size; i++ )
    {
        if( _Input[i] > vmax )
            vmax = _Input[i];
    }

    return vmax;
}

template< typename Head >
inline  Head __min__( Head _A, Head _B )
{
    return _A < _B ? _A : _B;
}

template< typename Head, typename ... Args >
Head __min__( Head _A, Head _B, Args... _Args )
{
    return __min__( __min__( _A, _B ), _Args... );
}

template< typename __type >
inline __type __min__( const __type* _Input, int _Size )
{
    int    imin = 0;
    __type vmin = _Input[imin];

    for( int i = 1 ; i < _Size; i++ )
    {
        if( _Input[i] < vmin )
        {
            vmin = _Input[i];
            imin = i;
        }
    }

    return vmin;
}

template< typename __type >
inline  __type __abs__ ( __type _A )
{
    return ( _A < 0 ) ? -_A : _A;
}

template< typename __type >
inline  __type __sign__( __type _A, __type _B )
{
    return _A * ( ( _B > 0 ) ? (__type)1 : -(__type)1 );
}

template< typename __type >
inline  __type __sign__( __type _A )
{
    return _A >= 0 ? (__type)1.0 : -(__type)1.0;
}

template< typename __type >
inline  void __swap__( __type& _A, __type& _B )
{
    __type c = _B;
    _B = _A;
    _A = c;
}

inline double __to_degrees__(double _Angle)
{
    return _Angle * __TO_DEGREES_CONVERSION_MULTIPLYER__;
}

inline double __to_radians__(double _Angle)
{
    return _Angle * __TO_RADIANS_CONVERSION_MULTIPLYER__;
}

template<typename __type> __type
__saturation__(__type _Input, __type _UpperLimit, __type _LowerLimit)
{
    if( _Input > _UpperLimit )
        return _UpperLimit;

    if( _Input < _LowerLimit )
        return _LowerLimit;

    return _Input;
}

inline std::wstring __to_wstring__(const std::string _Value)
{
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(_Value);
}

inline FILE* __open_file__(std::string _Path, std::string _Mode)
{
    FILE* file = std::fopen(_Path.c_str(), _Mode.c_str() );

#if defined(_WIN32) || defined(WIN32) // try to do something on Windows
    if(file == nullptr)
        file = _wfopen(&__to_wstring__(_Path)[0], &__to_wstring__(_Mode)[0] );
#else
#endif

    return file;
}

template< typename __type > inline __type*
__alloc__(int _Size)
{
    if(_Size <= 0)
        return nullptr;

    return (__type*)calloc( _Size, sizeof(__type) );
}

template< typename __type > inline __type*
__alloc__(int _Size, __type _Value)
{
    if( _Size <= 0 )
        return nullptr;

    __type *memory = (__type*)calloc( _Size, sizeof(__type) );

    for(int i = 0 ; i < _Size; i++ )
        memory[i] = _Value;

    return memory;
}

template< typename __type > inline __type*
__alloc__( int _Size, void (*__initializer__)( __type* _Memory, int _Size ) )
{
    if(_Size <= 0)
        return nullptr;

    __type *memory = (__type*)calloc( _Size, sizeof(__type) );

    if(__initializer__ && memory)
        __initializer__(memory, _Size);

    return memory;
}

template< typename __type > inline __type*
__realloc__( __type* _Memory, int _Newsize )
{
    if(_Newsize <= 0 && _Memory)
    {
        free(_Memory);
        return nullptr;
    }

    __type *data = (__type*)realloc(_Memory, sizeof (__type)*_Newsize );

    if(!data && _Memory)
        free(_Memory);

    return data;
}

template< typename __type > inline __type*
__mfree__( __type* _Memory )
{
    if(_Memory)
    {
        free(_Memory);
        _Memory = nullptr;
    }

    return _Memory;
}

inline std::chrono::high_resolution_clock::time_point tic()
{
    return std::chrono::high_resolution_clock::now();
}

template< typename __type = std::chrono::nanoseconds >
double elapsed( std::chrono::high_resolution_clock::time_point _Then, std::chrono::high_resolution_clock::time_point _Now )
{
    return (double)std::chrono::duration_cast<__type>(_Now - _Then).count();
}

namespace MULTI_THREADING {

class would_block
    : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};


class ThreadPool {
public:
    template <typename F, typename... Args>
    using return_type =
#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703L
        typename std::invoke_result<F&&, Args&&...>::type;
#else
        typename std::result_of<F&& (Args&&...)>::type;
#endif

    explicit ThreadPool(std::size_t threads
                        = (std::max)(2u, std::thread::hardware_concurrency()));
    template <typename F, typename... Args>
    auto enqueue_block(F&& f, Args&&... args) -> std::future<return_type<F, Args...>>;
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<return_type<F, Args...>>;
    void wait_until_empty();
    void wait_until_nothing_in_flight();
    void set_queue_size_limit(std::size_t limit);
    void set_pool_size(std::size_t limit);
    ~ThreadPool();

private:
    void start_worker(std::size_t worker_number,
                      std::unique_lock<std::mutex> const &lock);

    template <typename F, typename... Args>
    auto enqueue_worker(bool, F&& f, Args&&... args) -> std::future<return_type<F, Args...>>;

    template <typename T>
    static std::future<T> make_exception_future (std::exception_ptr ex_ptr);

    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // target pool size
    std::size_t pool_size;
    // the task queue
    std::queue< std::function<void()> > tasks;
    // queue length limit
    std::size_t max_queue_size = 100000;
    // stop signal
    bool stop = false;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition_producers;
    std::condition_variable condition_consumers;

    std::mutex in_flight_mutex;
    std::condition_variable in_flight_condition;
    std::atomic<std::size_t> in_flight;

    struct handle_in_flight_decrement
    {
        ThreadPool & tp;

        handle_in_flight_decrement(ThreadPool & tp_)
            : tp(tp_)
        { }

        ~handle_in_flight_decrement()
        {
            std::size_t prev
                = std::atomic_fetch_sub_explicit(&tp.in_flight,
                                                 std::size_t(1),
                                                 std::memory_order_acq_rel);
            if (prev == 1)
            {
                std::unique_lock<std::mutex> guard(tp.in_flight_mutex);
                tp.in_flight_condition.notify_all();
            }
        }
    };
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(std::size_t threads)
    : pool_size(threads)
    , in_flight(0)
{
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    for (std::size_t i = 0; i != threads; ++i)
        start_worker(i, lock);
}

// add new work item to the pool and block if the queue is full
template<class F, class... Args>
auto ThreadPool::enqueue_block(F&& f, Args&&... args) -> std::future<return_type<F, Args...>>
{
    return enqueue_worker (true, std::forward<F> (f), std::forward<Args> (args)...);
}

// add new work item to the pool and return future with would_block exception if it is full
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<return_type<F, Args...>>
{
    return enqueue_worker (false, std::forward<F> (f), std::forward<Args> (args)...);
}

template <typename F, typename... Args>
auto ThreadPool::enqueue_worker(bool block, F&& f, Args&&... args) -> std::future<return_type<F, Args...>>
{
    auto task = std::make_shared< std::packaged_task<return_type<F, Args...>()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type<F, Args...>> res = task->get_future();

    std::unique_lock<std::mutex> lock(queue_mutex);

    if (tasks.size () >= max_queue_size)
    {
        if (block)
        {
            // wait for the queue to empty or be stopped
            condition_producers.wait(lock,
                                     [this]
                                     {
                                         return tasks.size () < max_queue_size
                                                || stop;
                                     });
        }
        else
        {
            return ThreadPool::make_exception_future<return_type<F, Args...>> (
                std::make_exception_ptr (would_block("queue full")));
        }
    }


    // don't allow enqueueing after stopping the pool
    if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks.emplace([task](){ (*task)(); });
    std::atomic_fetch_add_explicit(&in_flight,
                                   std::size_t(1),
                                   std::memory_order_relaxed);
    condition_consumers.notify_one();

    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
    pool_size = 0;
    condition_consumers.notify_all();
    condition_producers.notify_all();
    condition_consumers.wait(lock, [this]{ return this->workers.empty(); });
    assert(in_flight == 0);
}

inline void ThreadPool::wait_until_empty()
{
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    this->condition_producers.wait(lock,
                                   [this]{ return this->tasks.empty(); });
}

inline void ThreadPool::wait_until_nothing_in_flight()
{
    std::unique_lock<std::mutex> lock(this->in_flight_mutex);
    this->in_flight_condition.wait(lock,
                                   [this]{ return this->in_flight == 0; });
}

inline void ThreadPool::set_queue_size_limit(std::size_t limit)
{
    std::unique_lock<std::mutex> lock(this->queue_mutex);

    if (stop)
        return;

    std::size_t const old_limit = max_queue_size;
    max_queue_size = (std::max)(limit, std::size_t(1));
    if (old_limit < max_queue_size)
        condition_producers.notify_all();
}

inline void ThreadPool::set_pool_size(std::size_t limit)
{
    if (limit < 1)
        limit = 1;

    std::unique_lock<std::mutex> lock(this->queue_mutex);

    if (stop)
        return;

    std::size_t const old_size = pool_size;
    assert(this->workers.size() >= old_size);

    pool_size = limit;
    if (pool_size > old_size)
    {
        // create new worker threads
        // it is possible that some of these are still running because
        // they have not stopped yet after a pool size reduction, such
        // workers will just keep running
        for (std::size_t i = old_size; i != pool_size; ++i)
            start_worker(i, lock);
    }
    else if (pool_size < old_size)
        // notify all worker threads to start downsizing
        this->condition_consumers.notify_all();
}

inline void ThreadPool::start_worker(
    std::size_t worker_number, std::unique_lock<std::mutex> const &lock)
{
    assert(lock.owns_lock() && lock.mutex() == &this->queue_mutex);
    assert(worker_number <= this->workers.size());

    auto worker_func =
        [this, worker_number]
    {
        for(;;)
        {
            std::function<void()> task;
            bool notify;

            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition_consumers.wait(lock,
                                               [this, worker_number]{
                            return this->stop || !this->tasks.empty()
                                || pool_size < worker_number + 1; });

                // deal with downsizing of thread pool or shutdown
                if ((this->stop && this->tasks.empty())
                    || (!this->stop && pool_size < worker_number + 1))
                {
                    // detach this worker, effectively marking it stopped
                    this->workers[worker_number].detach();
                    // downsize the workers vector as much as possible
                    while (this->workers.size() > pool_size
                           && !this->workers.back().joinable())
                        this->workers.pop_back();
                    // if this is was last worker, notify the destructor
                    if (this->workers.empty())
                        this->condition_consumers.notify_all();
                    return;
                }
                else if (!this->tasks.empty())
                {
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                    notify = this->tasks.size() + 1 ==  max_queue_size
                             || this->tasks.empty();
                }
                else
                    continue;
            }

            handle_in_flight_decrement guard(*this);

            if (notify)
            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                condition_producers.notify_all();
            }

            task();
        }
    };

    if (worker_number < this->workers.size()) {
        std::thread & worker = this->workers[worker_number];
        // start only if not already running
        if (!worker.joinable()) {
            worker = std::thread(worker_func);
        }
    } else
        this->workers.push_back(std::thread(worker_func));
}

template <typename T>
inline std::future<T> ThreadPool::make_exception_future (std::exception_ptr ex_ptr)
{
    std::promise<T> p;
    p.set_exception (ex_ptr);
    return p.get_future ();
}

} // namespace progschj


#if defined(_STRINGFWD_H) || defined(_LIBCPP_IOSFWD) || defined(_IOSFWD_)

#define DEBUGGER

    namespace STRING_EXTENSION
    {
        // utility functions
        inline std::string __remove_prefix__( std::string _Input, std::string _Prefix )
        {
            size_t j = 0;
            for( size_t i = 0 ; i < _Prefix.size() ; i++ )
            {
                if( _Input[i] != _Prefix[i] )
                {
                    break;
                }

                j++;
            }

            std::string output;

            for( size_t i = j ; i < _Input.size() ; i++ )
            {
                output += _Input[i];
            }

            return output;
        }

        inline std::string __format_camel_style_string__( std::string _Input )
        {
            std::string output;
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                output += _Input[i];

                if( i + 1 < _Input.size() && std::isupper( _Input[i+1] ) )
                    output += ' ';
            }

            return output;
        }

        inline std::string __class_field_to_string__( std::string _Input )
        {
            return __format_camel_style_string__( __remove_prefix__( _Input, "m_" ) );
        }

        inline std::vector< std::string > __split__( std::string _Input, std::string _Delimeter = " " )
        {
            if( _Input.empty() )
                return std::vector< std::string >();

            int start  = 0;
            int end    = 0;
            int size   = _Delimeter.size();
            std::vector< std::string > output;

            while ( end >= 0 )
            {
                end = _Input.find(_Delimeter, start);
                output.push_back( _Input.substr(start, end-start) );
                start = end + size;
            }

            return output;
        }

        inline bool __string_contains__( std::string _Input, char _Delimeter = ' ')
        {
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                if( _Input[i] == _Delimeter )
                    return true;
            }

            return false;
        }

        inline std::string __to_upper__( std::string _String )
        {
            std::transform(_String.begin(), _String.end(), _String.begin(), ::toupper);
            return _String;
        }

        inline std::string __to_lower__( std::string _String )
        {
            std::transform(_String.begin(), _String.end(), _String.begin(), ::tolower);
            return _String;
        }

        inline std::string __remove_symbol__( std::string _String, char _Symbol )
        {
            std::string output;

            // compute output string size
            size_t size = 0;
            for( size_t i = 0 ; i < _String.size() ; i++ )
            {
                if( _String[i] != _Symbol )
                    size++;
            }

            // edit output string
            output.resize(size);
            for( size_t i = 0, j = 0 ; i < _String.size() ; i++ )
            {
                if( _String[i] != _Symbol )
                    output[j++] = _String[i];
            }

            return output;
        }

        inline std::string __remove_symbols__( std::string _String, std::set< char > _Symbols )
        {
            std::string output;

            // compute output string size
            size_t size = 0;
            for( size_t i = 0 ; i < _String.size() ; i++ )
            {
                if( _Symbols.find( _String[i] ) == _Symbols.end() )
                    size++;
            }

            // edit output string
            output.resize(size);
            for( size_t i = 0, j = 0 ; i < _String.size() ; i++ )
            {
                if( _Symbols.find( _String[i] ) == _Symbols.end() )
                    output[j++] = _String[i];
            }

            return output;
        }

        inline std::string __replace_all__( std::string _String, std::string _Substring, std::string _NewSubstring )
        {
            // reserve buffer for substring
            std::string buffer;
            buffer.reserve( _Substring.size() );

            // generate output string
            std::string output;
            output.reserve( _String.size() );

            // main code
            for( size_t i = 0 ; i < _String.size() ;  )
            {
                for( size_t j = 0, k = i ; j < _Substring.size() ; j++, k++ )
                {
                    buffer.push_back( _String[k] );
                }

                if( buffer == _Substring )
                {
                    output.append(_NewSubstring);
                    i += _Substring.size();
                }
                else
                {
                    output.push_back( _String[i] );
                    i++;
                }

                buffer.clear();
            }

            return output;
        }

        inline void __replace_symbol__( std::string& _Input, char _Symbol )
        {
            for( size_t i = 0 ; i < _Input.size() ; i++ )
            {
                if( _Input[i] == _Symbol )
                {
                    _Input[i] =  _Symbol;
                }
            }
        }

        // from string conversion
        template< typename __type >
        __type __from_string__( std::string _Input );

        template<> inline float __from_string__< float >( std::string _Input )
        {
            try
            {
                return std::stof( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline double __from_string__<double>( std::string _Input )
        {
            try
            {
                return std::stod( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline long double __from_string__<long double>( std::string _Input )
        {
            try
            {
                return std::stold( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline short __from_string__<short>( std::string _Input )
        {
            try
            {
                return std::stoi( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline int __from_string__<int>( std::string _Input )
        {
            try
            {
                return std::stoi( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline long __from_string__<long>( std::string _Input )
        {
            try
            {
                return std::stol( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline long long __from_string__<long long>( std::string _Input )
        {
            try
            {
                return std::stoll( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline unsigned short __from_string__<unsigned short>( std::string _Input )
        {
            try
            {
                return std::stoul( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline unsigned int __from_string__<unsigned int>( std::string _Input )
        {
            try
            {
                return std::stoul( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline unsigned long __from_string__<unsigned long>( std::string _Input )
        {
            try
            {
                return std::stoul( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline unsigned long long __from_string__<unsigned long long>( std::string _Input )
        {
            try
            {
                return std::stoull( _Input );
            }
            catch(...)
            {
                return 0.0;
            }
        }

        template<> inline bool __from_string__<bool>( std::string _Input )
        {
            try
            {
                return _Input == "true" || std::stoi( _Input ) ? true : false;
            }
            catch(...)
            {
                return false;
            }
        }

        template<> inline std::string __from_string__< std::string >( std::string _Input )
        {
            return _Input;
        }

        template<> inline char __from_string__<char>( std::string _Input )
        {
            return _Input.empty() ? ' ' : _Input[0];
        }

        // to string conversion
        template< typename __type >
        std::string __to_string__( __type _Input );

        template<> inline std::string __to_string__<bool>( bool _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__< float >( float _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<double>( double _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<long double>( long double _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<short>( short _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<int>( int _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<long>( long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<long long>( long long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<unsigned short>( unsigned short _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<unsigned int>( unsigned int _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<unsigned long>( unsigned long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__<unsigned long long>( unsigned long long _Input )
        {
            return std::to_string( _Input );
        }

        template<> inline std::string __to_string__< std::string >( std::string _Input )
        {
            return _Input;
        }
    }

    class Debugger
    {
    protected:

        static std::string GetCurrentTime()
        {
            time_t _time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
            std::string output = ctime(&_time);
            return output.substr(0, output.length()-1);
        }

    public:

        static std::string MessageFormat( std::string _Typename, std::string _OjectName, std::string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + "[ " + _Typename + " ] " + "[ " + _OjectName + " ] " + _Message;
        }

        static std::string MessageFormat( std::string _OjectName, std::string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + "[ " + _OjectName + " ] " + _Message;
        }

        static std::string MessageFormat( std::string _Message)
        {
            return "[ " + GetCurrentTime() + " ] " + _Message;
        }

        // logging functions
        static void Log( std::string _Typename, std::string _OjectName, std::string _Message )
        {
            std::cout << MessageFormat(_Typename, _OjectName,  _Message) + "\n";
        }

        static void LogError( std::string _Typename, std::string _OjectName, std::string _Message )
        {
            std::cerr << MessageFormat(_Typename, _OjectName,  _Message) + "\n";
        }
    };

#endif

#undef __TO_DEGREES_CONVERSION_MULTIPLYER__
#undef __TO_RADIANS_CONVERSION_MULTIPLYER__

/*! @} */

#endif // UTILS_H
