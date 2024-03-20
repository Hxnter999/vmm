<<<<<<< Updated upstream
//#pragma once
//#include "ARCH/VMCB/vmcb.h"
//
//class HV
//{
//
//    /**
//     * The Singleton's constructor/destructor should always be private to
//     * prevent direct construction/desctruction calls with the `new`/`delete`
//     * operator.
//     */
//private:
//    static HV* pinstance_;
//    static std::mutex mutex_;
//
//protected:
//    HV() {}
//    ~Singleton() {}
//    std::string value_;
//
//public:
//
//    HV(HV& other) = delete;
//    void operator=(const HV&) = delete;
//
//    static HV* GetInstance();
//};
//
///**
// * Static methods should be defined outside the class.
// */
//
//HV* HV::pinstance_{ nullptr };
//std::mutex HV::mutex_;
//
///**
// * The first time we call GetInstance we will lock the storage location
// *      and then we make sure again that the variable is null and then we
// *      set the value. RU:
// */
//HV* HV::GetInstance()
//{
//    std::lock_guard<std::mutex> lock(mutex_);
//    if (pinstance_ == nullptr)
//    {
//        pinstance_ = new HV();
//    }
//    return pinstance_;
//}
=======
#pragma once
#include "ARCH/VMCB/vmcb.h"

class Hypervisor
{
	static Hypervisor* instance;

protected:
	Hypervisor() {}

public:
	vcpu_t* current_vcpu;
	vcpu_t* vcpus;
	uint32_t vcpu_count;
	MSR::msrpm_t* shared_msrpm;
	uint64_t* npt;

	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	static Hypervisor* Get() 
	{
		//naive because we know when its first called
		if (instance == nullptr)
		{
			instance = static_cast<Hypervisor*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(Hypervisor), 'hv'));
		}
		return instance;
	}

	void _delete() //this should only be called once (in Unload)
	{
		if (instance != nullptr)
		{
			ExFreePoolWithTag(instance, 'hv');
			instance = nullptr;
		}
	}
};

inline Hypervisor* Hypervisor::instance = nullptr;
#define HV Hypervisor::Get()
>>>>>>> Stashed changes
