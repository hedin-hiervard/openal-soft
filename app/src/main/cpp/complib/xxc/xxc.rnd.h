#ifndef COMPLIB_XXC_XXC_RND_H_
#define COMPLIB_XXC_XXC_RND_H_
//
// MTwister random number generator
// Knuth et al
//

namespace xxc {

// isaac rand
class rnd
{
public:
	enum { NParam = 624 };
	enum { MParam = 397 };
	enum { state_sz = NParam + 1 };

	void	reset( uint32 val );
	void	reset( const uint8* seed, size_t seedlen );

	uint32  get();

	void	load( const uint32 st[state_sz] );
	void	save( uint32 st[state_sz] );

private:
	void	forward();

    uint32	state[NParam];
    uint32* next;
    size_t	left;
    bool	inited;
};

} //xxc

#endif //COMPLIB_XXC_XXC_RND_H_
