#ifndef GXLIB_XXL_REF_PTR_H_
#define GXLIB_XXL_REF_PTR_H_

//
// policy
template< typename T >
struct ref_ptr_default_policy
{
private:
	class ptr_shim : public T
	{
		void add_ref() const;
		void release() const;
	};
public:
	static inline void inc_ref( const T* body )
	{ body->add_ref(); }
	static inline void dec_ref( const T* body )
	{ body->release(); }
	
	typedef ptr_shim* safe_ptr_t;

	static inline safe_ptr_t make_safe_ptr( T* body )
	{ return (safe_ptr_t)body; }
};

//
// ptr class
template< typename T >
class ref_ptr : protected ref_ptr_default_policy<T>
{
public:
	typedef ref_ptr_default_policy<T>	policy_t;

	typedef ref_ptr<T>					this_type;
	typedef T*							pointer;
	typedef T&							reference;

	ref_ptr()	
	: ptr_(0)
	{}

	ref_ptr( T* p, bool adopt = false )
	: ptr_(p)
	{	
		if ( ptr_ != 0 && !adopt ) policy_t::inc_ref( ptr_ );
	}

	ref_ptr( const ref_ptr& other )
	: ptr_(other.ptr_)
	{
		if ( ptr_ != 0 ) policy_t::inc_ref( ptr_ );
	}

	~ref_ptr()
	{ if ( ptr_ != 0 ) policy_t::dec_ref(ptr_); }

	this_type& operator=( const this_type& rhs )
	{
		this_type(rhs).swap( *this );
		return *this;
	}

	pointer get() const
	{ return ptr_; }

	void reset()
	{ 
		if ( ptr_ != 0 ) {
			policy_t::dec_ref(ptr_);
			ptr_ = 0;
		}
	}

	reference operator*() const
	{ 
		check( ptr_ != 0 );
		return *ptr_;
	}

	typename policy_t::safe_ptr_t operator->() const
	{
		check( ptr_ != 0 );
		return policy_t::make_safe_ptr( ptr_ );
	}

	operator bool() const
	{ return ptr_ == 0 ? false : true; }

	bool operator!() const
	{ return ptr_ == 0; }

	void swap( this_type& other )
	{
		pointer tmp = other.ptr_;
		other.ptr_ = ptr_;
		ptr_ = tmp;
	}

private:
	T* ptr_;
};

#endif //GXLIB_XXL_REF_PTR_H_
