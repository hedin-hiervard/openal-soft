#ifndef GXLIB_XXL_ARRAY_CFG_H_
#define GXLIB_XXL_ARRAY_CFG_H_

#define XXL_USED 1

#define XXL_TRY 
#define XXL_UNWIND( act )
#define XXL_UNWIND_RET( act, ret ) return (ret);

// TODO SHL:
/*

#ifndef OS_LINUX
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE

//never compiled?  //__cdecl
inline void *__cdecl operator new(size_t, void *_P) { return (_P); }
inline void  __cdecl operator delete(void *, void *) { return; }

#endif


#endif
*/

template< typename T >
void swap2( T& a, T& b )
{
	T t = a;
	a = b;
	b = t;
}


template< typename T >
T maximum_of_two( T a, T b )
{
	if ( a < b ) return b;
	else return a;
}


#endif //GXLIB_XXL_ARRAY_CFG_H_
