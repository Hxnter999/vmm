#pragma once
#include "../../Header/commons.h"
#include "../../Header/ARCH/VMCB/vmcb.h"

//AMD Vol3 
void syscall(vcpu_t& vcpu);

void sysret(vcpu_t& vcpu);