#ifndef COMPLIB_XXC_XXC_FBLOCK_H_
#define COMPLIB_XXC_XXC_FBLOCK_H_

namespace xxc {

template< typename T, size_t SZ >
class fblock
{
public:
	typedef	T			 value_type;
	typedef fblock<T,SZ> this_type;

    typedef T&    		reference;
    typedef const T&	const_reference;
    typedef T*    		iterator;
    typedef const T*   	const_iterator;

	typedef T			param_type;
public:
	fblock()
	{}

//	fblock( const this_type& b )
//	{ memcpy( data_, b.data_, sizeof(data_) ); return *this; }

//	this_type& operator=( const this_type& b )
//	{ memcpy( data_, b.data_, sizeof(data_) ); return *this; }
	void assign( const T* s )
	{ memcpy( data_, s, sizeof(data_) ); }

	void clear()
	{ memset( data_, 0, sizeof(data_) ); }


	reference		operator[]( size_t pos )
	{ return data_[pos]; }
	const_reference operator[]( size_t pos )  const
	{ return data_[pos]; }
	
	iterator begin()
	{ return &data_[0]; }
	const_iterator begin() const
	{ return &data_[0]; }
	
	iterator end()
	{ return &data_[SZ]; }
	const_iterator end() const
	{ return &data_[SZ]; }

	bool operator==( const this_type& v ) const
	{ return memcmp( data_, v.data_, sizeof(data_) ) == 0; }

	size_t max_size() const
	{ return SZ-1; }

protected:
	T		data_[SZ];

};

} // xxc


#endif //COMPLIB_XXC_XXC_FBLOCK_H_
