#ifndef SNABL_TYPE_HPP
#define SNABL_TYPE_HPP

namespace snabl {
	class AType {
	};
	
	template <typename ValueT>
	class Type: public AType {
	public:
		Type(const Sym &id);
	private:
		const Sym &_id;
	};

	template <typename ValueT>
	Type<ValueT>::Type(const Sym &id): _id(id) {
	}
}

#endif
