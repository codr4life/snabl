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

	template <typename T, typename ImpT=T>
	class Ptr {
	public:
		Ptr(nullptr_t _ = nullptr): _imp(nullptr) { }
		
		explicit Ptr(PtrImp<ImpT> &imp): Ptr<T, ImpT>() { set(&imp); }
		Ptr(const Ptr<T, ImpT> &src): Ptr<T, ImpT>(*src._imp) { }

		template <typename U>
		Ptr(Ptr<U, ImpT> src): Ptr<T, ImpT>(*src._imp) { }

		template <typename U>
		Ptr(Ptr<U, ImpT> &&src): _imp(src._imp) { src._imp = nullptr; }

		template <typename... ArgsT>
		Ptr(size_t nrefs, ArgsT &&... args):
			_imp(new PtrImp<ImpT>(nrefs, forward<ArgsT>(args)...)) { }

		~Ptr() {
			if (_imp) { decr(); }
		}
		
		Ptr<T, ImpT> operator =(Ptr<T, ImpT> src) {
			set(src._imp);
			return *this;
		}

		template <typename U>
		Ptr<T, ImpT> operator =(Ptr<U, ImpT> src) {
			set(src._imp);
			return *this;
		}

		template <typename U>
		Ptr<T, ImpT> &operator =(Ptr<U, ImpT> &&src) {
			cout << "massign" << endl;
			
			if (_imp != src._imp) {
				if (_imp) { decr(); }
				_imp = src._imp;
				src._imp = nullptr;
			}
			
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

		template <typename U>
		Ptr<U, ImpT> cast() const { return Ptr<U, ImpT>(*_imp); }
		
		void set(PtrImp<ImpT> *src) {
			if (_imp != src) {
				if (_imp) { decr(); }
				if ((_imp = src)) { incr(); }
			}
		}
	private:
		PtrImp<ImpT> *_imp;
	};
}

#endif
