#include <map>

#include "io_tool.hpp"
#include "nvml_tool.hpp"

#ifndef grw_hpp
#define grw_hpp

using container_map = std::map<const std::string, unsigned long long>;

class grw {
  private:
    NVMLTool nvml_tool;

    container_map pid_table;

    void update();
    void print_table();
    void write_to_file();

  public:
    grw();
    ~grw();

    void run();
};

#endif