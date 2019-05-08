#ifndef GXLIB_XXL_ARRAY_STG_H_
#define GXLIB_XXL_ARRAY_STG_H_

#include "xxl.array.meta.h"
#include "xxl.array.mem.h"

template< typename Tp >
class dynamic_storage : public meta::internal_class< dynamic_storage< Tp > >
{
public:

	typedef Tp			value_type;
	typedef Tp*			iterator;
	typedef const Tp*	const_iterator;

	struct buffer_ptrs {
		iterator	first;
		iterator	last;
		inline buffer_ptrs( iterator f, iterator l )
		: first(f), last(l) {}
	};

private:

	typedef dynamic_storage<Tp>		this_type;

	iterator	buffer_;
	iterator	end_;

	dynamic_storage( const dynamic_storage& );
	dynamic_storage& operator=( const dynamic_storage& );

public:

	inline dynamic_storage()
	: buffer_( 0 )
	, end_( 0 )
	{}

	inline explicit dynamic_storage( size_t elem )
	{
		buffer_ = alloc( elem );
		end_ = buffer_ + elem;
	}

	inline ~dynamic_storage()
	{
		size_t elem = (size_t)( end_ - buffer_ );
		dealloc( buffer_, elem );
	}

protected:

	inline iterator first()
	{ return buffer_; }

	inline const_iterator first() const
	{ return buffer_; }

	inline iterator end_of_storage()
	{ return end_; }

	inline const_iterator end_of_storage() const
	{ return end_; }

	inline size_t capacity() const
	{ return (size_t)( end_ - buffer_ ); }


	inline bool need_grow( size_t newSize ) const
	{ return newSize > capacity(); }

protected:

	buffer_ptrs grow_begin( size_t newSize )
	{
		size_t newCapacity = maximum_of_two( capacity() * 2, ( newSize + 1 ) );
		// some alignment optimizations here...

		iterator newBuffer = alloc( newCapacity );
		return buffer_ptrs( newBuffer, newBuffer + newCapacity );
	}

	void grow_commit( buffer_ptrs newBuffer ) 
	{
		if ( buffer_ ) {
			dealloc( buffer_, (size_t)(end_ - buffer_) );
		}
		buffer_ = newBuffer.first;
		end_	= newBuffer.last;
	}

	void grow_abort( buffer_ptrs newBuffer )
	{
		dealloc( newBuffer.first, (size_t)(newBuffer.last - newBuffer.first) );
	}

	iterator alloc( size_t size )
	{
		return (iterator)malloc( size * sizeof(Tp) );
	}

	void dealloc( iterator p, size_t size )
	{
		free( p );
	}

public:

	void swap( this_type& rhs )
	{
		::swap2( buffer_, rhs.buffer_ );
		::swap2( end_, rhs.end_ );
	}
};



#endif //GXLIB_XXL_ARRAY_STG_H_
