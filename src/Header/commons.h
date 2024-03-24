#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <wdm.h>
#include <intrin.h>
#include "datatypes.h"

//super skibiy and i dont gave a fuck!
#define unionvolatile volatile union

#include <_RTL_AVL_TREE.h>
#include <_RTL_PROCESS_MODULE_INFORMATION.h>
#include <_RTL_PROCESS_MODULES.h>
#include<_IMAGE_DATA_DIRECTORY.h>
#include<_IMAGE_OPTIONAL_HEADER64.h>
#include <_IMAGE_SECTION_HEADER.h>
#include<_IMAGE_FILE_HEADER.h>
#include <_KLDR_DATA_TABLE_ENTRY.h>
#include <_IMAGE_DOS_HEADER.h>
#include <_IMAGE_NT_HEADERS64.h>
#include <_IMAGE_SECTION_HEADER.h>
#include <_PEB.h>
#include <_EX_PUSH_LOCK.h>
#include <_EX_FAST_REF.h>
#include <_SE_AUDIT_PROCESS_CREATION_INFO.h>
#include <_JOBOBJECT_WAKE_FILTER.h>
#include <_PS_DYNAMIC_ENFORCED_ADDRESS_RANGES.h>
#include <_PS_PROTECTION.h>
#include <_PS_INTERLOCKED_TIMER_DELAY_VALUES.h>
#include <_PS_PROCESS_WAKE_INFORMATION.h>
#include <_ALPC_PROCESS_CONTEXT.h>
#include <_KSTACK_COUNT.h>
#include <_KEXECUTE_OPTIONS.h>
#include <_KWAIT_STATUS_REGISTER.h>
#include <_MMSUPPORT_FLAGS.h>
#include <_MMSUPPORT_INSTANCE.h>
#include <_MMSUPPORT_SHARED.h>
#include <_MMSUPPORT_FULL.h>
#include <_MMPTE.h>
#include <_KAFFINITY_EX.h>
#include <_KTHREAD.h>
#include <_KPROCESS.h>
#include <_EPROCESS.h>
#include <_KIDTENTRY64.h>

#include <_KDPC_LIST.h>
#include <_KHETERO_CPU_QOS.h>
#include <_KTIMER_TABLE_ENTRY.h>
#include <_KTIMER_TABLE_STATE.h>
#include <_MACHINE_FRAME.h>
#include <_PROC_HYPERVISOR_STATE.h>
#include <_KREQUEST_PACKET.h>
#include <_PROC_IDLE_POLICY.h>
#include <_PPM_IDLE_SYNCHRONIZATION_STATE.h>
#include <_PROC_FEEDBACK.h>
#include <_PPM_FFH_THROTTLE_STATE_INFO.h>
#include <_PROC_IDLE_SNAP.h>
#include <_PROC_PERF_CHECK_CONTEXT.h>
#include<_KPRCBFLAG.h>
#include<_PP_LOOKASIDE_LIST.h>
#include<_KSTATIC_AFFINITY_BLOCK.h>
#include<_KDPC_DATA.h>
#include<_KTIMER_TABLE.h>
#include<_RTL_RB_TREE.h>
#include<_PROCESSOR_POWER_STATE.h>
#include<_SYNCH_COUNTERS.h>
#include<_FILESYSTEM_DISK_COUNTERS.h>
#include<_KENTROPY_TIMING_STATE.h>
#include<_IOP_IRP_STACK_PROFILER.h>
#include<_KSHARED_READY_QUEUE.h>
#include<_KSECURE_FAULT_INFORMATION.h>
#include<_KTIMER_EXPIRATION_TRACE.h>
#include<_MACHINE_CHECK_CONTEXT.h>
#include<_REQUEST_MAILBOX.h>
#include <_KDESCRIPTOR.h>
#include <_KSPECIAL_REGISTERS.h>
#include <_KPROCESSOR_STATE.h>
#include <_KPRCB.h>
#include <_ISRDPCSTATS_SEQUENCE.h>
#include<_ISRDPCSTATS.h>
#include<_KINTERRUPT.h>

#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#pragma warning(disable: 4996)  // deprecated
#pragma warning(disable: 4200) // nonstandard extension used: zero-sized array in struct/union


void print(const char* fmt, ...);

uint32_t __stdcall GetSystemFirmwareTable(
    uint32_t FirmwareTableProviderSignature,
    uint32_t FirmwareTableID,
    void* pFirmwareTableBuffer,
    uint32_t BufferSize);