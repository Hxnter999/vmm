//#include "sce.h"
//#include <msrs/lstar.h>
//#include <msrs/cstar.h>
//#include <msrs/star.h>
//#include <msrs/sfmask.h>
//#include <msrs/u_cet.h>
//#include <msrs/s_cet.h>
//#include <msrs/efer.h>
//#include <msrs/pl3_ssp.h>
//#include <cr/control_registers.h>
//
//inline void syscalllong(vcpu_t& vcpu);
//void syscalllegacy(vcpu_t& vcpu);
//
////define SSTK_ENABLED = (CR4.CET) && (CR0.PE) && (!EFLAGS.VM)
//bool shadow_stack_enabled(uint8_t cpl, vcpu_t& vcpu)
//{
//	MSR::U_CET U_CET{}; U_CET.load(); //Most likely shared (only separated in the sigma svm) 
//	const bool SSTK_ENABLED = 
//		(vcpu.guest_vmcb.save_state.cr4.cet) && 
//		(vcpu.guest_vmcb.save_state.cr0.pe) && 
//		(!vcpu.guest_vmcb.save_state.rflags.VM);
//	
//	return SSTK_ENABLED && cpl == 3 && U_CET.sh_stk_en || cpl < 3 && vcpu.guest_vmcb.save_state.s_cet.sh_stk_en;
//}
//
//void syscall(vcpu_t& vcpu)
//{
//	//This is what will throw and trigger this
//	//IF (MSR_EFER.SCE == 0) // Check if syscall/sysret are enabled.
//	//EXCEPTION[#UD]
//
//	if (vcpu.guest_vmcb.save_state.efer.lma) //long mode 
//	{
//		syscalllong(vcpu);
//	}
//	else 
//	{
//		syscalllegacy(vcpu);
//	}
//}
//
////64BIT_MODE = ((EFER[LMA]==1) && (CS_desc.attr[L] == 1) && (CS_desc.attr[D] == 0))
////COMPATIBILITY_MODE = (EFER[LMA] == 1) && (CS_desc.attr[L] == 0)
//void syscalllong(vcpu_t& vcpu)
//{
//	auto& save_state = vcpu.guest_vmcb.save_state;
//
//	//save nRIP and RFLAGES
//	vcpu.guest_stack_frame.rcx = vcpu.guest_vmcb.control.nrip;
//	vcpu.guest_stack_frame.r11 = save_state.rflags.value;
//
//	//vcpu.guest_vmcb.save_state.cs.attributes.
//	bool is64bit = save_state.efer.lma && save_state.cs.attributes.longmode && !save_state.cs.attributes.default_bit; //could be wrong didnt check
//	uint64_t temp_RIP{};
//	if (is64bit)
//	{
//		temp_RIP = save_state.lstar.syscall_entry_point;
//	}
//	else
//	{
//		temp_RIP = save_state.cstar.syscall_entry_point;
//	}
//
//	auto& cs = save_state.cs;
//	cs.selector.value = save_state.star.syscall_cs_ss & 0xFFFC;
//	cs.attributes.dpl = 0; //= CS.attr = 64-bit code,dpl0 // Always switch to 64-bit mode in long mode.
//	cs.base = 0;
//	cs.limit = 0xFFFFFFFF;
//
//	auto& ss = save_state.ss;
//	ss.selector.value = save_state.star.syscall_cs_ss + 8;
//	ss.attributes.dpl = 0; //= SS.attr = 64-bit data,dpl0 // Always switch to 64-bit mode in long mode.
//	ss.base = 0;
//	ss.limit = 0xFFFFFFFF;
//
//	save_state.rflags.value &= ~static_cast<uint64_t>(save_state.sfmask.syscall_flag_mask);
//	save_state.rflags.RF = 0;
//
//	//CPL shit
//	uint8_t& cpl = save_state.cpl;
//	if (shadow_stack_enabled(cpl, vcpu)) 
//	{
//		MSR::PL3_SSP pl3_ssp{}; pl3_ssp.load();
//		pl3_ssp.ssp = save_state.ssp;
//		pl3_ssp.store();
//	}
//
//	cpl = 0;
//
//	if (shadow_stack_enabled(0, vcpu))
//		save_state.ssp = 0;
//
//	save_state.rip = temp_RIP;
//}
//
//void syscalllegacy(vcpu_t& vcpu) 
//{
//	auto& save_state = vcpu.guest_vmcb.save_state;
//
//	vcpu.guest_stack_frame.rcx = vcpu.guest_vmcb.control.nrip;
//
//	uint64_t temp_RIP{};
//
//	temp_RIP = save_state.star.syscall_target_eip;
//
//	segment_register_t& cs = save_state.cs;
//	cs.selector.value = save_state.star.syscall_cs_ss & 0xFFFC;
//	cs.attributes.dpl = 0; //= 32-bit code,dpl0 // Always switch to 32-bit mode in legacy mode.
//	cs.base = 0;
//	cs.limit = 0xFFFFFFFF;
//
//	segment_register_t& ss = save_state.ss;
//	ss.selector.value = save_state.star.syscall_cs_ss + 8;
//	ss.attributes.dpl = 0; //= 32-bit code,dpl0 // Always switch to 32-bit mode in legacy mode. 
//	cs.base = 0;
//	cs.limit = 0xFFFFFFFF;
//
//	save_state.rflags.VM = 0;
//	save_state.rflags.IF = 0;
//	save_state.rflags.RF = 0;
//
//	save_state.cpl = 0;
//
//	save_state.rip = temp_RIP;
//}
//
//
//void sysret64(vcpu_t& vcpu);
//void sysretnon64(vcpu_t& vcpu);
//
//void sysret(vcpu_t& vcpu)
//{
//	auto& save_state = vcpu.guest_vmcb.save_state;
//
//	cr0_t cr0 = { __readcr0() };
//	bool PROTECTED_MODE = cr0.pe && !save_state.rflags.VM;
//
//	if (!PROTECTED_MODE || save_state.cpl) {}//inject exception in this case #GP(0)
//
//	bool is64bit = save_state.efer.lma && save_state.cs.attributes.longmode && !save_state.cs.attributes.default_bit; //this could be wrong
//	if (is64bit)
//		sysret64(vcpu);
//	else
//		sysretnon64(vcpu);
//}
//
//void sysret64(vcpu_t& vcpu) 
//{
//	auto& save_state = vcpu.guest_vmcb.save_state;
//	// OPERAND_SIZE depends on processor mode, the current code segment descriptor
//	// default operand size [D], presence of the operand size override prefix (66h)
//	// and, in 64-bit mode, the REX prefix.
//
//	// Bit 22 of byte + 4. In code - segment descriptors, the
//	// D bit selects the default operand size and address sizes.In legacy mode, when D = 0 the default operand
//	// size and address size is 16 bits and when D = 1 the default operand size and address size is 32 bits.
//	// Instruction prefixes can be used to override the operand size or address size, or both.
//
//	const MSR::STAR star = save_state.star;
//	segment_register_t& cs = save_state.cs;
//	uint8_t OPERAND_SIZE = 64;//8, 16, 32, 64;
//	if (OPERAND_SIZE == 64) 
//	{
//		cs.selector.value = (star.sysret_cc_ss + 16) | 3;
//		cs.attributes.dpl = 3; // = 64-bit code,dpl3
//
//		save_state.rip = vcpu.guest_stack_frame.rcx;
//	}
//	else 
//	{
//		cs.selector.value = star.sysret_cc_ss | 3;
//		cs.attributes.dpl = 3; // = 32-bit code,dpl3
//
//		save_state.rip = vcpu.guest_stack_frame.rcx.low;
//	}
//	cs.base = 0;
//	cs.limit = 0xFFFFFFFF;
//
//	segment_register_t& ss = save_state.ss;
//	ss.selector.value = star.sysret_cc_ss + 8;
//
//	save_state.rflags.value = vcpu.guest_stack_frame.r11;
//	save_state.cpl = 3;
//
//	if (shadow_stack_enabled(3, vcpu)) {
//		MSR::PL3_SSP pl3_ssp{}; pl3_ssp.load();
//		save_state.ssp = pl3_ssp.bits;
//	}
//}
//
//void sysretnon64(vcpu_t& vcpu) 
//{
//	auto& save_state = vcpu.guest_vmcb.save_state;
//	const MSR::STAR star = save_state.star;
//	segment_register_t& cs = save_state.cs;
//
//	cs.selector.value = star.sysret_cc_ss | 3;
//	cs.base = 0;
//	cs.limit = 0xFFFFFFFF;
//	cs.attributes.dpl = 3; // 32-bit code,dpl3
//
//	segment_register_t& ss = save_state.ss;
//	ss.selector.value = star.sysret_cc_ss + 8;
//
//	save_state.rflags.interrupt_flag = 1;
//	save_state.cpl = 3;
//
//	if (shadow_stack_enabled(3, vcpu)) 
//	{
//		MSR::PL3_SSP pl3_ssp{}; pl3_ssp.load();
//		save_state.ssp = pl3_ssp.bits;
//	}
//
//	save_state.rip = vcpu.guest_stack_frame.rcx.low;
//}