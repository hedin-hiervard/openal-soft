//
// Inline part

namespace xxc {

// Tree management helpers
/////////////////////////////////////////////////////

always_inline
uint32 rmix_2( uint32 in, uint32 base )
{
	return 0xff & ((in ^ (in>>2) ^ (in<<5)) + 0x13);
}

// node's key comparison function (should be stable and strict)
always_inline sint32
node_cmp( const snode_t* a, const uint32 b )
{
	return (sint32)a->key - (sint32)b;
}

// from offset to ptr :  nptr = root + nofs 
#define nptr( ptr, ofs ) ((snode_t*)( (const char*)(ptr) + ((size_t)(ofs)<<4) ))
// from ptr to offset :  nofs = nptr - root
#define nofs( ptr, nptr) ((wofs_t)((const char*)(nptr) - (const char*)(ptr) )>>4)

always_inline
snode_t*
mem_block( void* block, uint32 n )
{
	// logical address block remix function
	//	in = (13 + in * 1979) & 0xff;
	uint32 base = (uint32)block;
	n += (base>>5)|(base<<27);
	n  = 0xff & ((n ^ (n>>2) ^ (n<<5)) + 0x37);
	return (snode_t*)block + n;
}

always_inline
sstore_t*
mem_store( void* block )
{
	return (sstore_t*)mem_block( block, 0 );
}


///
/// Find node by key, strictly inlined
always_inline snode_t* 
store_find( void* block, uint32 key )
{
	sstore_t* tree = mem_store( block );
	snode_t*  it = tree->root;
	snode_t*  nil= tree->nil;

	while ( it != nil ) {
		int cmp = node_cmp( it, key );
		if ( cmp == 0 ) break;
		it = nptr( block, it->link[cmp < 0]);
	}
	return it;
}

} // xxc


#ifndef XXC_STORE_INTERNAL
#undef nptr
#undef nofs
#endif
