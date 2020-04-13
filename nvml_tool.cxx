#include "nvml_tool.hpp"

NVMLTool::NVMLTool() {
    nvmlReturn_t result = nvmlInit();
    assert(result == NVML_SUCCESS);

    result = nvmlDeviceGetCount(&device_count);
    assert(result == NVML_SUCCESS);
}

NVMLTool::~NVMLTool() {
    nvmlReturn_t result = nvmlShutdown();
    assert(result == NVML_SUCCESS);
}

nvmlReturn_t NVMLTool::nvmlDeviceGetRunningProceses(unsigned int device_idx,
                                                    unsigned int *infoCount,
                                                    nvmlProcessInfo_t *infos,
                                                    bool computeMode) {
    nvmlDevice_t device;
    nvmlReturn_t result = nvmlDeviceGetHandleByIndex(device_idx, &device);
    if (result != NVML_SUCCESS)
        return result;

    return computeMode
               ? nvmlDeviceGetComputeRunningProcesses(device, infoCount, infos)
               : nvmlDeviceGetGraphicsRunningProcesses(device, infoCount,
                                                       infos);
}