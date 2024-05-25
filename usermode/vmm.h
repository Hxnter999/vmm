#pragma once
#include "helpers.h"
#include <exception>

constexpr uint64_t hypercall_key = 'AMDV'; // AMD-V is the best :D

enum class hypercall_code : uint64_t
{
	unload,
	ping,
	get_vmm_base,
	get_process_cr3,
	get_process_base,
	get_physical_address,
	hide_physical_page,
	unhide_physical_page,
	read_physical_memory,
	write_physical_memory,
	read_virtual_memory,
	write_virtual_memory,
};

struct hypercall_t {
	union {
		struct {
			hypercall_code code : 8;
			uint64_t key : 56;
		};
		uint64_t value;
	};

	uint64_t r8, r9, r10, r11;

	hypercall_t(hypercall_code code) : code(code), key(hypercall_key), r8(0), r9(0), r10(0), r11(0) {};
	inline void clear() { memset(this, 0, sizeof(hypercall_t)); key = hypercall_key; }
};

uint64_t __vmmcall(hypercall_t& request);

class vmm {
public:
	uint64_t process_cr3{};
	uint64_t process_base{};

	vmm(uint64_t process_id) {
		hypercall_t request{ hypercall_code::get_process_cr3 };
		request.r8 = process_id;
		if (!(process_cr3 = __vmmcall(request))) {
			std::println("Failed to get process cr3");
			return;
		}

		request.clear();
		request.code = hypercall_code::get_process_base;
		request.r8 = process_id;
		if (!(process_base = __vmmcall(request))) {
			std::println("Failed to get process base");
			return;
		}
	}

	// we dont want to call unload/ping on construction and destruction since we intend to support multiple usermode clients
	static void unload() {
		// currently broken anyway, will fix later
		execute_on_each_cpu([](uint32_t index) -> bool {
			hypercall_t request{ hypercall_code::unload };
			__vmmcall(request);
			return true;
		});
	}

	static bool ping() {
		// ping every core to ensure vmm is running properly
		return execute_on_each_cpu([](uint32_t index) -> bool {
			hypercall_t request{ hypercall_code::ping };
			bool result = false;
			__try {
				result = __vmmcall(request) == hypercall_key;
			}
			__except (1) { // incase of improper virtualization, catch #UD
				result = false;
				std::println("[{}] Improper virtualization", index);
			}
			return result;
		});
	}


	template <typename T>
	static T read_physical(uint64_t physical_address) {
		/*
		* read_physical_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source physical address
		* - R10 = bytes to read
		*/
		T buffer{};

		hypercall_t request{ hypercall_code::read_physical_memory };
		request.r8 = reinterpret_cast<uint64_t>(&buffer);
		request.r9 = physical_address;
		request.r10 = sizeof(T);

		if (__vmmcall(request) != sizeof(T)) {
			throw std::exception("Failed to read physical memory");
		}
		return buffer;
	}

	template <typename T>
	static bool read_physical(uint64_t physical_address, T& buffer) {
		/*
		* read_physical_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source physical address
		* - R10 = bytes to read
		*/

		hypercall_t request{ hypercall_code::read_physical_memory };
		request.r8 = reinterpret_cast<uint64_t>(&buffer);
		request.r9 = physical_address;
		request.r10 = sizeof(T);

		return __vmmcall(request) == sizeof(T);
	}

	template <typename T>
	static bool write_physical(uint64_t physical_address, T buffer) {
		/*
		* write_physical_memory()
		* - RAX = bytes written
		* - R8 = destination physical address
		* - R9 = source virtual address
		* - R10 = bytes to write
		*/

		hypercall_t request{ hypercall_code::write_physical_memory };
		request.r8 = physical_address;
		request.r9 = reinterpret_cast<uint64_t>(&buffer);
		request.r10 = sizeof(T);

		return __vmmcall(request) == sizeof(T);
	}

	static bool write_physical_raw(uint64_t physical_address, void* buffer, size_t size) {
		/*
		* write_physical_memory()
		* - RAX = bytes written
		* - R8 = destination physical address
		* - R9 = source virtual address
		* - R10 = bytes to write
		*/

		hypercall_t request{ hypercall_code::write_physical_memory };
		request.r8 = physical_address;
		request.r9 = reinterpret_cast<uint64_t>(buffer);
		request.r10 = size;

		return __vmmcall(request) == size;
	}

	template <typename T>
	T read_virtual(uint64_t address) {
		/*
		* read_virtual_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source virtual address
		* - R10 = bytes to read
		* - R11 = process cr3
		*/
		T buffer{};

		hypercall_t request{ hypercall_code::read_virtual_memory };
		request.r8 = reinterpret_cast<uint64_t>(&buffer);
		request.r9 = address;
		request.r10 = sizeof(T);
		request.r11 = process_cr3;

		if (__vmmcall(request) != sizeof(T)) {
			throw std::exception("Failed to read virtual memory");
		}
		return buffer;
	}

	template <typename T>
	bool read_virtual(uint64_t address, T& buffer) {
		/*
		* read_virtual_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source virtual address
		* - R10 = bytes to read
		* - R11 = process cr3
		*/

		hypercall_t request{ hypercall_code::read_virtual_memory };
		request.r8 = reinterpret_cast<uint64_t>(&buffer);
		request.r9 = address;
		request.r10 = sizeof(T);
		request.r11 = process_cr3;

		return __vmmcall(request) == sizeof(T);
	}


	template <typename T>
	bool write_virtual(uint64_t address, T buffer) {
		/*
		* write_virtual_memory()
		* - RAX = bytes written
		* - R8 = destination virtual address
		* - R9 = source virtual address
		* - R10 = bytes to write
		* - R11 = process cr3
		*/

		hypercall_t request{ hypercall_code::write_virtual_memory };
		request.r8 = address;
		request.r9 = reinterpret_cast<uint64_t>(&buffer);
		request.r10 = sizeof(T);
		request.r11 = process_cr3;

		return __vmmcall(request) == sizeof(T);
	}

	bool write_virtual_raw(uint64_t address, void* buffer, size_t size) {
		/*
		* write_virtual_memory()
		* - RAX = bytes written
		* - R8 = destination virtual address
		* - R9 = source virtual address
		* - R10 = bytes to write
		* - R11 = process cr3
		*/

		hypercall_t request{ hypercall_code::write_virtual_memory };
		request.r8 = address;
		request.r9 = reinterpret_cast<uint64_t>(buffer);
		request.r10 = size;
		request.r11 = process_cr3;

		return __vmmcall(request) == size;
	}
};