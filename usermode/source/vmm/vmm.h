#pragma once
#include <helpers.h>

constexpr std::uint64_t hypercall_key = 'AMDV'; // AMD-V is the best :D

enum class hypercall_code : std::uint64_t
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
			std::uint64_t key : 56;
		};
		std::uint64_t value;
	};

	std::uint64_t r8{}, r9{}, r10{}, r11{};

	hypercall_t(hypercall_code code) : code(code), key(hypercall_key) {}
	inline void clear() { memset(this, 0, sizeof(hypercall_t)); key = hypercall_key; }
};

std::uint64_t __vmmcall(hypercall_t& request);

class vmm {
public:
	std::uint64_t process_cr3{};
	std::uint64_t process_base{};
	
	vmm() = default;

	// Copy
	vmm(const vmm& other) = default;
	vmm& operator=(const vmm& other) = default;

	// Move
	vmm(vmm&& other) noexcept = default;
	vmm& operator=(vmm&& other) noexcept = default;


	explicit vmm(std::uint64_t process_id) {
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

	//static void unload() {
	//	/* this is mainly to support manual mapping since we cant use sc stop but it wont work for usermode
	//	* vmmcall(unload) has to be called while in cpl0
	//	* a suboptimal solution would be a dataptr swap to execute the unload routine then unhook and also free vmm from memory all within the same code
	//	*/
	//	execute_on_each_cpu([](std::uint32_t index) -> bool {
	//		hypercall_t request{ hypercall_code::unload };
	//		__vmmcall(request);
	//		return true;
	//	});
	//}

	static bool ping() { 
		// ping every core to ensure vmm is running properly
		return execute_on_each_cpu([](std::uint32_t index) -> bool {
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
	static T read_physical(std::uint64_t physical_address) {
		/*
		* read_physical_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source physical address
		* - R10 = bytes to read
		*/
		T buffer{};

		hypercall_t request{ hypercall_code::read_physical_memory };
		request.r8 = reinterpret_cast<std::uint64_t>(&buffer);
		request.r9 = physical_address;
		request.r10 = sizeof(T);

		__vmmcall(request);
		return buffer;
	}

	template <typename T>
	static bool read_physical(std::uint64_t physical_address, T& buffer) {
		/*
		* read_physical_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source physical address
		* - R10 = bytes to read
		*/

		hypercall_t request{ hypercall_code::read_physical_memory };
		request.r8 = reinterpret_cast<std::uint64_t>(&buffer);
		request.r9 = physical_address;
		request.r10 = sizeof(T);

		return __vmmcall(request) == sizeof(T);
	}

	static bool read_physical(std::uint64_t physical_address, void* buffer, size_t size) {
		/*
		* read_physical_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source physical address
		* - R10 = bytes to read
		*/

		hypercall_t request{ hypercall_code::read_physical_memory };
		request.r8 = reinterpret_cast<std::uint64_t>(buffer);
		request.r9 = physical_address;
		request.r10 = size;

		return __vmmcall(request) == size;
	}

	template <typename T>
	static bool write_physical(std::uint64_t physical_address, T buffer) {
		/*
		* write_physical_memory()
		* - RAX = bytes written
		* - R8 = destination physical address
		* - R9 = source virtual address
		* - R10 = bytes to write
		*/

		hypercall_t request{ hypercall_code::write_physical_memory };
		request.r8 = physical_address;
		request.r9 = reinterpret_cast<std::uint64_t>(&buffer);
		request.r10 = sizeof(T);

		return __vmmcall(request) == sizeof(T);
	}

	static bool write_physical(std::uint64_t physical_address, void* buffer, size_t size) {
		/*
		* write_physical_memory()
		* - RAX = bytes written
		* - R8 = destination physical address
		* - R9 = source virtual address
		* - R10 = bytes to write
		*/

		hypercall_t request{ hypercall_code::write_physical_memory };
		request.r8 = physical_address;
		request.r9 = reinterpret_cast<std::uint64_t>(buffer);
		request.r10 = size;

		return __vmmcall(request) == size;
	}

	template <typename T>
	T read_virtual(std::uint64_t address) {
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
		request.r8 = reinterpret_cast<std::uint64_t>(&buffer);
		request.r9 = address;
		request.r10 = sizeof(T);
		request.r11 = process_cr3;

		__vmmcall(request);
		return buffer;
	}

	template <typename T>
	bool read_virtual(std::uint64_t address, T& buffer) {
		/*
		* read_virtual_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source virtual address
		* - R10 = bytes to read
		* - R11 = process cr3
		*/

		hypercall_t request{ hypercall_code::read_virtual_memory };
		request.r8 = reinterpret_cast<std::uint64_t>(&buffer);
		request.r9 = address;
		request.r10 = sizeof(T);
		request.r11 = process_cr3;

		return __vmmcall(request) == sizeof(T);
	}

	bool read_virtual(std::uint64_t address, void* buffer, size_t size) {
		/*
		* read_virtual_memory()
		* - RAX = bytes read
		* - R8 = destination virtual address
		* - R9 = source virtual address
		* - R10 = bytes to read
		* - R11 = process cr3
		*/

		hypercall_t request{ hypercall_code::read_virtual_memory };
		request.r8 = reinterpret_cast<std::uint64_t>(buffer);
		request.r9 = address;
		request.r10 = size;
		request.r11 = process_cr3;

		return __vmmcall(request) == size;
	}

	template <typename T>
	bool write_virtual(std::uint64_t address, T buffer) {
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
		request.r9 = reinterpret_cast<std::uint64_t>(&buffer);
		request.r10 = sizeof(T);
		request.r11 = process_cr3;

		return __vmmcall(request) == sizeof(T);
	}

	bool write_virtual(std::uint64_t address, void* buffer, size_t size) {
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
		request.r9 = reinterpret_cast<std::uint64_t>(buffer);
		request.r10 = size;
		request.r11 = process_cr3;

		return __vmmcall(request) == size;
	}
};
