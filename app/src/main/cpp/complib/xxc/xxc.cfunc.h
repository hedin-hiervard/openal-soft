#ifndef COMPLIB_XXC_XXC_CFUNC_H_
#define COMPLIB_XXC_XXC_CFUNC_H_


typedef uint32 (*constfunc_t)( void* );



#define XXC_DECL_CONST( name ) \
	extern 	constfunc_t name##_fn; \
	/* */

#define XXC_IMPL_CONST( name ) \
	constfunc_t name##_fn = cinit; \
	/* */

#define XXC_GETCONST( name ) \
	((name##_fn)( (void*)&name##_fn )) \
	/* */

uint32 cinit( void* param )
{
	constfunc_t self = (constfunc_t)( *(constfunc_t*)param );
	if ( self == cinit ) {
		//initialization
	} else {
		assert(0);
	}
}

#endif //COMPLIB_XXC_XXC_CFUNC_H_
