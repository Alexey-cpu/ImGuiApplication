#ifndef SINGLETON_H
#define SINGLETON_H

#include <mutex>

template< typename __type >
class Singleton
{
public:

    static __type* Instance()
    {
        const std::lock_guard<std::mutex> lock(m_Mutex);

        if(m_Instance == nullptr)
        {
            m_Instance = new __type;
            m_Destroyer.initialize(m_Instance);
        }

        return m_Instance;
    }

private:

    class SingletonInstanceDestroyer
    {
        __type* m_Instance = nullptr;
    public:

        // constructors
        SingletonInstanceDestroyer(){}

        // destructor
        ~SingletonInstanceDestroyer()
        {
            if( m_Instance != nullptr )
                delete m_Instance;
        }

        // public methods
        void initialize(__type* _Instance)
        {
            m_Instance = _Instance;
        }
    };

    // constructors
    Singleton();
    Singleton(Singleton<__type> const&);
    Singleton<__type>& operator= (const Singleton<__type>& t);

    // virtual destructors
    virtual ~Singleton();

    // info
    static __type*    m_Instance;
    static std::mutex m_Mutex;
    static SingletonInstanceDestroyer m_Destroyer;
};

// static fields initialization
template< typename __type >
__type* Singleton<__type>::m_Instance = nullptr;

template< typename __type >
std::mutex Singleton<__type>::m_Mutex;

template< typename __type>
typename Singleton<__type>::SingletonInstanceDestroyer Singleton<__type>::m_Destroyer;

#endif // SINGLETON_H
