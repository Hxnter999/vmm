#pragma once
#include <VMEXIT/handlers.h>

enum class hypercall_code : uint64_t
{
	UNLOAD = 0,
	PING,
	test
};

HANDLER_STATUS hypercall_handler(vcpu_t& vcpu);