#ifndef GXLIB_XXL_REF_COUNTABLE_H_
#define GXLIB_XXL_REF_COUNTABLE_H_

//
// base abstract
class ref_countable
{
public:
	virtual 		~ref_countable() 	{}
	virtual void    add_ref() const		=0;
	virtual void	release() const		=0;
};

//
// ref countable wrapper
template< typename T >
class ref_countable_impl : public T
{
public:
	ref_countable_impl()
	: refc_(0)
	{}

	virtual ~ref_countable_impl()
	{}

public:
	virtual void add_ref() const
	{ refc_++; }

	virtual void release() const
	{
		check( refc_ != 0 );
		if ( --refc_ == 0 ) delete this;
	}

private:
	mutable size_t refc_;
};


#endif //GXLIB_XXL_REF_COUNTABLE_H_
