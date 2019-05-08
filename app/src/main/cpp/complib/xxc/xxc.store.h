#ifndef COMPLIB_XXC_XXC_STORE_H_
#define COMPLIB_XXC_XXC_STORE_H_
//
// Secure dynamic storage class
// implements obuscated encrypted inplace associative storage
// based on binary tree structure (jsw-tree)
// Used to pass secure data between functions

#include "xxc.defs.h"

namespace xxc {

typedef size_t 			ptr_t;
typedef unsigned short	wofs_t;

// node structure
struct snode_t
{
	wofs_t				link[2];
	sint32				level;
	uint32				key;
	void*				data;
}; 

const size_t max_snodes = 256 - 1;

// store header
struct sstore_t
{
	uint32		memalloc;	// allocated count (made first to mimic node struct)
	snode_t*	root;		// root node
	snode_t*	nil;		// nil  node (fixed position)
	snode_t*	memfree;	// freelist ptr
};  

// Safety check - both structures should be 16bytes in size
typedef char something_totaly_wrong__[ (sizeof(sstore_t) == 16)*2 - 1 ];
typedef char something_totaly_wrong2_[ (sizeof(sstore_t) == sizeof(snode_t))*2 - 1 ];

void*	store_init();
void	store_free( void* );

snode_t*	store_insert( void* block, uint32 key );
void		store_erase( void* block, uint32 key );
void*		store_shuffle( void* block );
//snode_t* 	store_find( void* block, uint32 key );


} //xxc

#include "xxc.store.inl"


#endif //COMPLIB_XXC_XXC_STORE_H_
