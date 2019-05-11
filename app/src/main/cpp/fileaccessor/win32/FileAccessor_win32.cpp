
#include <string>
#include <iostream>
#include <time.h>
#include <ctime>
#include <stdio.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "shlobj.h"

#include "utf8/utf8.h"

#include "FileAccessor.h"
#include "IO_WIN32.h"
#include "NULL_EXTRACTOR.h"

using namespace std;
using namespace utf8;
using namespace std::chrono;

namespace fileaccessor
{
    void FileAccessor::platformInit()
    {
        m_reader = new FileReaderWin32();
        m_writer = new FileWriterWin32();
        m_enumerator = new FileEnumeratorWin32();
        m_resolver = new FileResolverWin32(m_appName.value(), m_companyName.value());
        m_extractor = new FileExtractorNull();
    }

}
