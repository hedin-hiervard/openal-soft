#ifndef COMPLIB_XXC_XXC_FILE_H_
#define COMPLIB_XXC_XXC_FILE_H_
//
// Protected file interface
//
// implements transparent xxc::cipher with salt over the file-io
// (NO CACHING IS IMPLEMENTED YET)
//
namespace xxc {

///
///
iFileI* CreateXFile( const iStringT& fname, uint32 key );
iFileI* OpenXFile( const iStringT& fname, uint32 key );

} //xxc

#endif //COMPLIB_XXC_XXC_FILE_H_
