#pragma once
struct register_t {
	union {
		uint64_t value;

		union {
			struct {
				uint32_t low;
				uint32_t high;
			};
			struct {
				uint64_t qword;
			};
			struct {
				uint32_t dword1;
				uint32_t dword2;
			};
			struct {
				uint16_t word1;
				uint16_t word2;
				uint16_t word3;
				uint16_t word4;
			};
			struct {
				uint8_t byte1;
				uint8_t byte2;
				uint8_t byte3;
				uint8_t byte4;
				uint8_t byte5;
				uint8_t byte6;
				uint8_t byte7;
				uint8_t byte8;
			};
		};
	};

	inline bool operator==(uint64_t val) {
		return value == val;
	}

	inline bool operator!=(uint64_t val) {
		return value != val;
	}

	inline register_t& operator=(uint64_t val) {
		value = val;
		return *this;
	}

	operator uint64_t()
	{
		return value;
	}
};