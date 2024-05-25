#include "vmm.h"
#include <winternl.h>

int main() {
	if (!vmm::ping()) {
		std::println("Failed to ping vmm");
		return 0;
	}

	auto process_id = get_process_id(L"explorer.exe");
	if (!process_id) 
		return 0;

	vmm vmm{ process_id };
    std::println("CR3: {:X}, Base: {:X}", vmm.process_cr3, vmm.process_base);
}