#pragma once
#include "../shared.h"
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

	extern hypercall_status _vmmcall(hypercall_code code, uint64_t arg2 = 0, uint64_t arg3 = 0, uint64_t arg4 = 0);

#ifdef __cplusplus
}
#endif
