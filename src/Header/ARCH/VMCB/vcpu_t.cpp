#include "vcpu_t.h"
#include <Hypervisor.h>

//template<typename T>
//bool read_virtual(virtual_address_t va, T& out)
//{

//}

//template<typename T>
//bool write_virtual(virtual_address_t va, const T& value)
//{

//}

bool vcpu_t::read_virtual_w(virtual_address_t va, void* out, size_t size)
{
    PHYSICAL_ADDRESS phy{};
    if (!Hypervisor::get()->get_phys(guest_vmcb.save_state.cr3, va, phy)) return false;

    Hypervisor::get()->read_phys(phy, out, size);
    return true;
}

bool vcpu_t::write_virtual_w(virtual_address_t va, void* value, size_t size)
{
   	PHYSICAL_ADDRESS phy{};
   	if (!Hypervisor::get()->get_phys(guest_vmcb.save_state.cr3, va, phy)) return false;

    Hypervisor::get()->write_phys(phy, value, size);
    return true;
}
