#include <chrono>
#include <thread>
#include <map>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

#include <assert.h>

#include <nvml.h>

using container_map = std::map<const std::string, unsigned long long>;

const std::string find_container_id(const unsigned int pid) {
    const std::string filename = "mountinfo";
    const std::string mi_dir = "/proc/" + std::to_string(pid) + "/";

    std::ifstream mi_file{mi_dir + filename};
    if (mi_file.is_open())
    {
        std::stringstream str_stream;
        str_stream << mi_file.rdbuf();
        
        const std::string s{str_stream.str()};
        std::smatch m;
        std::regex e ("(.*)( /docker/)([^ ]*)( /sys/fs/cgroup/memory)");

        std::regex_search(s, m, e);
        return m[3];
    }
    
    return "";
}

class grw
{
private:
    static bool nvml_initialized;
    unsigned int device_count;

    container_map pid_table;

    unsigned int find_devices();
    void update();
    void print_table();
    void write_to_file();
public:
    grw();
    ~grw();
    bool initialized();

    void run();
    
};

grw::grw()
{
    assert (!initialized());

    nvmlReturn_t result = nvmlInit();
    assert (result==NVML_SUCCESS);
    nvml_initialized = true;

    assert (find_devices() > 0);
}

grw::~grw()
{
    nvmlReturn_t result = nvmlShutdown();
    assert (result==NVML_SUCCESS);
    nvml_initialized = false;
}

bool grw::initialized() {
    return nvml_initialized;
}

unsigned int grw::find_devices() {
    nvmlReturn_t result = nvmlDeviceGetCount(&device_count);
    if (result != NVML_SUCCESS)
    {
        return 0;
    }
    return device_count;
}

void grw::update() {

    container_map new_table;

    nvmlReturn_t result;
    for (unsigned int i = 0; i < device_count; i++)
    {
        nvmlDevice_t device;
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (result != NVML_SUCCESS) return;

        unsigned int process_info_count = 512;
        nvmlProcessInfo_t process_arr[process_info_count];
        
        result = nvmlDeviceGetComputeRunningProcesses(device, &process_info_count, process_arr);
        if (result != NVML_SUCCESS) return;
        for (unsigned int i = 0; i < process_info_count; i++)
        {
            auto container_id = find_container_id(process_arr[i].pid);
            if (container_id != "")
            {

                new_table[container_id] =
                    new_table.find(container_id) == new_table.end()
                        ? process_arr[i].usedGpuMemory
                        : new_table[container_id] +
                              process_arr[i].usedGpuMemory;
            }
            
        }
        
        if (pid_table != new_table)
        {
            pid_table.swap(new_table);
            print_table();
            write_to_file();
        }
        
        // Skip nvmlDeviceGetGraphicsRunningProcesses when prototyping
    }
    
}

void grw::print_table() {
    for (auto const& x : pid_table)
        std::cout << x.first
                  << " : "
                  << x.second
                  << std::endl;
    
}

void grw::write_to_file() {
    const std::string output_filename{"/tmp/pid_table"};
    std::ofstream output_file;
    output_file.open(output_filename);
    for (auto const& [key, value] : pid_table)
    {
        output_file << key
                    << " : "
                    << value
                    << std::endl;
    }
    
    output_file.close();
}

void grw::run() {
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        update();
    }
    
}

bool grw::nvml_initialized = false;

int main(int argc, char const *argv[])
{
    grw w{};
    w.run();
    return 0;
}
