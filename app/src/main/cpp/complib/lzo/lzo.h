#ifndef COMPLIB_LZO_LZO_H_
#define COMPLIB_LZO_LZO_H_

namespace LZO {

bool Init();
uint32 Decompress(const unsigned char* lzoBuff, uint32 lzoBuffLen, iDynamicBuffer& rawBuff);
uint32 Compress(const unsigned char* rawBuff, uint32 rawBuffLen, iDynamicBuffer& lzoBuff);

};


#endif //COMPLIB_LZO_LZO_H_
