#ifndef COMPLIB_XXC_WCE_WM5ID_H_
#define COMPLIB_XXC_WCE_WM5ID_H_
//
// WM5 GetDeviceUniqueId style hash function
//

#include "xxc.defs.h"

namespace xxc {

// internal struct, context
typedef struct {
   uint32	unknown[6];
   uint32	state[5];
   uint32	count[2];
   uint8	buffer[64];
} wm5id_context;

void wm5id_init(wm5id_context* context);
void wm5id_update( wm5id_context* context, const uint8* buf, uint32 size );
void wm5id_final( wm5id_context* context );

} //xxc


#endif //COMPLIB_XXC_WCE_WM5ID_H_
