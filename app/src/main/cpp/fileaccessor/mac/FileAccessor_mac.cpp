
#include "IO_POSIX.h"
#include "IO_ANDROID.h"
#include "GLOB_POSIX.h"
#include "IO_MAC.h"
#include "NULL_EXTRACTOR.h"

using namespace std;
using namespace std::chrono;

namespace fileaccessor
{
    void FileAccessor::platformInit()
    {
        m_reader = new FileReaderPOSIX();
        m_writer = new FileWriterPOSIX();
        m_enumerator = new FileEnumeratorPOSIX();
        m_resolver = new FileResolverMac(m_appName.value());
        m_extractor = new FileExtractorNull();
    }
}
