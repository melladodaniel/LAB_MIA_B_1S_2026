#include "DiskManagement.h"
#include "../Utilities/Utilities.h"
#include "../Structs/Structs.h"
#include <iostream>
#include <filesystem>
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

void Fdisk(int size,
           const std::string& drive,
           const std::string& name,
           const std::string& type,
           const std::string& fit,
           const std::string& unit) {

std::cout << "======INICIO FDISK======\n";

    if (fit != "b" && fit != "f" && fit != "w") {
        std::cout << "Error: Fit debe ser b, f o w\n";
        return;
    }

    if (size <= 0) {
        std::cout << "Error: Tamaño debe ser mayor a 0\n";
        return;
    }

    if (unit != "b" && unit != "k" && unit != "m") {
        std::cout << "Error: Unidad debe ser b, k o m\n";
        return;
    }

    if (unit == "k")
        size *= 1024;
    else if (unit == "m")
        size *= 1024 * 1024;

    std::string filepath = "./test/" + drive + ".bin";
     std::cout << filepath+"\n";
    auto file = Utilities::OpenFile(filepath);
    
    if (!file.is_open()) {
        std::cout << "No se pudo abrir el archivo\n";
        return;
    }


    // Leer MBR
MBR mbr{};
file.seekg(0);
file.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));

// Buscar partición libre
int index = -1;
for (int i = 0; i < 4; i++) {
    if (mbr.Partitions[i].Status[0] == 0) {
        index = i;
        break;
    }
}

if (index == -1) {
    std::cout << "No hay espacio para más particiones\n";
    return;
}

// Llenar partición
mbr.Partitions[index].Status[0] = '0';
mbr.Partitions[index].Type[0] = type[0];
std::memcpy(mbr.Partitions[index].Fit, fit.c_str(), 1);

mbr.Partitions[index].Start = sizeof(MBR);
mbr.Partitions[index].Size = size;
std::memset(mbr.Partitions[index].Name, 0, 16);
std::memcpy(mbr.Partitions[index].Name, name.c_str(), name.size());

// Escribir MBR actualizado
file.seekp(0);
file.write(reinterpret_cast<char*>(&mbr), sizeof(MBR));
file.flush();

MBR verify{};
file.seekg(0);
file.read(reinterpret_cast<char*>(&verify), sizeof(MBR));

std::cout << "Nombre guardado: "
          << std::string(verify.Partitions[index].Name)
          << "\n";

file.close();

    std::cout << "Partición creada correctamente\n";
    std::cout << "======FIN FDISK======\n";
}

void Mount(const std::string& drive,
           const std::string& name) {

    std::cout << "======INICIO MOUNT======\n";

    std::string filepath = "./test/" + drive + ".bin";
    auto file = Utilities::OpenFile(filepath);

    MBR mbr{};
file.seekg(0);
file.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));

bool found = false;

for (int i = 0; i < 4; i++) {

    if (mbr.Partitions[i].Status[0] == '0') {

        std::string partName(mbr.Partitions[i].Name);

        if (partName == name) {
            found = true;
            mbr.Partitions[i].Status[0] = '1';

            //Generar ID (Ejemplo: A132)
            std::string id = drive + std::to_string(i + 1) + "32";
            std::memset(mbr.Partitions[i].Id, 0, 4);
            std::memcpy(mbr.Partitions[i].Id, id.c_str(), std::min((size_t)4, id.size()));
            break;
        }
    }
}

    if (!found) {
        std::cout << "Partición no encontrada\n";
        file.close();
        std::cout << "======FIN MOUNT======\n";
        return;
    }

    file.seekp(0);
    file.write(reinterpret_cast<char*>(&mbr), sizeof(MBR));

    std::cout << "Partición montada correctamente\n";

    file.close();

    std::cout << "======FIN MOUNT======\n";
    }
}
