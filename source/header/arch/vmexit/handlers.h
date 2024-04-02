#pragma once
#include <hypervisor.h>
#include <hypercall/hypercall.h>
#include <cpuid/cpuid_t.h>
#include <msrs/efer.h>
#include <msrs/hsave.h>

void msr_handler(vcpu_t& vcpu);
void cpuid_handler(vcpu_t& vcpu);
void npf_handler(vcpu_t& vcpu);

extern "C" bool vmexit_handler(vcpu_t* const vcpu);