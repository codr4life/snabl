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
		constexpr Ptr(nullptr_t _ = nullptr): _imp(nullptr) { }
		
		explicit constexpr Ptr(PtrImp<ImpT> &imp): Ptr<T, ImpT>() { set(&imp); }
		constexpr Ptr(const Ptr<T, ImpT> &src): Ptr<T, ImpT>(*src._imp) { }

		template <typename U>
		constexpr Ptr(const Ptr<U, ImpT> &src): Ptr<T, ImpT>(*src._imp) { }

		template <typename... ArgsT>
		constexpr Ptr(size_t nrefs, ArgsT &&... args):
			_imp(new PtrImp<ImpT>(nrefs, forward<ArgsT>(args)...)) { }

		~Ptr() {
			if (_imp) { decr(); }
		}
		
		constexpr const Ptr<T, ImpT> &operator =(const Ptr<T, ImpT> &src) {
			set(src._imp);
			return *this;
		}

		template <typename U>
		constexpr const Ptr<T, ImpT> &operator =(const Ptr<U, ImpT> &src) {
			set(src._imp);
			return *this;
		}

		constexpr const T &operator *() const {
			if (!_imp) { throw Error("Dereferencing null ptr"); }
			return _imp->val;
		}
		
		constexpr T &operator *() {
			if (!_imp) { throw Error("Dereferencing null ptr"); }
			return _imp->val;
		}

		constexpr const T *operator ->() const {
			if (!_imp) { throw Error("Accessing null ptr"); }
			return &_imp->val;
		}
		
		constexpr T *operator ->() {
			if (!_imp) { throw Error("Accessing null ptr"); }
			return &_imp->val;
		}

		constexpr operator bool() const { return _imp; }
		
		constexpr void incr() {
			if (!_imp) { throw Error("Incrementing null ptr"); }
			_imp->nrefs++;
		}
		
		constexpr void decr() {
			if (!_imp) { throw Error("Decrementing null ptr"); }
			if (!(--_imp->nrefs)) { delete _imp; }
		}
		
		constexpr size_t nrefs() const { return _imp ? _imp->nrefs : 0; }

		template <typename U>
		constexpr Ptr<U, ImpT> cast() const { return Ptr<U, ImpT>(*_imp); }
		
		constexpr void set(PtrImp<ImpT> *src) {
			if (_imp == src) { return; }
			if (_imp) { decr(); }
			_imp = src;
			if (_imp) { incr(); }
		}
	private:
		PtrImp<ImpT> *_imp;
	};
}

#endif
