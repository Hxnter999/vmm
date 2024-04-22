#include <arch/vmexit/handlers.h>

//the waitintercept happens before the TLB is flushed

HANDLER_STATUS cr3_read_handler(vcpu_t& vcpu) {
	//get what reg it is being moved from ect ect
}

HANDLER_STATUS cr3_write_handler(vcpu_t& vcpu) {
	
}