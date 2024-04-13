#pragma once

typedef unsigned long long uint64_t;

enum class hypercall_code : uint64_t
{
	UNLOAD = 0,
	PING,
	test
};