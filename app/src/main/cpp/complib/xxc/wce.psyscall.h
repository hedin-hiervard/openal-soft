#ifndef COMPLIB_XXC_WCE_PSYSCALL_H_
#define COMPLIB_XXC_WCE_PSYSCALL_H_
//
// Windows CE PSysCall inteface
// Specifies how to call system API directly

#if !defined(ARM)
//#error "Works only on ARM!"

#endif
#define SYSCALL_STARTADDR 		0xF0010000
#define SYSCALL_STARTADDR_CE6	0xF1020000

// retrieves API Entry address
#define SYSCALL_ADDR(mid)        (SYSCALL_STARTADDR - (mid)*4)
// declares function 
#define SYSCALL_DECL(type, mid, args) (*(type (*)args)SYSCALL_ADDR(mid))

// Important API functions
#define SCAPI_VirtualAlloc		3
#define SCAPI_VirtualFree		4
#define SCAPI_VirtualProtect    5
#define SCAPI_KernelIoControl	99
#define SCAPI_SetKMode			108
#define SCAPI_NKTerminateThread	87

#define SCAPI6_VirtualAlloc		?
#define SCAPI6_VirtualFree		?
#define SCAPI6_VirtualProtect   ?
#define SCAPI6_KernelIoControl	39
#define SCAPI6_SetKMode			?
#define SCAPI6_NKTerminateThread	?

// 420 - 500
#define W32_KernelLibIoControl4  170
#define W32_KernelLibIoControl5  170
#define W32_KernelLibIoControl6	40

#define KLIO_CE4 (SYSCALL_STARTADDR - W32_KernelLibIoControl4*4)
#define KLIO_CE5 KLIO_CE4
#define KLIO_CE6 (SYSCALL_STARTADDR_CE6 - W32_KernelLibIoControl6*4)

// Predefined Kernel Module numbers
#define KMOD_INVALID    0
#define KMOD_CORE       1
#define KMOD_DBG        2               // Debugger specific extensions
#define KMOD_CELOG      3
#define KMOD_VERIFIER   4
#define KMOD_OAL        5               // Pass through to OEMIoControl
#define KMOD_MAX        5


#endif //COMPLIB_XXC_WCE_PSYSCALL_H_
