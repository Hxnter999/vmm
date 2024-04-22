#pragma once
#include <vcpu/vcpu.h>
#include <msrs/efer.h>

enum class hypercall_code : uint64_t
{
	UNLOAD = 0,
	PING,
	test
};

void hypercall_handler(vcpu_t& vcpu);