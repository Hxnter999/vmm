#pragma once
#include "datatypes.h"
#include <intrin.h>
#pragma warning(disable : 4996)

namespace MSR {
	template<typename T>
	void loadMSR(T& fn);

	template<typename T>
	void storeMSR(T& fn);
};