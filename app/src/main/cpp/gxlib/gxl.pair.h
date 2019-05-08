#ifndef GXLIB_GXL_PAIR_H_
#define GXLIB_GXL_PAIR_H_

template< typename T, typename U >
class iPair
{
public:

	/// Initializes first to T() and second to U(). 
	iPair( const T &f, const U& s )
	: first(f)
	, second(s)
	{}

	/// Default constructor
	iPair()
	{}

	/// Copy constructor
	iPair( const iPair& p )
	: first( p.first )
	, second( p.second )
	{}

	/// Assignment operator
	iPair& operator = ( const iPair& other )
	{
		first	= other.first;
		second	= other.second;
		return *this;
	}

	/// Equality operator
	bool operator == ( const iPair& other ) const
	{
		return ( first == other.first ) && ( second == other.second );
	}

	/// Inequality operator
	bool operator != ( const iPair& other ) const
	{
		return !operator==(other);
	}

	T first;
	U second;
};

template< typename T, typename U >
iPair<T,U> MakePair( const T& first, const U& second ) 
{
	return iPair<T,U>( first, second );
}


#endif //GXLIB_GXL_PAIR_H_
