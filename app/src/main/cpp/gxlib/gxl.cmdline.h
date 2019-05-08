#ifndef GXLIB_GXL_CMDLINE_H_
#define GXLIB_GXL_CMDLINE_H_

class iCmdLineParser
{
public:
	iCmdLineParser(LPCTSTR lpstrCmdLine);

	inline uint32 Count() const { return m_argList.GetSize(); }
	inline const iStringT& Arg(uint32 idx) { check(idx<m_argList.GetSize()); return m_argList[idx]; }

private:
	void Parse(LPCTSTR lpstrCmdLine);

private:
	iSimpleArray<iStringT>	m_argList;
};


#endif //GXLIB_GXL_CMDLINE_H_
