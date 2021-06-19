#ifndef SMORGASBORD_SCENEGRAPH_HPP
#define SMORGASBORD_SCENEGRAPH_HPP

#include "transform.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#define PropertyGetter(Type) std::function<Type ()>
#define PropertySetter(Type) std::function<void (const Type &)>

#define SMORGASBORD_PROPERTY_FIELD(type, name) \
	type name = enumerator.AddField<type>(&name, #type, #name)

#define SMORGASBORD_PROPERTY_GET(type, name, getter) \
	Smorgasbord::PropertyGet<type> name = enumerator.Add<type>( \
		&name, \
		#type, \
		#name, \
		true, \
		true, \
		Property<bool>([this](void) -> bool { return this->getter();}))

#define SMORGASBORD_PROPERTY_SET(type, name, setter) \
	Smorgasbord::PropertySet<type> name = enumerator.Add<type>( \
		&name, \
		#type, \
		#name, \
		true, \
		true, \
		PropertySet<bool>([this](const bool &b){ this->setter(b); }))

#define SMORGASBORD_PROPERTY_GETSET(type, name, getter, setter) \
	Smorgasbord::Property<type> name = enumerator.Add<type>( \
		&name, \
		#type, \
		#name, \
		true, \
		true, \
		Property<type>( \
			[this](void) -> type { return this->getter();}, \
			[this](const type &b){ this->setter(b); }))

namespace Smorgasbord {

template<typename T>
class Property
{
protected:
	PropertyGetter(T) getter = nullptr;
	PropertySetter(T) setter = nullptr;
	
public:
	Property(
		PropertyGetter(T) _getter,
		PropertySetter(T) _setter)
		: getter(_getter), setter(_setter)
	{ }
	
	Property(Property<T> &b)
		: getter(b.getter), setter(b.setter)
	{ }
	
	Property(Property<T> &&b)
		: getter(b.getter), setter(b.setter)
	{ }
	
	operator T()
	{
		return getter();
	}
	
	Property<T> &operator =(const T &b)
	{
		setter(b);
		return *this;
	}
	
	void operator =(Property<T> &b)
	{
		getter = b.getter;
		setter = b.setter;
	}
};

template<typename T>
class PropertyGet : public Property<T>
{
public:
	PropertyGet(
		PropertyGetter(T) _getter)
		: Property<T>(_getter, PropertySetter(T)())
	{ }
	
	PropertyGet(Property<T> &b)
		: Property<T>(b)
	{ }
	
	PropertyGet(Property<T> &&b)
		: Property<T>(b)
	{ }
	
	Property<T> &operator =(const T &b) = delete;
};

template<typename T>
class PropertySet : public Property<T>
{
public:
	PropertySet(
		PropertySetter(T) _setter)
		: Property<T>(PropertyGetter(T)(), _setter)
	{ }
	
	PropertySet(Property<T> &b)
		: Property<T>(b)
	{ }
	
	PropertySet(Property<T> &&b)
		: Property<T>(b)
	{ }
	
	operator T() = delete;
};

struct SceneObjectPropertyField
{
	std::string type;
	std::string name;
	uint32_t size = 0; // in bytes
	void *p = nullptr;
	
	SceneObjectPropertyField(
		const char *_type, const char *_name, uint32_t _size, void *_p)
		: type(_type), name(_name), size(_size), p(_p)
	{ }
};

struct SceneObjectProperty
{
	std::string type;
	std::string name;
	bool hasGetter = false;
	bool hasSetter = false;
	uint32_t size = 0; // in bytes
	void *p = nullptr;
	
	SceneObjectProperty(
		const char *_type,
		const char *_name,
		bool _hasGetter,
		bool _hasSetter,
		uint32_t _size,
		void *_p)
		: type(_type)
		, name(_name)
		, hasGetter(_hasGetter)
		, hasSetter(_hasSetter)
		, size(_size)
		, p(_p)
	{ }
};

class SceneObjectPropertyEnumerator
{
private:
	std::unordered_map<std::string, SceneObjectPropertyField> fields;
	std::unordered_map<std::string, SceneObjectProperty> properties;
	
public:
	template <typename T>
	T& AddField(T *p, const char *type, const char *name)
	{
		fields.emplace(
			name,
			SceneObjectPropertyField(type, name, (uint32_t)sizeof(T), p));
		*p = { };
		return *p;
	}
	
	template <typename T>
	Property<T> Add(
		Property<T> *p,
		const char *type,
		const char *name,
		bool hasGetter,
		bool hasSetter,
		Property<T> property)
	{
		properties.emplace(
			name,
			SceneObjectProperty(
				type, name, hasGetter, hasSetter, (uint32_t)sizeof(T), p));
		property = T();
		return property;
	}
	
	const  std::unordered_map<std::string, SceneObjectPropertyField> &GetFields() const
	{
		return fields;
	}
	
	const std::unordered_map<std::string, SceneObjectProperty> &GetProperties() const
	{
		return properties;
	}
};

class SceneObject
{
protected:
	SceneObjectPropertyEnumerator enumerator;
	
public:
	virtual ~SceneObject() { };
	
	virtual std::string GetType() = 0;
	virtual std::string GetName() = 0;
};

class ScenePlacedObject : public SceneObject
{
public:
	SMORGASBORD_PROPERTY_FIELD(glm::vec3, position);
	SMORGASBORD_PROPERTY_FIELD(glm::quat, rotation);
	SMORGASBORD_PROPERTY_FIELD(glm::vec3, scale);
};

}

#endif // SMORGASBORD_SCENEGRAPH_HPP
