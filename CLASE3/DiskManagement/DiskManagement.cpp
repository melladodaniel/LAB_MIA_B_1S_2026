#include "DiskManagement.h"
#include "../Utilities/Utilities.h"
#include "../Structs/Structs.h"
#include <iostream>
#include <cstring>
#include <vector>

namespace DiskManagement {

void Mkdisk(int size, const std::string& fit, const std::string& unit) {

    std::cout << "======Inicio MKDISK======\n";

    if (fit != "bf" && fit != "ff" && fit != "wf") {
        std::cout << "Error: Fit debe ser bf, ff o wf\n";
        return;
    }

    if (size <= 0) {
        std::cout << "Error: Tamaño debe ser mayor a 0\n";
        return;
    }

    if (unit != "k" && unit != "m") {
        std::cout << "Error: Unidad debe ser k o m\n";
        return;
    }

    Utilities::CreateFile("./test/A.bin");

    if (unit == "k")
        size *= 1024;
    else
        size *= 1024 * 1024;

    auto file = Utilities::OpenFile("./test/A.bin");

    std::vector<char> zeroBuffer(1024, 0);

    for (int i = 0; i < size / 1024; i++) {
        file.seekp(i * 1024);
        file.write(zeroBuffer.data(), 1024);
    }

    MBR newMBR{};
    newMBR.MbrSize = size;
    newMBR.Signature = 10;

    std::memcpy(newMBR.Fit, fit.c_str(), 2);
    std::memcpy(newMBR.CreationDate, "2026-09-02", 10);

    file.seekp(0);
    file.write(reinterpret_cast<char*>(&newMBR), sizeof(MBR));

    MBR tempMBR{};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&tempMBR), sizeof(MBR));

    std::cout << "===Data recuperada===\n";
    std::cout << "Tamaño: " << tempMBR.MbrSize << "\n";
    std::cout << "Fit: " << std::string(tempMBR.Fit, 2) << "\n";
    std::cout << "Firma: " << tempMBR.Signature << "\n";

    file.close();

    std::cout << "======Fin MKDISK======\n";
}

}