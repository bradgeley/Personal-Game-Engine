// Bradley Christensen - 2022
#pragma once



//----------------------------------------------------------------------------------------------------------------------
// Singleton
//
// Singleton, implemented as a static pointer so you can delete and recreate the singleton at will.
//
template <typename T>
class Singleton
{
protected:

    Singleton() {}
    ~Singleton() {}

    static T* s_instance;
    
public:
    
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    static T& Get();

    static void Delete();
};



//----------------------------------------------------------------------------------------------------------------------
// Singleton instance storage
//
template<typename T>
T* Singleton<T>::s_instance = nullptr;



//----------------------------------------------------------------------------------------------------------------------
// Template function definitions
//
template<typename T>
T& Singleton<T>::Get()
{
    if (!s_instance)
    {
        s_instance = new T();
    }
    return *s_instance;
}



template<typename T>
void Singleton<T>::Delete()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}
