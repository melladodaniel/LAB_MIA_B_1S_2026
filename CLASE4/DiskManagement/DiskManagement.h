#pragma once
#include <string>

namespace DiskManagement {

void Mkdisk(int size, const std::string& fit, const std::string& unit);

void Fdisk(int size,
           const std::string& drive,
           const std::string& name,
           const std::string& type,
           const std::string& fit,
           const std::string& unit);

void Mount(const std::string& drive,
           const std::string& name);

}
