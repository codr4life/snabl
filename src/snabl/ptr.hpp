#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <typename T>
	struct PtrImp {
		T val;
		size_t nrefs;

		template <typename... ArgsT>
		PtrImp(size_t nrefs, ArgsT &&... args):
			val(forward<ArgsT>(args)...), nrefs(nrefs) { }
	};

	template <typename T>
	class Ptr {
	public:
		Ptr(nullptr_t _ = nullptr): _imp(nullptr) { }
		
		explicit Ptr(PtrImp<T> &imp): Ptr<T>() { set(&imp); }

		Ptr(const Ptr<T> &src): Ptr<T>(*src._imp) { }

		Ptr(Ptr<T> &&src): _imp(src._imp) { src._imp = nullptr; }
		
		template <typename... ArgsT>
		Ptr(size_t nrefs, ArgsT &&... args):
			_imp(new PtrImp<T>(nrefs, forward<ArgsT>(args)...)) { }

		~Ptr() {
			if (_imp) { decr(); }
		}
		
		const Ptr<T> &operator =(const Ptr<T> &src) {
			set(src._imp);
			return *this;
		}

		const T &operator *() const { return _imp->val; }
		
		T &operator *() { return _imp->val; }

		const T *operator ->() const { return &_imp->val; }
		
		T *operator ->() { return &_imp->val; }

		operator bool() const { return _imp; }
		
		void incr() { _imp->nrefs++; }
		
		void decr() {
			if (!(--_imp->nrefs)) {
				delete _imp;
				_imp = nullptr;
			}
		}
		
		size_t nrefs() const { return _imp ? _imp->nrefs : 0; }

		void set(PtrImp<T> *src) {
			if (_imp != src) {
				if (_imp) { decr(); }
				if ((_imp = src)) { incr(); }
			}
		}
	private:
		PtrImp<T> *_imp;
	};
}

#endif
