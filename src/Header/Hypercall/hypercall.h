#pragma once
#include "../ARCH/VMCB/vmcb.h"
#include "../ARCH/MSRs/efer.h"

enum class hypercall_code : uint64_t
{
	UNLOAD = 0,
};

void hypercall_handler(vcpu* vcpu);