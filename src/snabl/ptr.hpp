#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

#include "snabl/std.hpp"

namespace snabl {
	template <typename T>
	class Ptr {
	public:
		struct Make {};
		
		struct Imp {
			T val;
			size_t nrefs;
			
			template <typename... ArgsT>
			Imp(ArgsT &&... args): val(forward<ArgsT>(args)...), nrefs(1) { }
		};

		Ptr(nullptr_t _ = nullptr): _imp(nullptr) { }
		Ptr(const Ptr<T> &src): Ptr<T>(src._imp) { }
		Ptr(Ptr<T> &&src): _imp(src._imp) { src._imp = nullptr; }
		
		template <typename... ArgsT>
		Ptr(const Make &_, ArgsT &&... args):
			_imp(new Imp(forward<ArgsT>(args)...)) { }

		~Ptr() {
			if (_imp) { decr(); }
		}
		
		const Ptr<T> &operator =(const Ptr<T> &src) {
			set(src._imp);
			return *this;
		}

		Ptr<T> &operator =(Ptr<T> &&src) {
			if (_imp) { decr(); }
			_imp = src->imp;
			src->_imp = nullptr;
			return *this;
		}

		T &operator *() const { return const_cast<T &>(_imp->val); }		
		T &operator *() { return _imp->val; }

		T *operator ->() const { return const_cast<T *>(&_imp->val); }		
		T *operator ->() { return &_imp->val; }

		operator bool() const { return _imp; }
		
		size_t nrefs() const { return _imp ? _imp->nrefs : 0; }
	private:
		Imp *_imp;

		Ptr(Imp *imp): Ptr<T>() {
			if ((_imp = imp)) { incr(); }
		}

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

	template <typename T, typename... ArgsT>
	Ptr<T> make_ptr(ArgsT &&... args) {
		thread_local static const typename Ptr<T>::Make tag;
		return Ptr<T>(tag, forward<ArgsT>(args)...);
	}
}

#endif
