#ifndef COMPLIB_XXC_XXC_SYSINFO_H_
#define COMPLIB_XXC_XXC_SYSINFO_H_
//
// system information class
//

#include "xxc.params.h"
#include "xxc.fblock.h"

namespace xxc {

struct dev_type {
	enum type {
		unknown,
		pocket_pc,
		smartphone
	};
};

struct dev_info
{
	dev_type::type	type;
	uint32			version;
	uint32			flags;

	TCHAR			cpu[32];
	TCHAR			oeminfo[128];
	TCHAR			platform[128];
	TCHAR			lang[5];
	DWORD			oem_hash;
};

// returns hash of the OEMinfo 
// so we can actually detect microsoft emulator
uint32 dev_acquire( dev_info& info );


} // xxc


#endif //COMPLIB_XXC_XXC_SYSINFO_H_
