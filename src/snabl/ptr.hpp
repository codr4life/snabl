#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <typename T>
	class Ptr {
	public:
		struct Make {};

		template <typename... ArgsT>
		static Ptr<T> make(ArgsT &&... args) {
			return Ptr<T>(Make(), forward<ArgsT>(args)...);
		}
		
		struct Imp {
			T val;
			size_t nrefs;
			
			template <typename... ArgsT>
			Imp(ArgsT &&... args): val(forward<ArgsT>(args)...), nrefs(0) { }
		};

		Ptr(nullptr_t _ = nullptr): _imp(nullptr) { }

		Ptr(const Ptr<T> &src): _imp(src._imp) {
			if (_imp) { _imp->nrefs++; }
		}
		
		Ptr(Ptr<T> &&src): _imp(src._imp) { src._imp = nullptr; }
		
		~Ptr() {
			if (_imp) { decr(); }
		}
		
		const Ptr<T> &operator =(const Ptr<T> &src) {
			if (_imp != src._imp) {
				if (_imp) { decr(); }
				if ((_imp = src._imp)) { _imp->nrefs++; }
			}

			return *this;
		}

		Ptr<T> &operator =(Ptr<T> &&src) {
			if (_imp != src._imp) {
				if (_imp) { decr(); }
				_imp = src._imp;
				src._imp = nullptr;
			}
			
			return *this;
		}

		T *get() const { return const_cast<T *>(&_imp->val); }		
		T *get() { return &_imp->val; }

		T &operator *() const { return const_cast<T &>(_imp->val); }		
		T &operator *() { return _imp->val; }

		T *operator ->() const { return const_cast<T *>(&_imp->val); }		
		T *operator ->() { return &_imp->val; }

		operator bool() const { return _imp; }
		
		size_t nrefs() const { return _imp ? _imp->nrefs : 0; }
	private:
		Imp *_imp;

		template <typename... ArgsT>
		Ptr(const Make &_, ArgsT &&... args):
			_imp(new Imp(forward<ArgsT>(args)...)) { }
		
		Ptr(Imp *imp): _imp(imp) {
			if (_imp) { _imp++; }
		}

		void decr() {
			if (! _imp->nrefs) {
				delete _imp;
				_imp = nullptr;
			} else {
				_imp->nrefs--;
			}
		}
	};

	template <typename T>
	ostream &operator <<(ostream &out, const Ptr<T> &x) {
		out << x.get() ? "null" : *x;
		return out;
	}
}

#endif
