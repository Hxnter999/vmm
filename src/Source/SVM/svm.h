#pragma once
#include "../../Header/commons.h"

#include "../../Header/ARCH/VMCB/vmcb.h"

#include "../../Header/ARCH/CPUID/Extended Features/fn_identifiers.h"
#include "../../Header/ARCH/CPUID/Extended Features/fn_processor_capacity.h"
#include "../../Header/ARCH/CPUID/Extended Features/fn_svm_features.h"
#include "../../Header/ARCH/CPUID/Standard Features/fn_vendor.h"

#include "../../Header/SVM_STATUS.h"

#include "../../Header/Hypercall/hypercall.h"
#include "../../Header/ARCH/VMEXIT/handlers.h"

#include "../../Header/Util/bitset.h"

extern "C" bool vmexit_handler(vcpu_t* vcpu);