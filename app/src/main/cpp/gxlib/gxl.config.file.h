#ifndef GXLIB_GXL_CONFIG_FILE_H_
#define GXLIB_GXL_CONFIG_FILE_H_

//////////////////////////////////////////////////////////////////////////
class IConfigReaderProc
{
public:
	
	virtual void OnConfigGroup(const iStringA& groupName) =0;
	virtual void OnConfigKey(const iStringA& keyName, const iStringA& keyValue) =0;
};

//////////////////////////////////////////////////////////////////////////
bool ParseConfigFile(const iStringT& fname, IConfigReaderProc& readerProc);


#endif //GXLIB_GXL_CONFIG_FILE_H_
