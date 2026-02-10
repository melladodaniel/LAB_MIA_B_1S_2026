#include "Utilities.h"
#include <filesystem>

namespace Utilities {

bool CreateFile(const std::string& name) {
    std::filesystem::create_directories(
        std::filesystem::path(name).parent_path()
    );

    std::ofstream file(name, std::ios::binary | std::ios::app);
    return file.good();
}

std::fstream OpenFile(const std::string& name) {
    return std::fstream(name, std::ios::in | std::ios::out | std::ios::binary);
}

template<typename T>
bool WriteObject(std::fstream& file, const T& data, std::streampos pos) {
    file.seekp(pos);
    file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    return file.good();
}

template<typename T>
bool ReadObject(std::fstream& file, T& data, std::streampos pos) {
    file.seekg(pos);
    file.read(reinterpret_cast<char*>(&data), sizeof(T));
    return file.good();
}

}