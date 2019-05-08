#include "gxl.inc.h"
#include "gxl.config.file.h"



bool ParseConfigFile(const iStringT& fname, IConfigReaderProc& readerProc)
{
#ifndef OS_APPLE
	FILE* file = _tfopen(fname.CStr(),_T("rt"));
#else
	FILE* file = fopen(CvtT2A<>(fname.CStr()), "r");
#endif
	if (!file) return false;

	char buff[1024];
	while (fgets(buff, 1024, file)) {
		iStringA strBuff(buff);

		// Trim spaces
		strBuff.Trim();
		// Check for full line comment
		if (strBuff[0] == '/' && strBuff[1] == '/') continue;
		// split key and value
		sint32 epos = strBuff.Find('=');
		if (epos < 1 || (epos+1) >= (sint32)strBuff.Length()) continue;
		iStringA key = strBuff.Left(epos);
		key.Trim();
		iStringA value = strBuff.Right(strBuff.Length() - (epos+1));
		value.Trim();
		if (key.Empty() || value.Empty()) continue;
		readerProc.OnConfigKey(key, value);
	}

	return true;
}
