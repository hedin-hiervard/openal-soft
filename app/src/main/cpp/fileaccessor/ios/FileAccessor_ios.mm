#include "FileAccessor.h"
#include "IO_POSIX.h"
#include "IO_IOS.h"
#include "GLOB_POSIX.h"
#include "NULL_EXTRACTOR.h"

using namespace std;

namespace fileaccessor
{
    void FileAccessor::platformInit()
    {
        m_reader = new FileReaderPOSIX();
        m_writer = new FileWriterPOSIX();
        m_enumerator = new FileEnumeratorPOSIX();
        m_resolver = new FileResolveriOS();
        m_extractor = new FileExtractorNull();
    }
}

