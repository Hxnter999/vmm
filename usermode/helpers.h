#pragma once
#include <iostream>
#include <vector>
#include <print>
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

using per_cpu_callback_t = bool(*)(uint32_t);
inline bool execute_on_each_cpu(per_cpu_callback_t callback) {
	SYSTEM_INFO info = {};
	GetSystemInfo(&info);

	for (uint32_t i = 0; i < info.dwNumberOfProcessors; ++i) {
		auto prev_affinity = SetThreadAffinityMask(GetCurrentThread(), 1ull << i);
		auto result = callback(i);
		SetThreadAffinityMask(GetCurrentThread(), prev_affinity);
		if (!result)
			return false;
	}
	return true;
}

uint64_t get_process_id(std::wstring_view process_name) {
	std::vector<uint8_t> buffer{};
	uint32_t size{};

	NTSTATUS status{};
	while ((status = NtQuerySystemInformation(SystemProcessInformation, buffer.data(), static_cast<ULONG>(buffer.size()), reinterpret_cast<ULONG*>(&size))) == 0xC0000004) { // STATUS_INFO_LENGTH_MISMATCH
		buffer.resize(size);
	}

	if (!NT_SUCCESS(status)) {
		std::println("Failed to query system information");
		return 0;
	}

	auto process = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(buffer.data() + reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(buffer.data())->NextEntryOffset);
	while (process->NextEntryOffset) {
		if (process->ImageName.Buffer && 
			std::wstring_view(process->ImageName.Buffer).find(process_name) != std::wstring_view::npos) {
			return reinterpret_cast<uint64_t>(process->UniqueProcessId);
		}
		process = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(reinterpret_cast<uint8_t*>(process) + process->NextEntryOffset);
	}
	return 0;
}