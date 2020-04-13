#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#ifndef io_tool_hpp
#define io_tool_hpp

class OutputFile {
    std::ofstream output_file;

    void init(const std::string &filename);

  public:
    OutputFile();

    OutputFile(const std::string &filename);

    ~OutputFile();

    template <typename T> OutputFile &operator<<(T &val) {
        output_file << val;
        return *this;
    }
};

const std::string find_container_id(std::string pid);

const std::string find_container_id(unsigned int pid);

#endif /* io_tool_hpp */