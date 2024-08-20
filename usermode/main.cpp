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
	
	using namespace std::chrono_literals;
	auto total{ 0ns };
	std::array<std::uint8_t, 0x1000> buffer{};
	
	for (int i = 0; i < 0x1000; i++) {
		auto start = std::chrono::high_resolution_clock::now();
		mem.read_virtual(mem.process_base, buffer.data(), buffer.size());
		auto end = std::chrono::high_resolution_clock::now();

		total += end - start;
	}

	std::println("Total: {}  |  {}  |  {}",
		total,
		std::chrono::duration_cast<std::chrono::microseconds>(total),
		std::chrono::duration_cast<std::chrono::milliseconds>(total)); 

	// How many times can we read a page in a second/minute... 
	auto average{ total / 0x1000 };
	std::println("Average: {}  |  Per millisecond: {}  |  Per second: {}  |  Per minute: {}",
		average,
		std::chrono::duration_cast<std::chrono::nanoseconds>(1ms) / average,    // 1'000'000ns / average
		std::chrono::duration_cast<std::chrono::nanoseconds>(1s) / average,		// 1'000'000'000ns / average
		std::chrono::duration_cast<std::chrono::nanoseconds>(1min) / average);  // 60'000'000'000ns / average
}
