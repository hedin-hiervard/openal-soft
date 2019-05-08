#ifndef GXLIB_GXL_SMARTPTR_H_
#define GXLIB_GXL_SMARTPTR_H_

#pragma warning( push )
#pragma warning( disable: 4284 )

template< typename _Tp >
class iSmartPtr
{
private:
	_Tp*	_ptr;				

	iSmartPtr( iSmartPtr const& );
	iSmartPtr& operator = ( iSmartPtr const& );

	typedef iSmartPtr<_Tp> ThisType;

public:
	typedef _Tp		ElementType;

	explicit iSmartPtr( _Tp* p = 0 ) : _ptr( p ) {}
	~iSmartPtr() { delete _ptr; }

	void Reset( _Tp* p = 0 )
	{
		if ( _ptr != p ) {
			ThisType(p).Swap( *this );
		}
	}

	_Tp& operator * () const
	{
		check( _ptr != 0 );
		return *_ptr;
	}

	_Tp* operator -> () const
	{
		check( _ptr != 0 );
		return _ptr;
	}

	_Tp* Get() const
	{ return _ptr; }

	operator bool () const
	{ return _ptr == 0 ? false : true; }

	bool operator ! () const
	{ return _ptr == 0; }

	void Swap( iSmartPtr& s )
	{
		_Tp* tmp = s._ptr;
		s._ptr = _ptr;
		_ptr = tmp;
	}

	_Tp* Giveup()
	{ _Tp* val = _ptr; _ptr = 0; return val; }
};

#pragma warning( pop )


#endif //GXLIB_GXL_SMARTPTR_H_
