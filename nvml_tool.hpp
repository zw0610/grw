#include <nvml.h>

#include <assert.h>

#ifndef nvml_tool_hpp
#define nvml_tool_hpp

class NVMLTool {
  public:
    unsigned int device_count;

    NVMLTool();

    ~NVMLTool();

    nvmlReturn_t nvmlDeviceGetRunningProceses(unsigned int device_idx,
                                              unsigned int *infoCount,
                                              nvmlProcessInfo_t *infos,
                                              bool computeMode = true);
};

#endif /* nvml_tool_hpp */