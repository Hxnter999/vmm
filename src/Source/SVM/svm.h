#pragma once
#include <commons.h>

#include <arch/vmcb/vmcb.h>

#include <cpuid/extended-features/fn_identifiers.h>
#include <cpuid/extended-features/fn_processor_capacity.h>
#include <cpuid/extended-features/fn_svm_features.h>
#include <cpuid/standard-features/fn_vendor.h>

#include <svm_status.h>

#include <hypercall/hypercall.h>
#include <arch/vmexit/handlers.h>
		 
#include <util/bitset.h>

extern "C" bool vmexit_handler(vcpu_t* vcpu, uint64_t);