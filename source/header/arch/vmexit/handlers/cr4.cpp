#include <vmexit/handlers.h>

void cr4_handler(vcpu_t& cpu) {
	//const cr3_t& cr3 = vcpu.guest.state.cr3;

	//cr4_t& old_cr4 = vcpu.guest.state.cr4;

	//should check for Decode Assists (support for this feature)
	const size_t gpr_number = cpu.guest.control.exit_info_1.mov_cr.gpr_number;
	const cr4_t new_cr4 = { cpu.ctx.gpr[gpr_number].value };

	if (new_cr4.reserved13 || new_cr4.reserved19 || new_cr4.reserved24) 
		cpu.inject_event(exception_vector::GP, 0);

	if (new_cr4.la57)
		cpu.inject_event(exception_vector::GP, 0);

	//if (new_cr4.pge != old_cr4.pge || !new_cr4.pcide && old_cr4.pcide || new_cr4.smep && !new_cr4.smap)
	cpu.skip_instruction();
}