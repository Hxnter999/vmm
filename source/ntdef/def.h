#pragma once
// theres alot of useless shit here im sorry but its from one of my testing projects
// its very annoying to reorganize and only include whats needed so i just reused it :D

#define unionvolatile volatile union

#include "_KAFFINITY_EX.h"
#include "_KEXECUTE_OPTIONS.h"
#include "_KSTACK_COUNT.h"
#include "_KDESCRIPTOR.h"
#include "_KSPECIAL_REGISTERS.h"
#include "_EX_PUSH_LOCK.h"
#include "_EX_PUSH_LOCK_AUTO_EXPAND_STATE.h"
#include "_EX_PUSH_LOCK_AUTO_EXPAND.h"
#include "_EX_FAST_REF.h"
#include "_RTL_AVL_TREE.h"
#include "_SE_AUDIT_PROCESS_CREATION_INFO.h"
#include "_MMSUPPORT_FLAGS.h"
#include "_MMSUPPORT_INSTANCE.h"
#include "_MMSUPPORT_SHARED.h"
#include "_MMSUPPORT_FULL.h"
#include "_ALPC_PROCESS_CONTEXT.h"
#include "_PS_INTERLOCKED_TIMER_DELAY_VALUES.h"
#include "_JOBOBJECT_WAKE_FILTER.h"
#include "_PS_PROCESS_WAKE_INFORMATION.h"
#include "_PS_PROTECTION.h"
#include "_KPROCESSOR_STATE.h"
#include "_KPRCB.h"
#include "_LDR_DLL_LOAD_REASON.h"
#include "_KIDTENTRY64.h"
#include "_SYSTEM_INFORMATION_CLASS.h"

#include "_PS_DYNAMIC_ENFORCED_ADDRESS_RANGES.h"
#include "_KPROCESS.h"
#include "_KPROCESS_STATE.h"
#include "_KPROCESS_PPM_POLICY.h"

#include "_COUNTER_READING.h"
#include "_KWAIT_STATUS_REGISTER.h"
#include "_KTHREAD_COUNTERS.h"
#include "_KTHREAD_PPM_POLICY.h"
#include "_KTHREAD_STATE.h"
#include "_KTHREAD_TAG.h"
#include "_KTHREAD.h"

#include "_EPROCESS.h"

#undef unionvolatile