#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <typename T>
	struct PtrImp {
		T val;
		size_t nrefs;

		template <typename... ArgsT>
		PtrImp(size_t nrefs, ArgsT &&... args);
	};

	template <typename T>
	template <typename... ArgsT>
	PtrImp<T>::PtrImp(size_t nrefs, ArgsT &&... args):
		val(forward<ArgsT>(args)...), nrefs(nrefs) { }
	
	template <typename T, typename ImpT=T>
	class Ptr {
	public:
		Ptr();

		explicit Ptr(PtrImp<ImpT> &imp);

		Ptr(const Ptr<T, ImpT> &src);

		template <typename U>
		Ptr(const Ptr<U, ImpT> &src);

		template <typename... ArgsT>
		Ptr(size_t nrefs, ArgsT &&... args);

		~Ptr();
		
		const Ptr<T, ImpT> &operator =(const Ptr<T, ImpT> &src);

		template <typename U>
		const Ptr<T, ImpT> &operator =(const Ptr<U, ImpT> &src);

		const T &operator *() const;
		T &operator *();

		const T *operator ->() const;
		T *operator ->();
		
		void incr();
		void decr();
		size_t nrefs() const;

		template <typename U>
		Ptr<U, ImpT> cast() const;
		void set(PtrImp<ImpT> *src);
	private:
		PtrImp<ImpT> *_imp;
	};

	template <typename T, typename ImpT>
	Ptr<T, ImpT>::Ptr(): _imp(nullptr) { }

	template <typename T, typename ImpT>
	Ptr<T, ImpT>::Ptr(PtrImp<ImpT> &imp): Ptr<T, ImpT>() { set(&imp); }

	template <typename T, typename ImpT>
	Ptr<T, ImpT>::Ptr(const Ptr<T, ImpT> &src): Ptr<T, ImpT>(*src._imp) { }

	template <typename T, typename ImpT>
	template <typename U>
	Ptr<T, ImpT>::Ptr(const Ptr<U, ImpT> &src): Ptr<T, ImpT>(*src._imp) { }

	template <typename T, typename ImpT>
	template <typename... ArgsT>
	Ptr<T, ImpT>::Ptr(size_t nrefs, ArgsT &&... args):
		_imp(new PtrImp<ImpT>(nrefs, forward<ArgsT>(args)...)) { }

	template <typename T, typename ImpT>
	Ptr<T, ImpT>::~Ptr() {
		if (_imp) { decr(); }
	}

	template <typename T, typename ImpT>
	const Ptr<T, ImpT> &Ptr<T, ImpT>::operator =(const Ptr<T, ImpT> &src) {
		set(src._imp);
		return *this;
	}

	template <typename T, typename ImpT>
	template <typename U>
	const Ptr<T, ImpT> &Ptr<T, ImpT>::operator =(const Ptr<U, ImpT> &src) {
		set(src._imp);
		return *this;
	}

	template <typename T, typename ImpT>
	const T &Ptr<T, ImpT>::operator *() const {
		if (!_imp) { throw Error("Dereferencing null ptr"); }
		return _imp->val;
	}

	template <typename T, typename ImpT>
	T &Ptr<T, ImpT>::operator *() {
		if (!_imp) { throw Error("Dereferencing null ptr"); }
		return _imp->val;
	}

	template <typename T, typename ImpT>
	const T *Ptr<T, ImpT>::operator ->() const {
		if (!_imp) { throw Error("Accessing null ptr"); }
		return &_imp->val;
	}

	template <typename T, typename ImpT>
	T *Ptr<T, ImpT>::operator ->() {
		if (!_imp) { throw Error("Accessing null ptr"); }
		return &_imp->val;
	}

	template <typename T, typename ImpT>
	void Ptr<T, ImpT>::incr() {
		if (!_imp) { throw Error("Incrementing null ptr"); }
		_imp->nrefs++;
	}

	template <typename T, typename ImpT>
	void Ptr<T, ImpT>::decr() {
		if (!_imp) { throw Error("Decrementing null ptr"); }
		if (!(--_imp->nrefs)) { delete _imp; }
	}

	template <typename T, typename ImpT>
	size_t Ptr<T, ImpT>::nrefs() const { return _imp ? _imp->nrefs : 0; }

	template <typename T, typename ImpT>
	template <typename U>
	Ptr<U, ImpT> Ptr<T, ImpT>::cast() const { return Ptr<U, ImpT>(*_imp); }

	template <typename T, typename ImpT>
	void Ptr<T, ImpT>::set(PtrImp<ImpT> *src) {
		if (_imp == src) { return; }
		if (_imp) { decr(); }
		_imp = src;
		if (_imp) { incr(); }
	}
}

#endif
