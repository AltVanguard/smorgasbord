#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <memory>

using namespace std;

namespace Smorgasbord {

struct IScope
{
	virtual ~IScope() = default;
};

///struct IScoped
///{
///	virtual ~IScoped() = default;
///	
///	virtual unique_ptr<IScope> GetScope() = 0;
///};

template<typename T>
class UseScope : public IScope
{
private:
	T *target;
	
public:
	UseScope(T &_target)
		: target(&_target)
	{
		//this->target->Use();
	}
	
	~UseScope()
	{
		this->target->Unuse();
	}
	
	inline T* operator->()
	{
		return this->target;
	}
	
	inline T* operator*()
	{
		return this->target;
	}
};

template<class T>
class MapScope : public IScope
{
private:
	T *target;
	
public:
	MapScope(T &_target)
		: target(&_target)
	{
		//this->target->Map();
	}
	~MapScope()
	{
		this->target->Unmap();
	}
	
public:
	inline T* operator->()
	{
		return this->target;
	}
	
	inline T* operator*()
	{
		return this->target;
	}
};

template<class T>
class DisposeScope : public IScope
{
private:
	T *target;
	
public:
	DisposeScope(T &_target)
		: target(&_target)
	{
		
	}
	~DisposeScope()
	{
		this->target->Dispose();
	}
	
public:
	inline T* operator->()
	{
		return this->target;
	}
	
	inline T* operator*()
	{
		return this->target;
	}
};

}

#define SMORGASBORD_CONCATTOKENS(x, y) x##y
#define SMORGASBORD_CREATEIDENTIFIER(x, y) SMORGASBORD_CONCATTOKENS(x, y)
#define SMORGASBORD_CREATESCOPE(scopeType, targetType) unique_ptr< scopeType<targetType> >(new scopeType<targetType>(*this));
#define Scope(target, ...) std::unique_ptr<IScope> SMORGASBORD_CREATEIDENTIFIER(__scope_, __LINE__) = (target)->GetScope(__VA_ARGS__);
#define ScopeAlias(target, alias, ...) std::unique_ptr<IScope> SMORGASBORD_CREATEIDENTIFIER(__scope_, __LINE__) = (target)->GetScope(__VA_ARGS__); auto &alias = target;

#endif // SCOPE_HPP