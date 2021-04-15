#pragma once
#include <new.h>
#include <string>

#define Delegate_h_only

class Delegate
{
	class DelegateBase
	{
	public:
		virtual ~DelegateBase() {}
		virtual void Call() const = 0;
		virtual DelegateBase* Clone() const = 0;
	};

	class FreeFunctionDelegate : public DelegateBase
	{
		void(*function_)();
	public:
		FreeFunctionDelegate(void(function)()) : function_(function) {}
		void Call() const override { return function_(); }
		DelegateBase* Clone() const override { return new FreeFunctionDelegate(*this); }
	};

	template <typename A1>
	class FreeFunctionDelegate1A : public DelegateBase
	{
		void(*function_)(A1);
		A1 arg1_;
	public:
		FreeFunctionDelegate1A(void(function)(A1), A1 arg1) : function_(function), arg1_(arg1) {}
		void Call() const override { return function_(arg1_); }
		DelegateBase* Clone() const override { return new FreeFunctionDelegate1A(*this); }
	};

	template <class C>
	class MemberFunctionDelegate : public DelegateBase
	{
		void(C::*function_)();
		C* instance_;
	public:
		MemberFunctionDelegate(void(C::*function)(), C* instance) : function_(function), instance_(instance) {}
		void Call() const override { return (instance_->*function_)(); }
		DelegateBase* Clone() const override { return new MemberFunctionDelegate(*this); }
	};

	template <class C, typename A1>
	class MemberFunctionDelegate1A : public DelegateBase
	{
		void(C::*function_)(A1);
		C* instance_;
		A1 arg1_;
	public:
		MemberFunctionDelegate1A(void(C::*function)(A1), C* instance, A1 arg1) : function_(function), instance_(instance), arg1_(arg1) {}
		void Call() const override { return (instance_->*function_)(arg1_); }
		DelegateBase* Clone() const override { return new MemberFunctionDelegate1A(*this); }
	};

	template <class C>
	class ConstMemberFunctionDelegate : public DelegateBase
	{
		void(C::*function_)() const;
		const C* instance_;
	public:
		ConstMemberFunctionDelegate(void(C::*function)() const, const C* instance) : function_(function), instance_(instance) {}
		void Call() const override { return (instance_->*function_)(); }
		DelegateBase* Clone() const override { return new ConstMemberFunctionDelegate(*this); }
	};

	template <class C, typename A1>
	class ConstMemberFunctionDelegate1A : public DelegateBase
	{
		void(C::*function_)(A1) const;
		const C* instance_;
		const A1 arg1_;
	public:
		ConstMemberFunctionDelegate1A(void(C::*function)(A1) const, const C* instance, const A1 arg1) : function_(function), instance_(instance), arg1_(arg1) {}
		void Call() const override { return (instance_->*function_)(arg1_); }
		DelegateBase* Clone() const override { return new ConstMemberFunctionDelegate1A(*this); }
	};

public:
#ifndef Delegate_h_only
	Delegate(void);
	Delegate(void(*freeFunction)());
	template <class C>
	Delegate(void (C::*memberFunction)(), C* instance);
	template <class C>
	Delegate(void (C::*constMemberFunction)() const, const C* instance);
	Delegate(const Delegate& other);
	Delegate& operator= (const Delegate& other);

	~Delegate();

	void Bind(void(*freeFunction)());
	template <class C>
	void Bind(void (C::*memberFunction)(), C* instance);
	template <class C>
	void Bind(void (C::*constMemberFunction)() const, const C* instance);

	void Invoke() const;
#else
	Delegate(void) : delegate_(nullptr)
	{}

	Delegate(void(*freeFunction)())
	{
		Bind(freeFunction);
	}

	template <typename A1>
	Delegate(void(*freeFunction)(A1), A1 arg1)
	{
		Bind(freeFunction, arg1);
	}

	template <class C>
	Delegate(void (C::*memberFunction)(), C* instance)
	{
		Bind(memberFunction, instance);
	}

	template <class C, typename A1>
	Delegate(void (C::*memberFunction)(A1), C* instance, A1 arg1)
	{
		Bind(memberFunction, instance, arg1);
	}

	template <class C>
	Delegate(void (C::*constMemberFunction)() const, const C* instance)
	{
		Bind(constMemberFunction, instance);
	}

	template <class C, typename A1>
	Delegate(void (C::*constMemberFunction)(A1) const, const C* instance, const A1 arg1)
	{
		Bind(constMemberFunction, instance, arg1);
	}

	Delegate(const Delegate& other)
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

	Delegate& operator= (const Delegate& other)
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

	~Delegate()
	{
		//delete delegate_;
	}

	void Bind(void(*freeFunction)())
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

	template <typename A1>
	void Bind(void(*freeFunction)(A1), A1 arg1)
	{
		delete delegate_;

		if (freeFunction == nullptr)
		{
			delegate_ = nullptr;
		}
		else
		{
			delegate_ = new(data_) FreeFunctionDelegate1A<A1>(freeFunction, arg1);
		}
	}

	template <class C>
	void Bind(void (C::*memberFunction)(), C* instance)
	{
		delete delegate_;
		delegate_ = new(data_) MemberFunctionDelegate<C>(memberFunction, instance);
	}

	template <class C, typename A1>
	void Bind(void (C::*memberFunction)(A1), C* instance, A1 arg1)
	{
		delete delegate_;
		delegate_ = new(data_) MemberFunctionDelegate1A<C, A1>(memberFunction, instance, arg1);
	}

	template <class C>
	void Bind(void (C::*constMemberFunction)() const, const C* instance)
	{
		delete delegate_;
		delegate_ = new(data_) ConstMemberFunctionDelegate<C>(constMemberFunction, instance);
	}

	template <class C, typename A1>
	void Bind(void (C::*constMemberFunction)(A1) const, const C* instance, const A1 arg1)
	{
		delete delegate_;
		delegate_ = new(data_) ConstMemberFunctionDelegate1A<C, A1>(constMemberFunction, instance, arg1);
	}

	void Invoke() const
	{
		if (delegate_ == nullptr)
			return;

		return delegate_->Call();
	}
#endif

private:
	DelegateBase* delegate_ = nullptr;
	char data_[50];
};