#ifndef COMPLIB_XXC_XXC_CFG_H_
#define COMPLIB_XXC_XXC_CFG_H_
// XXC Crypto library
// Simple but efficient way to get some protection for your app
// Based on elliptic cryptography and some of the ciphers
// 2007 SiGMan

#ifdef OS_WINCE
#if defined(ARM) || defined(_ARM_) || defined(MIPS) || defined(SH3)
//#	define XXC_NO_UNALIGNED_ACCESS
#	define XXC_NOLOOKUP
#	define XXC_UNALIGNED_BYTE_READ
#endif

#endif
#define XXC_CFG_NO_SEH

// true for pocketPC and X86 - but better perform a check in startup
#define XXC_CPU_LE

#ifndef always_inline
#ifndef DEBUG1234
#	define always_inline __forceinline
#else
#	define always_inline __inline
#endif

#endif

#endif //COMPLIB_XXC_XXC_CFG_H_
