#pragma once
#include "../vmcb/vmcb.h"
#include "../msrs/hsave_pa.h"
#include "../msrs/efer.h"
#include "../cpuid/cpuid_t.h"

void msr_handler(vcpu_t& vcpu);

void cpuid_handler(vcpu_t& vcpu);

void npf_handler(vcpu_t& vcpu);