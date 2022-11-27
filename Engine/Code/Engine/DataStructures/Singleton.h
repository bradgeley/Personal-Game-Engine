// Bradley Christensen - 2022
#pragma once



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

    static T& Get()
    {
        if (!s_instance)
        {
            s_instance = new T();
        }
        return *s_instance;
    }

    static void Delete()
    {
        if (s_instance)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }
};

