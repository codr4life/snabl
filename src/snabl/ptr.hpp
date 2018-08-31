#ifndef SNABL_PTR_HPP
#define SNABL_PTR_HPP

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
	
	template <typename T>
	class Ptr {
	public:
		Ptr();
		Ptr(const Ptr<T> &src);
		Ptr(PtrImp<T> *imp);

		template <typename... ArgsT>
		Ptr(size_t nrefs, ArgsT &&... args);

		~Ptr();
		
		const Ptr<T> &operator =(const Ptr<T> &src);
		const T &operator *() const;
		T &operator *();
		
		void incr();
		void decr();
		size_t nrefs() const;

		template <typename U>
		Ptr<U> cast();
	private:
		PtrImp<T> *_imp;
		void set(PtrImp<T> *src);
	};

	template <typename T>
	Ptr<T>::Ptr(): _imp(nullptr) { }

	template <typename T>
	Ptr<T>::Ptr(const Ptr<T> &src): _imp(nullptr) { set(src._imp); }

	template <typename T>
	Ptr<T>::Ptr(PtrImp<T> *imp): _imp(imp) { }

	template <typename T>
	template <typename... ArgsT>
	Ptr<T>::Ptr(size_t nrefs, ArgsT &&... args):
		_imp(new PtrImp<T>(nrefs, forward<ArgsT>(args)...)) { }

	template <typename T>
	Ptr<T>::~Ptr() { decr(); }

	template <typename T>
	const Ptr<T> &Ptr<T>::operator =(const Ptr<T> &src) {
		set(src._imp);
		return *this;
	}

	template <typename T>
	const T &Ptr<T>::operator *() const {
		if (!_imp) { throw Error("Dereferencing nil ptr"); }
		return _imp->val;
	}

	template <typename T>
	T &Ptr<T>::operator *() {
		if (!_imp) { throw Error("Dereferencing nil ptr"); }
		return _imp->val;
	}

	template <typename T>
	void Ptr<T>::incr() {
		if (!_imp) { throw Error("Decrementing nil ptr"); }
		_imp->nrefs++;
	}

	template <typename T>
	void Ptr<T>::decr() {
		if (!_imp) { throw Error("Decrementing nil ptr"); }
		if (!(--_imp->nrefs)) { delete _imp; }
	}

	template <typename T>
	size_t Ptr<T>::nrefs() const { return _imp ? _imp->nrefs : 0; }

	template <typename T>
	template <typename U>
	Ptr<U> Ptr<T>::cast() { return Ptr<U>(new PtrImp<U>(1, _imp->val)); }

	template <typename T>
	void Ptr<T>::set(PtrImp<T> *src) {
		if (_imp == src) { return; }
		if (_imp) { decr(); }
		_imp = src;
		if (_imp) { incr(); }
	}
}

#endif
