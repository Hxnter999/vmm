#include <vmm/vmm.h>

auto main() -> void {
	if (!vmm::ping()) {
		std::println("Failed to ping vmm");
		return;
	}

	auto process_id = get_process_id(L"explorer.exe");
	if (!process_id) {
		std::println("Failed to get process id");
		return;
	}

	vmm mem{ process_id };

	std::println("CR3: {:X}, Base: {:X}",
		mem.process_cr3, mem.process_base);
}