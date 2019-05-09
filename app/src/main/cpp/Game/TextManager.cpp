#include "stdafx.h"
#include "lzo/lzo.h"

//#include "xxc/xxc.cfg.h"
//#include "xxc/xxc.defs.h"
//#include "xxc/xxc.cipher.h"
//#include "xxc/xxc.shash.h"
//#include "xxc/xxc.app.h"
//#include "xxc/xxc.dllfn.h"


iTextManager::iTextManager()
{}

iTextManager::~iTextManager()
{
	Cleanup();
}

void iTextManager::Cleanup()
{
}

bool ProcessString(iStringT& str)
{
	str.Trim();

	if (str.Empty() || str[0] != L'{') return false;
	str = str.Mid(str.ReverseFind(L'{'));
	str.Trim(L"{}\n");

	return true;
}

bool iTextManager::Init()
{
#ifdef OS_WINCE
	// Fill Secret structure
	if (pSecret->state) {
		void* mod = xxc::dll_module( XXC_HV_COREDLL );
		pSecret->fnKernelIoControl = xxc::dll_function( mod, XXC_HV_KERNELIOCTRL );
		pSecret->fnGetDeviceUniqueID = xxc::dll_function( mod, XXC_HV_GETUNIQUEID );
		pSecret->fnGetFileAttributesEx = xxc::dll_function( mod, XXC_HV_GETFILEATTEX );
	}
#endif

#if defined (TRANSLATOR_VERSION)
	// Load Text resources
	iFilePtr file(OpenWin32File(gRootPath + L"language.dat"));
	if (file) {
		m_bHasLngFile = true;
		const uint32 buff_siz = 2048;
		iCharW buff[buff_siz];
		file->Read(buff, sizeof(iCharW));
		if (buff[0] != 0xFEFF) {
			m_bHasLngFile = false;
		} else {
			uint32 reserve = 0;
			uint32 readen = 0;
			while (readen = file->Read(buff+reserve, (buff_siz-reserve) * sizeof(iCharW))) {
				uint32 bpos = 0;
				for (uint32 nn=bpos; nn<readen; ++nn) {
					if (buff[nn] == L'\n') {
						iStringT str(buff+bpos, nn-bpos);
						if (ProcessString(str)) {
							m_lngData.Add(str);
						}
						bpos = nn+1;
					}
				}

				memcpy(buff, buff+bpos, (buff_siz-bpos) * sizeof(iCharW));
				reserve = buff_siz-bpos;
			}
		}

	} else {
		m_bHasLngFile = false;
	}
#endif

	return true;
}

