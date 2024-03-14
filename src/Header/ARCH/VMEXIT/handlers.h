#pragma once
#include "../VMCB/vmcb.h"

inline void msr_handler(vcpu* vcpu) {
	vcpu->guest_vmcb.save_state.rip = vcpu->guest_vmcb.control.nrip;


}