#include <arch/vmexit/handlers.h>


typedef struct  
{
	union {
		struct {
			uint64_t x87 : 1;
			uint64_t SSE : 1;
			uint64_t YMM : 1;
			uint64_t reserved1 : 6;
			uint64_t MPK : 1;
			uint64_t reserved2 : 1;
			uint64_t CET_U : 1;
			uint64_t CET_S : 1;
			uint64_t reserved3 : 49;
			uint64_t LWP : 1;
			uint64_t X : 1;
		};

		ULARGE_INTEGER value;
	};
} XFEATURE_ENABLED_MASK, xcr0_t;

constexpr uint32_t XFEATURE_ENABLED_MASK_ID = 0;
HANDLER_STATUS xsetbv_handler(vcpu_t& vcpu)
{
	//might need to check if cpl is not 0

	uint32_t xcr{ vcpu.guest_stack_frame.rcx.dword1 };

	//Currently, only the XFEATURE_ENABLED_MASK register (XCR0) is supported.
	//If an invalid XCR is specified in ECX.
	if (xcr != XFEATURE_ENABLED_MASK_ID)
		return HANDLER_STATUS::INJECT_GP;

	xcr0_t value{ .value { .u {
			.LowPart = static_cast<uint32_t>(vcpu.guest_stack_frame.rax),
			.HighPart = static_cast<uint32_t>(vcpu.guest_stack_frame.rdx)
	}}};
	
	//If the value in EDX:EAX sets bits that are reserved in the XCR specified by ECX.
	if (value.reserved1 + value.reserved2 + value.reserved3 != 0)
		return HANDLER_STATUS::INJECT_GP;

	//If an attempt is made to clear bit 0 of XCR0.
	if (value.x87 != 1)
		return HANDLER_STATUS::INJECT_GP;

	//If an attempt is made to set XCR0[2:1] to 10b.
	if (value.SSE == 0 && value.YMM == 1)
		return HANDLER_STATUS::INJECT_GP;

	//that XCR0[63:3] is clear
	if (value.value.QuadPart & ~0b111)
		return HANDLER_STATUS::INJECT_GP;

	_xsetbv(xcr, value.value.QuadPart);
	return HANDLER_STATUS::INCREMENT_RIP;
}