#include "Delegate.h"
#ifndef Delegate_h_only

Delegate::Delegate(void) : delegate_(nullptr)
{}

Delegate::Delegate(void(*freeFunction)())
{
	Bind(freeFunction);
}

template <class C>
Delegate::Delegate(void (C::*memberFunction)(), C* instance)
{
	Bind(memberFunction, instance);
}

template <class C>
Delegate::Delegate(void (C::*constMemberFunction)() const, const C* instance)
{
	Bind(constMemberFunction, instance);
}

Delegate::Delegate(const Delegate& other)
{
	if (other.delegate_ == nullptr)
	{
		delegate_ = nullptr;
	}
	else
	{
		delegate_ = other.delegate_->Clone();
	}
}

Delegate& Delegate::operator= (const Delegate& other)
{
	if (this != &other)
	{
		delete delegate_;
		if (other.delegate_ == nullptr)
		{
			delegate_ = nullptr;
		}
		else
		{
			delegate_ = other.delegate_->Clone();
		}
	}
	return *this;
}

Delegate::~Delegate()
{
	delete delegate_;
}

void Delegate::Bind(void(*freeFunction)())
{
	delete delegate_;

	if (freeFunction == nullptr)
	{
		delegate_ = nullptr;
	}
	else
	{
		delegate_ = new(data_) FreeFunctionDelegate(freeFunction);
	}
}

template <class C>
void Delegate::Bind(void (C::*memberFunction)(), C* instance)
{
	delete delegate_;
	delegate_ = new(data_) MemberFunctionDelegate<C>(memberFunction, instance);
}

template <class C>
void Delegate::Bind(void (C::*constMemberFunction)() const, const C* instance)
{
	delete delegate_;
	delegate_ = new(data_) ConstMemberFunctionDelegate<C>(constMemberFunction, instance);
}

void Delegate::Invoke() const
{
	if (delegate_ == nullptr)
		return;

	return delegate_->Call();
}
#endif
