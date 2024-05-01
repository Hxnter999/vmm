#pragma once
#include <hypercall/hypercall.h>

void msr_handler(vcpu_t& vcpu);
void cpuid_handler(vcpu_t& vcpu);
void npf_handler(vcpu_t& vcpu);
void svm_handler(vcpu_t& vcpu); // to pass checks with SEH and properly handle instruction intercepts that require svme=1 which is hidden from the guest.

extern "C" bool vmexit_handler(vcpu_t& vcpu);