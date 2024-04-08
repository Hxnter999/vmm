#pragma once
#include "../ARCH/VMCB/vcpu_t.h"
#include "../ARCH/MSRs/efer.h"

enum class hypercall_code : uint64_t
{
	UNLOAD = 0,
	PING,
	test
};

void hypercall_handler(vcpu_t& vcpu);