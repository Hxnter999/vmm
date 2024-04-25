#pragma once
#include <commons.h>
#include <vcpu/vcpu.h>
#include <svm_status.h>

namespace global {
	inline host_pt_t* shared_host_pt;
	inline vcpu_t* vcpus;
	inline uint32_t vcpu_count;
}

svm_status check_svm_support();

bool virtualize();

void unload_single_vcpu(vcpu_t& vcpu); // for the vmexit handler... temporary
void devirtualize();