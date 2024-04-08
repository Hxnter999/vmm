#include <VMCB/vmcb.h>
#include <Hypervisor.h>

template<typename T>
bool vcpu_t::read_virtual(virtual_address_t va, T& out)
{
	PHYSICAL_ADDRESS phy{};
	if (!Hypervisor::get()->get_phys(guest_vmcb.save_state.cr3, va, phy)) return false;

	out = Hypervisor::get()->read_phys<T>(phy);
	return true;
}

template<typename T>
bool vcpu_t::write_virtual(virtual_address_t va, const T& value)
{
	PHYSICAL_ADDRESS phy{};
	if (!Hypervisor::get()->get_phys(guest_vmcb.save_state.cr3, va, phy)) return false;

	Hypervisor::get()->write_phys(phy, value);
	return true;
}