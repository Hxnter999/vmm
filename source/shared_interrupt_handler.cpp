#include <commons.h>
#include <vcpu/vcpu.h>
#include <ia_32/rflags_t.h>

struct host_exception_context_t {
	union {
		register_t gpr[16];
		struct {
			register_t rax;
			register_t rcx;
			register_t rdx;
			register_t rbx;
			register_t rsp;
			register_t rbp;
			register_t rsi;
			register_t rdi;
			register_t r8;
			register_t r9;
			register_t r10;
			register_t r11;
			register_t r12;
			register_t r13;
			register_t r14;
			register_t r15;
		};
	} ctx;
	uint64_t interrupt_number;
	uint64_t error_code;
	register_t rip;
	uint64_t cs;
	rflags_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

//h_ctx is passed from asm
extern "C" void shared_interrupt_handler(host_exception_context_t& h_ctx) {

	vcpu_t& vcpu = *reinterpret_cast<vcpu_t*>(_readfsbase_u64()); //vcpu address is passed in fsbase
	if (!vcpu.interrupt_data.handle) { // crash, prob should crash in a way that cant be intercepted but whatever
		__debugbreak();
		__writecr3(0); 
		return;
	}

	vcpu.interrupt_data.error_code = h_ctx.error_code;
	vcpu.interrupt_data.interrupt_number = h_ctx.interrupt_number;
	vcpu.interrupt_data.handle = false;
}