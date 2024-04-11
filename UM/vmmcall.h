#pragma once
#include "../shared.h"
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

	extern uint64_t _vmmcall(hypercall_code code);

#ifdef __cplusplus
}
#endif
