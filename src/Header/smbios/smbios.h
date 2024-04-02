#pragma once
#include <commons.h>

#define SMBIOS_STRING(name)  uint8_t name##_; const char * name

namespace smbios {

	enum TYPE : uint8_t
	{
		TYPE_BIOS_INFO = 0,
		TYPE_SYSTEM_INFO = 1,
		TYPE_BASEBOARD_INFO = 2,
		TYPE_SYSTEM_ENCLOSURE = 3,
		TYPE_PROCESSOR_INFO = 4,
		TYPE_PORT_CONNECTOR = 8,
		TYPE_SYSTEM_SLOT = 9,
		TYPE_OEM_STRINGS = 11,
		TYPE_PHYSICAL_MEMORY_ARRAY = 16,
		TYPE_MEMORY_DEVICE = 17,
		TYPE_MEMORY_ARRAY_MAPPED_ADDRESS = 19,
		TYPE_MEMORY_DEVICE_MAPPED_ADDRESS = 20,
		TYPE_SYSTEM_BOOT_INFO = 32,
		TYPE_MANAGEMENT_DEVICE = 34,
		TYPE_MANAGEMENT_DEVICE_COMPONENT = 35,
		TYPE_MANAGEMENT_DEVICE_THRESHOLD_DATA = 36,
		TYPE_ONBOARD_DEVICES_EXTENDED_INFO = 41,
	};


	struct BiosInfo
	{
		SMBIOS_STRING(Vendor);
		SMBIOS_STRING(BIOSVersion);
		uint16_t BIOSStartingSegment;
		SMBIOS_STRING(BIOSReleaseDate);
		uint8_t BIOSROMSize;
		uint8_t BIOSCharacteristics[8];
		uint8_t ExtensionByte1;
		uint8_t ExtensionByte2;
		uint8_t SystemBIOSMajorRelease;
		uint8_t SystemBIOSMinorRelease;
		uint8_t EmbeddedFirmwareMajorRelease;
		uint8_t EmbeddedFirmwareMinorRelease;
	};

	struct SystemInfo
	{
		// 2.0+
		SMBIOS_STRING(Manufacturer);
		SMBIOS_STRING(ProductName);
		SMBIOS_STRING(Version);
		SMBIOS_STRING(SerialNumber);
		// 2.1+
		uint8_t UUID[16];
		uint8_t WakeupType;
		// 2.4+
		SMBIOS_STRING(SKUNumber);
		SMBIOS_STRING(Family);
	};

	struct BaseboardInfo
	{
		// 2.0+
		SMBIOS_STRING(Manufacturer);
		SMBIOS_STRING(Product);
		SMBIOS_STRING(Version);
		SMBIOS_STRING(SerialNumber);
		SMBIOS_STRING(AssetTag);
		uint8_t FeatureFlags;
		SMBIOS_STRING(LocationInChassis);
		uint16_t ChassisHandle;
		uint8_t BoardType;
		uint8_t NoOfContainedObjectHandles;
		uint16_t* ContainedObjectHandles;
	};

	struct SystemEnclosure
	{
		// 2.0+
		SMBIOS_STRING(Manufacturer);
		uint8_t Type;
		SMBIOS_STRING(Version);
		SMBIOS_STRING(SerialNumber);
		SMBIOS_STRING(AssetTag);
		// 2.1+
		uint8_t BootupState;
		uint8_t PowerSupplyState;
		uint8_t ThermalState;
		uint8_t SecurityStatus;
		// 2.3+
		uint32_t OEMdefined;
		uint8_t Height;
		uint8_t NumberOfPowerCords;
		uint8_t ContainedElementCount;
		uint8_t ContainedElementRecordLength;
		const uint8_t* ContainedElements;
		// 2.7+
		SMBIOS_STRING(SKUNumber);
	};

	struct ProcessorInfo
	{
		// 2.0+
		SMBIOS_STRING(SocketDesignation);
		uint8_t ProcessorType;
		uint8_t ProcessorFamily;
		SMBIOS_STRING(ProcessorManufacturer);
		uint8_t ProcessorID[8];
		SMBIOS_STRING(ProcessorVersion);
		uint8_t Voltage;
		uint16_t ExternalClock;
		uint16_t MaxSpeed;
		uint16_t CurrentSpeed;
		uint8_t Status;
		uint8_t ProcessorUpgrade;
		// 2.1+
		uint16_t L1CacheHandle;
		uint16_t L2CacheHandle;
		uint16_t L3CacheHandle;
		// 2.3+
		SMBIOS_STRING(SerialNumber);
		SMBIOS_STRING(AssetTagNumber);
		SMBIOS_STRING(PartNumber);
		// 2.5+
		uint8_t CoreCount;
		uint8_t CoreEnabled;
		uint8_t ThreadCount;
		uint16_t ProcessorCharacteristics;
		// 2.6+
		uint16_t ProcessorFamily2;
		// 3.0+
		uint16_t CoreCount2;
		uint16_t CoreEnabled2;
		uint16_t ThreadCount2;
	};

	struct PortConnector
	{
		SMBIOS_STRING(InternalReferenceDesignator);
		uint8_t InternalConnectorType;
		SMBIOS_STRING(ExternalReferenceDesignator);
		uint8_t ExternalConnectorType;
		uint8_t PortType;
	};

	struct SystemSlot
	{
		// 2.0+
		SMBIOS_STRING(SlotDesignation);
		uint8_t SlotType;
		uint8_t SlotDataBusWidth;
		uint8_t CurrentUsage;
		uint8_t SlotLength;
		uint16_t SlotID;
		uint8_t SlotCharacteristics1;
		// 2.1+
		uint8_t SlotCharacteristics2;
		// 2.6+
		uint16_t SegmentGroupNumber;
		uint8_t BusNumber;
		uint8_t DeviceOrFunctionNumber;
	};

	struct OemStrings
	{
		// 2.0+
		uint8_t Count;
		const char* Values;
	};

	struct PhysicalMemoryArray
	{
		// 2.1+
		uint8_t Location;
		uint8_t Use;
		uint8_t ErrorCorrection;
		uint32_t MaximumCapacity;
		uint16_t ErrorInformationHandle;
		uint16_t NumberDevices;
		// 2.7+
		uint64_t ExtendedMaximumCapacity;
	};

	struct MemoryDevice
	{
		// 2.1+
		uint16_t PhysicalArrayHandle;
		uint16_t ErrorInformationHandle;
		uint16_t TotalWidth;
		uint16_t DataWidth;
		uint16_t Size;
		uint8_t FormFactor;
		uint8_t DeviceSet;
		SMBIOS_STRING(DeviceLocator);
		SMBIOS_STRING(BankLocator);
		uint8_t MemoryType;
		uint16_t TypeDetail;
		// 2.3+
		uint16_t Speed;
		SMBIOS_STRING(Manufacturer);
		SMBIOS_STRING(SerialNumber);
		SMBIOS_STRING(AssetTagNumber);
		SMBIOS_STRING(PartNumber);
		// 2.6+
		uint8_t Attributes;
		// 2.7+
		uint32_t ExtendedSize;
		uint16_t ConfiguredClockSpeed;
		// 2.8+
		uint16_t MinimumVoltage;
		uint16_t MaximumVoltage;
		uint16_t ConfiguredVoltage;
	};

	struct MemoryArrayMappedAddress
	{
		// 2.1+
		uint32_t StartingAddress;
		uint32_t EndingAddress;
		uint16_t MemoryArrayHandle;
		uint8_t PartitionWidth;
		// 2.7+
		uint64_t ExtendedStartingAddress;
		uint64_t ExtendedEndingAddress;
	};

	struct MemoryDeviceMappedAddress
	{
		// 2.1+
		uint32_t StartingAddress;
		uint32_t EndingAddress;
		uint16_t MemoryDeviceHandle;
		uint16_t MemoryArrayMappedAddressHandle;
		uint8_t PartitionRowPosition;
		uint8_t InterleavePosition;
		uint8_t InterleavedDataDepth;
		// 2.7+
		uint64_t ExtendedStartingAddress;
		uint64_t ExtendedEndingAddress;
	};

	struct SystemBootInfo
	{
		// 2.0+
		uint8_t Reserved[6];
		const uint8_t* BootStatus;
	};

	struct ManagementDevice
	{
		// 2.0+
		SMBIOS_STRING(Description);
		uint8_t Type;
		uint32_t Address;
		uint8_t AddressType;
	};

	struct ManagementDeviceComponent
	{
		// 2.0+
		SMBIOS_STRING(Description);
		uint16_t ManagementDeviceHandle;
		uint16_t ComponentHandle;
		uint16_t ThresholdHandle;
	};

	struct ManagementDeviceThresholdData
	{
		// 2.0+
		uint16_t LowerThresholdNonCritical;
		uint16_t UpperThresholdNonCritical;
		uint16_t LowerThresholdCritical;
		uint16_t UpperThresholdCritical;
		uint16_t LowerThresholdNonRecoverable;
		uint16_t UpperThresholdNonRecoverable;
	};

	struct OnboardDevicesExtendedInfo
	{
		// 2.0+
		SMBIOS_STRING(ReferenceDesignation);
		uint8_t DeviceType;
		uint8_t DeviceTypeInstance;
		uint16_t SegmentGroupNumber;
		uint8_t BusNumber;
		uint8_t DeviceOrFunctionNumber;
	};

	struct Entry
	{
		uint8_t type;
		uint8_t length;
		uint16_t handle;
		union
		{
			ProcessorInfo processor;
			BaseboardInfo baseboard;
			SystemInfo sysinfo;
			BiosInfo bios;
			SystemEnclosure sysenclosure;
			PhysicalMemoryArray physmem;
			MemoryDevice memory;
			SystemSlot sysslot;
			OemStrings oemstrings;
			PortConnector portconn;
			MemoryArrayMappedAddress mamaddr;
			MemoryDeviceMappedAddress mdmaddr;
			SystemBootInfo bootinfo;
			ManagementDevice mdev;
			ManagementDeviceComponent mdcom;
			ManagementDeviceThresholdData mdtdata;
			OnboardDevicesExtendedInfo odeinfo;
		} data;
		const uint8_t* rawdata;
		const char* strings;

	};

	enum SpecVersion
	{
		SMBIOS_2_0 = 0x0200,
		SMBIOS_2_1 = 0x0201,
		SMBIOS_2_2 = 0x0202,
		SMBIOS_2_3 = 0x0203,
		SMBIOS_2_4 = 0x0204,
		SMBIOS_2_5 = 0x0205,
		SMBIOS_2_6 = 0x0206,
		SMBIOS_2_7 = 0x0207,
		SMBIOS_2_8 = 0x0208,
		SMBIOS_3_0 = 0x0300
	};

	class Parser
	{
	public:
		Parser(const uint8_t* data, size_t size, int version = 0);
		void reset();
		const Entry* next();
		int version() const;
		bool valid() const;

	private:
		const uint8_t* data_;
		size_t size_;
		Entry entry_;
		const uint8_t* ptr_;
		const uint8_t* start_;
		int version_;

		const Entry* parseEntry();
		const char* getString(int index) const;
	};

} // namespace smbios