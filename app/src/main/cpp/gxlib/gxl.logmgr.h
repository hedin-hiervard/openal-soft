#ifndef GXLIB_GXL_LOGMGR_H_
#define GXLIB_GXL_LOGMGR_H_

class iLogMgr
{
public:
	iLogMgr();
	~iLogMgr();

	bool Init(const iStringT& fname);
	void Log(const iStringT& entry);
	
private:
	iFilePtr m_pLogFile;
};


#endif //GXLIB_GXL_LOGMGR_H_
