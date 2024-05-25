#include <commons.h>
#include <hypercall/hypercall.h>
#include <hypercall/helpers.h>

// hypercalls should pass the request structure into rcx and the rest of the required parameters for the request into r8-r15
void hypercall_handler(vcpu_t& cpu) {
	hypercall_t request {.value = cpu.ctx.rcx.value};

	if (request.key != hypercall_key) {
		cpu.inject_exception(exception_vector::UD, 0);
		return;
	}

	switch (request.code) {

	case hypercall_code::unload:
		cpu.shadow.should_exit = true;
		break;

	case hypercall_code::ping:
		cpu.ctx.rax.value = hypercall_key;
		break;

	case hypercall_code::get_base_address:
		get_base_address(cpu);
		break;
	
	case hypercall_code::get_process_cr3:
		get_process_cr3(cpu);
		break;

	case hypercall_code::get_physical_address:
		get_physical_address(cpu);
		break;

	case hypercall_code::hide_physical_page:
		hide_physical_page(cpu);
		break;

	case hypercall_code::unhide_physical_page:
		unhide_physical_page(cpu);
		break;
	
	case hypercall_code::read_physical_memory:
		read_physical_memory(cpu);
		break;

	case hypercall_code::write_physical_memory:
		write_physical_memory(cpu);
		break;

	case hypercall_code::read_virtual_memory:
		read_virtual_memory(cpu);
		break;

	case hypercall_code::write_virtual_memory:
		write_virtual_memory(cpu);
		break;

	default:
		break;
	}

	cpu.skip_instruction();
}