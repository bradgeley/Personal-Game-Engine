// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/EngineCommon.h"
#include "Engine/Core/Name.h"
#include <unordered_map>
#include <typeinfo>



//----------------------------------------------------------------------------------------------------------------------
struct PropertyBase
{
    virtual ~PropertyBase() = default;
    virtual PropertyBase* Clone() const = 0;
    virtual bool IsEqual(PropertyBase const* other) const = 0;
    virtual size_t GetType() const = 0;
};



//----------------------------------------------------------------------------------------------------------------------
template<typename T>
struct TypedPropertyBase : public PropertyBase
{
    virtual PropertyBase* Clone() const override;
    virtual bool IsEqual(PropertyBase const* other) const override;
    virtual size_t GetType() const override;
    void Set(T const& value);

    static size_t StaticGetType();
    static TypedPropertyBase<T> const* GetAsThisType(PropertyBase const* prop);
    static TypedPropertyBase<T>* GetAsThisType(PropertyBase* prop);

    T m_property = {};
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
PropertyBase* TypedPropertyBase<T>::Clone() const
{
    TypedPropertyBase<T>* clone = new TypedPropertyBase<T>();
    clone->m_property = m_property;
    return clone;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool TypedPropertyBase<T>::IsEqual(PropertyBase const* other) const
{
    if (auto const& asThisType = TypedPropertyBase<T>::GetAsThisType(other))
    {
        return asThisType->m_property == m_property;
    }
    return false;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
size_t TypedPropertyBase<T>::GetType() const
{
    return StaticGetType();
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void TypedPropertyBase<T>::Set(T const& value)
{
    m_property = value;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
size_t TypedPropertyBase<T>::StaticGetType()
{
    #ifndef RTTI_ENABLED
        // If RTTI is disabled, we can still get the type of the property base, it just limits the use of this class
        // to not be used across DLL's. If Set is called from engine code on a NP instance, then Get is called on that
        // same instance in game code, it will fail to match the types. That is pretty much unacceptable unfortunately.
        static const size_t typeId = reinterpret_cast<size_t>(&typeId);
        return typeId;
    #else
        return typeid(T).hash_code();
    #endif
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
TypedPropertyBase<T> const* TypedPropertyBase<T>::GetAsThisType(PropertyBase const* prop)
{
    if (prop->GetType() == StaticGetType())
    {
        return reinterpret_cast<TypedPropertyBase<T> const*>(prop);
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
TypedPropertyBase<T>* TypedPropertyBase<T>::GetAsThisType(PropertyBase* prop)
{
    if (prop->GetType() == StaticGetType())
    {
        return reinterpret_cast<TypedPropertyBase<T>*>(prop);
    }
    return nullptr;
}



//----------------------------------------------------------------------------------------------------------------------
// Named Properties
//
// Generic Property Container
//
struct NamedProperties
{
    ~NamedProperties();
    
    template<typename T>
    void Set(Name name, T const& value);
    
    template<typename T>
    T Get(Name name, T const& defaultValue) const;

    void Clear();
    int Size() const;
    bool Contains(Name name) const;

private:

    std::unordered_map<Name, PropertyBase*> m_properties;
};



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void NamedProperties::Set(Name name, T const& value)
{
    auto maybeExistingProperty = m_properties.find(name);
    if (maybeExistingProperty != m_properties.end())
    {
        PropertyBase* existingProperty = maybeExistingProperty->second;
        if (TypedPropertyBase<T>* existingTypedProperty = TypedPropertyBase<T>::GetAsThisType(existingProperty))
        {
            // overwrite the existing value
            existingTypedProperty->Set(value);
            return;
        }
        // delete the existing value
        delete existingProperty;
        m_properties.erase(maybeExistingProperty);
    }
    // Add the new entry
    TypedPropertyBase<T>* newProperty = new TypedPropertyBase<T>();
    newProperty->Set(value);
    m_properties.emplace(name, newProperty);
}



//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T NamedProperties::Get(Name name, T const& defaultValue) const
{
    auto maybeExistingProperty = m_properties.find(name);
    if (maybeExistingProperty != m_properties.end())
    {
        PropertyBase* existingProperty = maybeExistingProperty->second;
        if (auto existingTypedProperty = TypedPropertyBase<T>::GetAsThisType(existingProperty))
        {
            return existingTypedProperty->m_property;
        }
    }
    return defaultValue;
}

