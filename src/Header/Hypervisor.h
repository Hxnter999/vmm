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