#include "../handlers.h"

HANDLER_STATUS dtr_save_handler(vcpu_t& vcpu)
{
	segment_register_t& dtr{
		vcpu.guest_vmcb.control.exit_code == VMEXIT_IDTR_READ ? vcpu.guest_vmcb.save_state.idtr : vcpu.guest_vmcb.save_state.gdtr
	};

	if(is_arch_x64(vcpu.guest_vmcb.save_state)) {
		
	}
}

HANDLER_STATUS dtr_load_handler(vcpu_t& vcpu)
{

	segment_register_t& dtr{
		vcpu.guest_vmcb.control.exit_code == VMEXIT_IDTR_READ ? vcpu.guest_vmcb.save_state.idtr : vcpu.guest_vmcb.save_state.gdtr
	};

	register_t regsrc{};


	if (is_arch_x64(vcpu.guest_vmcb.save_state)) 
	{
		//In x64 its fixed at 8+2 byte instruction
		struct src_64_idt_t
		{
			uint32_t limit;
			uint64_t base;
		} *src = reinterpret_cast<src_64_idt_t*>(regsrc.value);
		dtr.limit = src->limit;
		dtr.base = src->base;

		return;
	}

	uint8_t* rip = reinterpret_cast<uint8_t*>(vcpu.guest_rip);

	ZydisDecoder decoder;
	ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32);
	//9 bytes (dunno if this is correct)
	uint8_t buh[9];
	vcpu.read_virtual(rip, buh);
	ZyanUSize offset = 0;
	ZydisDecodedInstruction instruction;
	ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
	ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, buh + offset, sizeof(buh) - offset, &instruction, operands));


	uint8_t operand_size = instruction.operand_width;

	struct src_32_idt_t 
	{
		uint16_t limit;
		uint32_t base;
	} *src = reinterpret_cast<src_32_idt_t*>(regsrc.value);

	if (operand_size) //32 bit
	{
		dtr.limit = src->limit;
		dtr.base = src->base;
	}
	else //16 bit
	{
		dtr.limit = src->limit;
		dtr.base = src->base & 0x00FFFFFF;
	}
}