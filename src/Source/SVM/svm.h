#pragma once
#include "../../Header/commons.h"

#include "../../Header/ARCH/VMCB/vmcb.h"

#include "../../Header/ARCH/MSRs/vm_cr.h"
#include "../../Header/ARCH/VMCB/vmcb.h"
#include "../../Header/ARCH/MSRs/pat.h"
#include "../../Header/ARCH/MSRs/hsave_pa.h"
#include "../../Header/ARCH/MSRs/efer.h"

#include "../../Header/ARCH/CPUID/Extended Features/fn_identifiers.h"
#include "../../Header/ARCH/CPUID/Extended Features/fn_processor_capacity.h"
#include "../../Header/ARCH/CPUID/Extended Features/fn_svm_features.h"
#include "../../Header/ARCH/CPUID/Standard Features/fn_vendor.h"

#include "../../Header/SVM_STATUS.h"

#include "../../Header/Hypercall/hypercall.h"
#include "../../Header/ARCH/VMEXIT/handlers.h"

#include "../../Header/Util/bitset.h"

extern "C" bool vmexit_handler(vcpu_t* vcpu);
bool setup_msrpm();
void setup_vmcb(vcpu_t* vcpu, CONTEXT* ctx);
SVM_STATUS initialize();
bool virtualize(vcpu_t* vcpu);
void devirtualize();