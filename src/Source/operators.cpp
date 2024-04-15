//#include <commons.h>
//
//void* operator new(size_t size) {
//    void* ptr = ExAllocatePoolWithTag(NonPagedPool, size, 'sgmA');
//    if (!ptr) {
//        //Uh oh!
//        print("new fail");
//        return nullptr;
//    }
//    return ptr;
//}
//
//void* operator new[](size_t size) {
//    void* ptr = ExAllocatePoolWithTag(NonPagedPool, size, 'sgmA');
//    if (!ptr) {
//        //Uh oh!
//        print("new[] fail");
//        return nullptr;
//    }
//    return ptr;
//}
//
//void operator delete(void* ptr) noexcept {
//    ExFreePoolWithTag(ptr, 'sgma');
//}
//
//void operator delete(void* ptr, size_t size) noexcept {
//    UNREFERENCED_PARAMETER(size);
//    ExFreePoolWithTag(ptr, 'sgma');
//}
//
//void operator delete[](void* ptr) noexcept {
//    ExFreePoolWithTag(ptr, 'sgma');
//}