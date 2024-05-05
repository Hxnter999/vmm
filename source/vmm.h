#pragma once
#include <commons.h>
#include <vcpu/vcpu.h>
#include <svm_status.h>
#include <ntdef/def.h>
#include <hypercall/hypercall.h>

extern "C" uint64_t __vmmcall(hypercall_t request);

namespace global {
	inline host_pt_t* shared_host_pt;
	inline _EPROCESS* system_process;
	inline cr3_t system_cr3; // pointless but cleaner
	inline vcpu_t* vcpus;
	inline uint32_t vcpu_count;
}

svm_status check_svm_support();

bool virtualize();

void unload_single_vcpu(vcpu_t& cpu); // for the vmexit handler... temporary
void devirtualize();