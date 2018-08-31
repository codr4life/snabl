#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <typename T>
	class Ptr {
	public:
		struct Imp {
			T val;
			size_t nrefs;
			
			template <typename... ArgsT>
			Imp(size_t nrefs, ArgsT &&... args):
				val(forward<ArgsT>(args)...), nrefs(nrefs) { }
		};

		Ptr(nullptr_t _ = nullptr): _imp(nullptr) { }
		
		explicit Ptr(Imp &imp): Ptr<T>() { set(&imp); }

		Ptr(const Ptr<T> &src): Ptr<T>(*src._imp) { }

		Ptr(Ptr<T> &&src): _imp(src._imp) { src._imp = nullptr; }
		
		template <typename... ArgsT>
		Ptr(size_t nrefs, ArgsT &&... args):
			_imp(new Imp(nrefs, forward<ArgsT>(args)...)) { }

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
		
		size_t nrefs() const { return _imp ? _imp->nrefs : 0; }
	private:
		Imp *_imp;

		void incr() { _imp->nrefs++; }
		
		void decr() {
			if (!(--_imp->nrefs)) {
				delete _imp;
				_imp = nullptr;
			}
		}
		
		void set(Imp *src) {
			if (_imp != src) {
				if (_imp) { decr(); }
				if ((_imp = src)) { incr(); }
			}
		}		
	};
}

#endif
