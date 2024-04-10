#pragma once
#include <commons.h>
#include "../vmcb/vcpu_t.h"
#include "../msrs/hsave_pa.h"
#include "../msrs/efer.h"
#include "../cpuid/cpuid_t.h"
#include "handlercommon.h"

#define INCREMENT_RIP vcpu.guest_vmcb.save_state.rip = vcpu.guest_vmcb.control.nrip

void msr_handler(vcpu_t& vcpu);

void cpuid_handler(vcpu_t& vcpu);

void npf_handler(vcpu_t& vcpu);

void dtr_save_handler(vcpu_t& vcpu);
void dtr_load_handler(vcpu_t& vcpu);

void syscall(vcpu_t& vcpu);
void sysret(vcpu_t& vcpu);