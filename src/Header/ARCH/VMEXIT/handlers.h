#pragma once
#include "../VMCB/vmcb.h"
#include "../MSRs/hsave_pa.h"
#include "../MSRs/efer.h"
#include "../CPUID/cpuid_t.h"

void msr_handler(vcpu_t& vcpu);

void cpuid_handler(vcpu_t& vcpu);