#ifndef GXLIB_GXL_RTTI_H_
#define GXLIB_GXL_RTTI_H_

class NonCopyable
{
	NonCopyable( const NonCopyable& ) {}
	void operator = ( const NonCopyable& ) {}
public:
	NonCopyable() {}
};

typedef void*	CompileTimeIdType;

template< class T >
struct CompileTimeId
{
	static char Cookie;
	static CompileTimeIdType Value()
	{ return &Cookie; }
};

template< class T >
char CompileTimeId<T>::Cookie;

//

class TypeAware
{
public:

	virtual void* CastTo( CompileTimeIdType ) 
	{ return 0; }

public:

	TypeAware() {}
	virtual ~TypeAware() {}

};

template< class Ptr >
Ptr DynamicCast( TypeAware* object )
{
	if ( !object ) return 0;
	return static_cast<Ptr>( object->CastTo( CompileTimeId<Ptr>::Value() ));
}

#define IMPL_TYPEAWARE(ClassType) \
public: \
virtual void* CastTo( CompileTimeIdType type ) { \
return (CompileTimeId<ClassType*>::Value() == type ? this : 0); \
} \
public:\
/* */

#endif //GXLIB_GXL_RTTI_H_
