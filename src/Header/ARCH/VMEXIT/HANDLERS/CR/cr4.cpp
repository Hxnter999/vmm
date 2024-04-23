#include <arch/vmexit/handlers.h>

HANDLER_STATUS cr4_read_handler(vcpu_t& vcpu) {
	ZydisDecoder decoder{};

	if (is_arch_x64(vcpu.guest_vmcb.save_state)) {
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
	}
	else {
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_STACK_WIDTH_32);
	}

	uint8_t opcodes[10]{};
	if (!vcpu.read_rip(&opcodes)) {
		//guh!
	}

	ZydisDecodedInstruction instruction;
	ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
	if (ZYAN_FAILED(ZydisDecoderDecodeFull(&decoder, opcodes, sizeof(opcodes), &instruction, operands))) {
		//guh!
	}

	ZydisRegister& z = operands[0].reg.value; // register that cr4 is being moved into
}

HANDLER_STATUS cr4_write_handler(vcpu_t& vcpu) {

}