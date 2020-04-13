#include <chrono>
#include <thread>

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include <assert.h>

#include <nvml.h>

#include "grw.hpp"

grw::grw() {}

grw::~grw() {}

void grw::update() {

    container_map new_table;

    nvmlReturn_t result;
    for (unsigned int i = 0; i < nvml_tool.device_count; i++) {
        unsigned int process_info_count = 512;
        nvmlProcessInfo_t process_arr[process_info_count];

        result = nvml_tool.nvmlDeviceGetRunningProceses(i, &process_info_count,
                                                        process_arr);
        if (result != NVML_SUCCESS)
            return;
        for (unsigned int i = 0; i < process_info_count; i++) {
            auto container_id = find_container_id(process_arr[i].pid);
            if (container_id != "") {

                new_table[container_id] =
                    new_table.find(container_id) == new_table.end()
                        ? process_arr[i].usedGpuMemory
                        : new_table[container_id] +
                              process_arr[i].usedGpuMemory;
            }
        }

        if (pid_table != new_table) {
            pid_table.swap(new_table);
            print_table();
            write_to_file();
        }

        // Skip nvmlDeviceGetGraphicsRunningProcesses when prototyping
    }
}

void grw::print_table() {
    for (auto const &x : pid_table)
        std::cout << x.first << " : " << x.second << std::endl;
}

void grw::write_to_file() {
    OutputFile output_file{"/tmp/pid_table"};
    for (auto const &[key, value] : pid_table) {
        output_file << key << " : " << value << "\n";
    }
}

void grw::run() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        update();
    }
}
