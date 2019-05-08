#ifndef COMPLIB_XXC_XXC_CIPHER_H_
#define COMPLIB_XXC_XXC_CIPHER_H_
//
// stream cipher based on salsa12
// NOTE: we use 32bit stream positions, 
// so files with >4Gb size cannot be handled properly!

//#include "xxc.?"

namespace xxc {

//
class cipher
{
public:
	typedef uint32 word_t;

	cipher();
	~cipher();
	
	void clear();
	//void reset( const uint8* key, size_t keylen );
	void reset( const word_t key[4] );

	void process( uint8* stream, size_t length );
	void resync( size_t pos );

protected:
	void keysetup( const word_t key[4] );
	void ivsetup( const word_t iv0, const word_t iv1 );

	size_t	keyleft;
	word_t	kstream[64];
	word_t	state[16];
};


} //xxc


#endif //COMPLIB_XXC_XXC_CIPHER_H_
