//
// vulkan_extension_entrypoints.cpp
// As part of the Opal project
// --------------------------------------------------
// Description: Loader-backed definitions for Vulkan extension entry points
// that the Windows Vulkan loader does not export.
//

#include "diagnostics.h"
#if defined(VULKAN) && defined(_WIN32)

#include "opal/opal.h"
#include "vulkan_state.h"
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

// On Linux, libvulkan exports every known entry point, so Opal can call
// extension functions such as vkCmdTraceRaysKHR directly. vulkan-1.lib on
// Windows only exports core and WSI functions, so the ray tracing entry points
// are defined here and forwarded through vkGetInstanceProcAddr, which returns
// loader trampolines that dispatch to the right device.

namespace {

VkInstance currentInstance() {
    opal::Device *device = opal::Device::globalInstance;
    if (device == nullptr || device->context == nullptr) {
        throw std::runtime_error(
            "Vulkan extension function used before a device was acquired");
    }
    return opal::vulkan::contextState(device->context.get()).instance;
}

template <typename Function> Function resolve(const char *name) {
    struct Cache {
        VkInstance instance = VK_NULL_HANDLE;
        PFN_vkVoidFunction function = nullptr;
    };
    // One cache per entry point; each name maps to a distinct Function type.
    static Cache cache;
    const VkInstance instance = currentInstance();
    if (cache.function == nullptr || cache.instance != instance) {
        cache.function = vkGetInstanceProcAddr(instance, name);
        cache.instance = instance;
        if (cache.function == nullptr) {
            throw std::runtime_error(std::string("Vulkan entry point ") + name +
                                     " is not available");
        }
    }
    return reinterpret_cast<Function>(cache.function);
}

} // namespace

extern "C" {

VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR *pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR *pCallableShaderBindingTable,
    uint32_t width, uint32_t height, uint32_t depth) {
    resolve<PFN_vkCmdTraceRaysKHR>("vkCmdTraceRaysKHR")(
        commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable,
        pHitShaderBindingTable, pCallableShaderBindingTable, width, height,
        depth);
}

VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer, uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR *pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR *const *ppBuildRangeInfos) {
    resolve<PFN_vkCmdBuildAccelerationStructuresKHR>(
        "vkCmdBuildAccelerationStructuresKHR")(commandBuffer, infoCount, pInfos,
                                               ppBuildRangeInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
    VkDevice device, const VkAccelerationStructureCreateInfoKHR *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkAccelerationStructureKHR *pAccelerationStructure) {
    return resolve<PFN_vkCreateAccelerationStructureKHR>(
        "vkCreateAccelerationStructureKHR")(device, pCreateInfo, pAllocator,
                                            pAccelerationStructure);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureKHR(
    VkDevice device, VkAccelerationStructureKHR accelerationStructure,
    const VkAllocationCallbacks *pAllocator) {
    resolve<PFN_vkDestroyAccelerationStructureKHR>(
        "vkDestroyAccelerationStructureKHR")(device, accelerationStructure,
                                             pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureBuildSizesKHR(
    VkDevice device, VkAccelerationStructureBuildTypeKHR buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR *pBuildInfo,
    const uint32_t *pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR *pSizeInfo) {
    resolve<PFN_vkGetAccelerationStructureBuildSizesKHR>(
        "vkGetAccelerationStructureBuildSizesKHR")(
        device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL
vkGetAccelerationStructureDeviceAddressKHR(
    VkDevice device, const VkAccelerationStructureDeviceAddressInfoKHR *pInfo) {
    return resolve<PFN_vkGetAccelerationStructureDeviceAddressKHR>(
        "vkGetAccelerationStructureDeviceAddressKHR")(device, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
    VkDevice device, VkDeferredOperationKHR deferredOperation,
    VkPipelineCache pipelineCache, uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR *pCreateInfos,
    const VkAllocationCallbacks *pAllocator, VkPipeline *pPipelines) {
    return resolve<PFN_vkCreateRayTracingPipelinesKHR>(
        "vkCreateRayTracingPipelinesKHR")(device, deferredOperation,
                                          pipelineCache, createInfoCount,
                                          pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesKHR(
    VkDevice device, VkPipeline pipeline, uint32_t firstGroup,
    uint32_t groupCount, size_t dataSize, void *pData) {
    return resolve<PFN_vkGetRayTracingShaderGroupHandlesKHR>(
        "vkGetRayTracingShaderGroupHandlesKHR")(device, pipeline, firstGroup,
                                                groupCount, dataSize, pData);
}

} // extern "C"

#endif
