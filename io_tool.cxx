#include "io_tool.hpp"

OutputFile::OutputFile() { init("/tmp/pid_table"); }

OutputFile::OutputFile(const std::string &filename) { init(filename); }

OutputFile::~OutputFile() {
    if (output_file.is_open())
        output_file.close();
}

void OutputFile::init(const std::string &filename) {
    output_file.open(filename);
}

const std::string find_container_id(std::string pid) {
    const std::string filename = "mountinfo";
    const std::string mi_dir = "/proc/" + pid + "/";

    std::ifstream mi_file{mi_dir + filename};
    if (mi_file.is_open()) {
        std::stringstream str_stream;
        str_stream << mi_file.rdbuf();

        const std::string s{str_stream.str()};
        std::smatch m;
        std::regex e("(.*)( /docker/)([^ ]*)( /sys/fs/cgroup/memory)");

        std::regex_search(s, m, e);
        return m[3];
    }

    return "";
}

const std::string find_container_id(unsigned int pid) {
    return find_container_id(std::to_string(pid));
}