#pragma once
#include <commons.h>
#include "../vmcb/vmcb.h"
#include "../cpuid/cpuid_t.h"
#include "handlercommon.h"

enum class HANDLER_STATUS 
{
	INCREMENT_RIP,
	INJECT_GP,
	INJECT_UD,
	INJECT_PF
};

HANDLER_STATUS msr_handler(vcpu_t& vcpu);

HANDLER_STATUS cpuid_handler(vcpu_t& vcpu);

HANDLER_STATUS npf_handler(vcpu_t& vcpu);

//HANDLER_STATUS dtr_save_handler(vcpu_t& vcpu);
//HANDLER_STATUS dtr_load_handler(vcpu_t& vcpu);
//
//HANDLER_STATUS syscall(vcpu_t& vcpu);
//HANDLER_STATUS sysret(vcpu_t& vcpu);

HANDLER_STATUS xsetbv_handler(vcpu_t& vcpu);

//HANDLER_STATUS rdtsc_handler(vcpu_t& vcpu);