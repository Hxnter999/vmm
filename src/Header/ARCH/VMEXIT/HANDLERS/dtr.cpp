#include "../handlers.h"

void dtr_save_handler(vcpu_t& vcpu) 
{
	vcpu;
}

void dtr_load_handler(vcpu_t& vcpu)
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
	ModRM_t* modRm = reinterpret_cast<ModRM_t*>(rip + 2);
	uint8_t operand_size = (modRm->mod >> 1) & 0b1;

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