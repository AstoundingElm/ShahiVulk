#include "/home/peter/Desktop/vulkan/1.3.211.0/x86_64/include/vulkan/vulkan.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#define MAX_DEVICES 2
VkInstance instance;
#define TUT1_MAX_QUEUE_FAMILY 10

typedef struct 
{
	VkPhysicalDevice physical_device;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memories;

	VkQueueFamilyProperties queue_families[TUT1_MAX_QUEUE_FAMILY];
	uint32_t queue_family_count;
	bool queue_families_incomplete;
}tut1_physical_device;

 tut1_physical_device devs[MAX_DEVICES];

void createInstance(){



VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Vulkan Tutorial",
		.applicationVersion = 0x010000,
		.pEngineName = "Vulkan Tutorial",
		.engineVersion = 0x010000,
		.apiVersion = VK_API_VERSION_1_0,
	};

VkInstanceCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
	};

 vkCreateInstance(&info, NULL, &instance);

};


void tut1_enumerate_devices(VkInstance vk, tut1_physical_device *devs, uint32_t *count)
{
	VkPhysicalDevice phy_devs[*count];
	//tut1_error retval = TUT1_ERROR_NONE;
	VkResult res;

	vkEnumeratePhysicalDevices(vk, count, phy_devs);


	for (uint32_t i = 0; i < *count; ++i)
	{
		devs[i].physical_device = phy_devs[i];

		vkGetPhysicalDeviceProperties(devs[i].physical_device, &devs[i].properties);
		vkGetPhysicalDeviceFeatures(devs[i].physical_device, &devs[i].features);
		vkGetPhysicalDeviceMemoryProperties(devs[i].physical_device, &devs[i].memories);

		uint32_t qfc = 0;
		devs[i].queue_family_count = TUT1_MAX_QUEUE_FAMILY;
		vkGetPhysicalDeviceQueueFamilyProperties(devs[i].physical_device, &qfc, NULL);
		vkGetPhysicalDeviceQueueFamilyProperties(devs[i].physical_device, &devs[i].queue_family_count, devs[i].queue_families);

		devs[i].queue_families_incomplete = devs[i].queue_family_count < qfc;
	}


}

const char *tut1_VkPhysicalDeviceType_string(VkPhysicalDeviceType type)
{
	switch (type)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		return "Neither GPU nor CPU";
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		return "Integrated GPU";
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		return "Discrete GPU";
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		return "Virtual GPU";
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return "CPU";
	default:
		return "Unrecognized device type";
	}
}


int main(){
createInstance();
uint32_t dev_count = MAX_DEVICES;
tut1_enumerate_devices(instance, devs, &dev_count);
printf("I detected the following %"PRIu32" device%s:\n", dev_count, dev_count == 1?"":"s");

for (uint32_t i = 0; i < dev_count; ++i)
	{
		tut1_physical_device *dev = &devs[i];
		VkPhysicalDeviceProperties *pr = &dev->properties;

		printf("  - %s: %s (id: 0x%04X) from vendor 0x%04X [driver version: 0x%04X, API version: 0x%04X]\n",
				tut1_VkPhysicalDeviceType_string(pr->deviceType), pr->deviceName,
				pr->deviceID, pr->vendorID, pr->driverVersion, pr->apiVersion);
		if (dev->queue_families_incomplete)
		{
			
			printf("    I have information on only %"PRIu32" of them:\n", dev->queue_family_count);
		}
		else
			printf("    The device supports the following %"PRIu32" queue famil%s:\n", dev->queue_family_count, dev->queue_family_count == 1?"y":"ies");

		for (uint32_t j = 0; j < dev->queue_family_count; ++j)
		{
			VkQueueFamilyProperties *qf = &dev->queue_families[j];

			printf("    * %"PRIu32" queue%s with the following capabilit%s:\n", qf->queueCount, qf->queueCount == 1?"":"s",
					qf->queueFlags && (qf->queueFlags & (qf->queueFlags - 1)) == 0?"y":"ies");
			if (qf->queueFlags == 0)
				printf("          None\n");
			if ((qf->queueFlags & VK_QUEUE_GRAPHICS_BIT))
				printf("          Graphics\n");
			if ((qf->queueFlags & VK_QUEUE_COMPUTE_BIT))
				printf("          Compute\n");
			if ((qf->queueFlags & VK_QUEUE_TRANSFER_BIT))
				printf("          Transfer\n");
			if ((qf->queueFlags & VK_QUEUE_SPARSE_BINDING_BIT))
				printf("          Sparse binding\n");
		}

		printf("    The device supports memories of the following types:\n");
		for (uint32_t j = 0; j < dev->memories.memoryTypeCount; ++j)
		{
			printf("    *");
			if (dev->memories.memoryTypes[j].propertyFlags == 0)
				printf(" <no properties>");
			if ((dev->memories.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
				printf(" device-local");
			if ((dev->memories.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
				printf(" host-visible");
			if ((dev->memories.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
				printf(" host-coherent");
			if ((dev->memories.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT))
				printf(" host-cached");
			if ((dev->memories.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
				printf(" lazy");
			printf(": Available in Heap of size %"PRIu64"MB\n", dev->memories.memoryHeaps[dev->memories.memoryTypes[j].heapIndex].size / (1024 * 1024));
		}
	}

	

printf("It works");
}
