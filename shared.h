#pragma once

typedef unsigned long long uint64_t;

enum class HYPERCALL_CODE : uint64_t
{
	UNLOAD = 0,
	INIT,
	VIRTUALIZE,
	DESTROY,
	PING,
	test,
	test2,
};

enum class HYPERCALL_STATUS : uint64_t
{
	SUCCESS = 0,
	FAILURE
};