#include "gxl.inc.h"
#include "gxl.ctr.array.h"
#include "gxl.cmdline.h"

iCmdLineParser::iCmdLineParser(LPCTSTR lpstrCmdLine)
{
	Parse(lpstrCmdLine);
}

void iCmdLineParser::Parse(LPCTSTR lpstrCmdLine)
{
	const iCharT* begin = lpstrCmdLine;
	const iCharT* endChar = lpstrCmdLine + _tcslen(lpstrCmdLine)+1;

	while (begin < endChar) {
		if (*begin == '\"') {
			++begin;
			if (begin == endChar) continue;
			if (const iCharT* f = _tcschr(begin, '\"')) {
				if (f == begin){
					++begin;
				} else {
					m_argList.Add(iStringT(begin, f-begin));
					begin = f+1;
				}
			} else {
				if ( _tcslen(begin) > 0 ) m_argList.Add(begin);
				begin = endChar;
			}
		} else {
			if (const iCharT* f = _tcschr(begin, ' ')) {
				if (begin == f) {
					++begin;
				} else {
					m_argList.Add(iStringT(begin, f-begin));
					begin = f+1;
				}
			} else {
				if ( _tcslen(begin) > 0 ) m_argList.Add(begin);
				begin = endChar;
			}
		}
	}
}
