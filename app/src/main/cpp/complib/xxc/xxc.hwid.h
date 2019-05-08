#ifndef COMPLIB_XXC_XXC_HWID_H_
#define COMPLIB_XXC_XXC_HWID_H_
//
// Hardware identification module

#include "xxc.fblock.h"

// following approaches are taken
// 1) Using GetDeviceUniqueID if available
// 2) Using KernelIoControl if available and working
// 3) Using gethostname as last resort(*)
// if none is working - bail out
// check internally if the function was really called (results etc)
// if none - bail out "tamper a constants first"

namespace xxc {

void Text2Bin(LPCWSTR text, size_t sizText, unsigned char* bin, size_t sizBin);
void Bin2Text(const unsigned char* bin, size_t sizBin, LPWSTR text, size_t sizText);

void GetDeviceId(xxc::hash& h);

} //xxc


#endif //COMPLIB_XXC_XXC_HWID_H_
