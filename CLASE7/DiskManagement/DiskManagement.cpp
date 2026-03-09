#include "DiskManagement.h"
#include "../Utilities/Utilities.h"
#include "../Structs/Structs.h"
#include <iostream>
#include <filesystem>
#include <cstring>
#include <vector>
#include <sstream>

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

void DiskManagement::Mkfs(const std::string& id) {
    std::cout << "======INICIO MKFS======\n";

    if (id.empty()) {
        std::cout << "Error: -id obligatorio\n";
        return;
    }

    std::string filepath = "./test/A.bin";
    auto file = Utilities::OpenFile(filepath);
    if (!file.is_open()) {
        std::cout << "No se pudo abrir disco\n";
        return;
    }

    // Leer MBR
    MBR mbr{};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));

    // Buscar partición
    Partition part{};
    bool found = false;
    for (int i = 0; i < 4; i++) {
        if (std::string(mbr.Partitions[i].Id) == id) {
            part = mbr.Partitions[i];
            found = true;
            break;
        }
    }
    if (!found) {
        std::cout << "Partición no encontrada\n";
        file.close();
        return;
    }

    // SuperBlock
    int partition_start = part.Start;
    int partition_size = part.Size;
    SuperBlock sb{};

    int n = (partition_size - sizeof(SuperBlock)) / (sizeof(Inode) + 3*sizeof(FolderBlock) + 4);
    sb.filesystem_type = 2;
    sb.inodes_count = n;
    sb.blocks_count = n * 3;
    sb.free_inodes_count = n - 1;
    sb.free_blocks_count = sb.blocks_count - 1;
    sb.bm_inode_start = partition_start + sizeof(SuperBlock);
    sb.bm_block_start = sb.bm_inode_start + n;
    sb.inode_start = sb.bm_block_start + sb.blocks_count;
    sb.block_start = sb.inode_start + (n * sizeof(Inode));

    // Escribir SuperBlock
    file.seekp(partition_start);
    file.write(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));

    char zero = '0';
    char one = '1';

    // Bitmap inodos
    for (int i = 0; i < n; i++) {
        file.seekp(sb.bm_inode_start + i);
        file.write(&zero, 1);
    }

    // Bitmap bloques
    for (int i = 0; i < sb.blocks_count; i++) {
        file.seekp(sb.bm_block_start + i);
        file.write(&zero, 1);
    }

    // Marcar root
    file.seekp(sb.bm_inode_start);
    file.write(&one, 1);
    file.seekp(sb.bm_block_start);
    file.write(&one, 1);

    // Crear inodo root
    Inode root{};
    root.uid = 1;
    root.gid = 1;
    root.size = 0;
    root.type = 0; // carpeta
    root.perm = 664;
    for (int i = 0; i < 15; i++) root.block[i] = -1;
    root.block[0] = 0;
    file.seekp(sb.inode_start);
    file.write(reinterpret_cast<char*>(&root), sizeof(Inode));

    // Crear bloque root
    FolderBlock folder{};
    strcpy(folder.content[0].name, ".");
    folder.content[0].inode = 0;
    strcpy(folder.content[1].name, "..");
    folder.content[1].inode = 0;
    folder.content[2].inode = -1;
    folder.content[3].inode = -1;
    file.seekp(sb.block_start);
    file.write(reinterpret_cast<char*>(&folder), sizeof(FolderBlock));

    file.flush();
    file.close();

    std::cout << "Sistema EXT2 creado correctamente\n";
    std::cout << "======FIN MKFS======\n";
}

void DiskManagement::Mkdir(const std::string& path) {
    std::cout << "======INICIO MKDIR RECURSIVO======\n";

    if (path.empty() || path[0] != '/') {
        std::cout << "Error: path invalido. Debe iniciar con '/'\n";
        return;
    }

    std::string filepath = "./test/A.bin";
    auto file = Utilities::OpenFile(filepath);
    if (!file.is_open()) {
        std::cout << "No se pudo abrir disco\n";
        return;
    }

    // Leer MBR
    MBR mbr{};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));
    Partition part = mbr.Partitions[0]; // asumimos partición montada

    // Leer SuperBlock
    SuperBlock sb{};
    file.seekg(part.Start);
    file.read(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));

    if (sb.filesystem_type != 2) {
        std::cout << "Partición no formateada\n";
        file.close();
        return;
    }

    std::stringstream ss(path);
    std::string folderName;
    int currentInodeIndex = 0; // root
    std::string fullPath = "."; // ruta física relativa

    while (std::getline(ss, folderName, '/')) {
        if (folderName.empty()) continue;

        if (folderName.length() > 11) {
            std::cout << "Nombre demasiado largo: " << folderName << "\n";
            file.close();
            return;
        }

        fullPath += "/" + folderName; // acumula ruta física

        // Leer inodo actual
        Inode current{};
        file.seekg(sb.inode_start + currentInodeIndex * sizeof(Inode));
        file.read(reinterpret_cast<char*>(&current), sizeof(Inode));

        bool found = false;

        // Revisar si la carpeta ya existe
        for (int i = 0; i < 12 && !found; i++) {
            if (current.block[i] == -1) continue;

            FolderBlock fb{};
            file.seekg(sb.block_start + current.block[i] * sizeof(FolderBlock));
            file.read(reinterpret_cast<char*>(&fb), sizeof(FolderBlock));

            for (int j = 0; j < 4; j++) {
                if (fb.content[j].inode != -1 &&
                    folderName == fb.content[j].name) {
                    currentInodeIndex = fb.content[j].inode;
                    found = true;
                    break;
                }
            }
        }

        if (found) continue; // ya existe, ir a la siguiente carpeta

        // =======================
        // Crear nueva carpeta
        // =======================
        if (sb.free_inodes_count <= 0 || sb.free_blocks_count <= 0) {
            std::cout << "Sin espacio\n";
            file.close();
            return;
        }

        // Buscar nuevo inodo
        int newInode = -1;
        int newBlock = -1;
        char one = '1';

        for (int i = 0; i < sb.inodes_count; i++) {
            char status;
            file.seekg(sb.bm_inode_start + i);
            file.read(&status, 1);
            if (status == '0') {
                newInode = i;
                file.seekp(sb.bm_inode_start + i);
                file.write(&one, 1);
                sb.free_inodes_count--;
                break;
            }
        }

        // Buscar nuevo bloque
        for (int i = 0; i < sb.blocks_count; i++) {
            char status;
            file.seekg(sb.bm_block_start + i);
            file.read(&status, 1);
            if (status == '0') {
                newBlock = i;
                file.seekp(sb.bm_block_start + i);
                file.write(&one, 1);
                sb.free_blocks_count--;
                break;
            }
        }

        if (newInode == -1 || newBlock == -1) {
            std::cout << "Sin espacio\n";
            file.close();
            return;
        }

        // Crear inodo nuevo
        Inode newFolder{};
        newFolder.type = 0;
        newFolder.perm = 664;
        newFolder.uid = 1;
        newFolder.gid = 1;
        newFolder.size = 0;
        for (int i = 0; i < 15; i++) newFolder.block[i] = -1;
        newFolder.block[0] = newBlock;

        file.seekp(sb.inode_start + newInode * sizeof(Inode));
        file.write(reinterpret_cast<char*>(&newFolder), sizeof(Inode));

        // Crear bloque carpeta
        FolderBlock fb{};
        strcpy(fb.content[0].name, ".");
        fb.content[0].inode = newInode;
        strcpy(fb.content[1].name, "..");
        fb.content[1].inode = currentInodeIndex;
        fb.content[2].inode = -1;
        fb.content[3].inode = -1;

        file.seekp(sb.block_start + newBlock * sizeof(FolderBlock));
        file.write(reinterpret_cast<char*>(&fb), sizeof(FolderBlock));

        // Insertar en el bloque padre (directos)
        bool inserted = false;
        for (int i = 0; i < 12 && !inserted; i++) {
            if (current.block[i] == -1) continue;

            FolderBlock parentFB{};
            file.seekg(sb.block_start + current.block[i] * sizeof(FolderBlock));
            file.read(reinterpret_cast<char*>(&parentFB), sizeof(FolderBlock));

            for (int j = 0; j < 4; j++) {
                if (parentFB.content[j].inode == -1) {
                    strcpy(parentFB.content[j].name, folderName.c_str());
                    parentFB.content[j].inode = newInode;

                    file.seekp(sb.block_start + current.block[i] * sizeof(FolderBlock));
                    file.write(reinterpret_cast<char*>(&parentFB), sizeof(FolderBlock));
                    inserted = true;
                    break;
                }
            }
        }

        // =======================
        // Crear carpeta física en sistema real
        // =======================
        try {
            std::filesystem::create_directories(fullPath);
        } catch (const std::exception& e) {
            std::cout << "Error creando carpeta física: " << e.what() << "\n";
        }

        currentInodeIndex = newInode; // avanzar al siguiente nivel
    }

    // Guardar SuperBlock actualizado
    file.seekp(part.Start);
    file.write(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));
    file.flush();
    file.close();

    std::cout << "MKDIR COMPLETADO CORRECTAMENTE\n";
}

void Mkfile(const std::string& path,
                            const std::string& cont) {

    std::cout << "======INICIO MKFILE======\n";

    if (path.empty() || path[0] != '/') {
        std::cout << "Path invalido\n";
        return;
    }

    // =============================
    // LEER CONTENIDO ARCHIVO REAL
    // =============================

    std::ifstream contentFile(cont);
    if (!contentFile.is_open()) {
        std::cout << "No se pudo abrir archivo de contenido\n";
        return;
    }

    std::stringstream buffer;
    buffer << contentFile.rdbuf();
    std::string fileContent = buffer.str();
    contentFile.close();

    // =============================
    // ABRIR DISCO
    // =============================

    std::string filepath = "./test/A.bin";
    auto file = Utilities::OpenFile(filepath);

    if (!file.is_open()) {
        std::cout << "No se pudo abrir disco\n";
        return;
    }

    // =============================
    // LEER MBR
    // =============================

    MBR mbr{};
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&mbr), sizeof(MBR));

    Partition part = mbr.Partitions[0];

    // =============================
    // LEER SUPERBLOCK
    // =============================

    SuperBlock sb{};
    file.seekg(part.Start);
    file.read(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));

    if (sb.filesystem_type != 2) {
        std::cout << "Partición no formateada\n";
        file.close();
        return;
    }

    // =============================
    // SEPARAR PATH
    // =============================

    std::stringstream ss(path);
    std::string token;
    std::vector<std::string> folders;

    while (std::getline(ss, token, '/')) {
        if (!token.empty())
            folders.push_back(token);
    }

    if (folders.empty()) {
        std::cout << "Path invalido\n";
        file.close();
        return;
    }

    std::string filename = folders.back();
    folders.pop_back();

    int currentInodeIndex = 0;

    // =============================
    // BUSCAR DIRECTORIO PADRE
    // =============================

    for (auto& folder : folders) {

        Inode current{};
        file.seekg(sb.inode_start + currentInodeIndex * sizeof(Inode));
        file.read(reinterpret_cast<char*>(&current), sizeof(Inode));

        bool found = false;

        for (int i = 0; i < 12 && !found; i++) {

            if (current.block[i] == -1)
                continue;

            FolderBlock fb{};
            file.seekg(sb.block_start + current.block[i] * sizeof(FolderBlock));
            file.read(reinterpret_cast<char*>(&fb), sizeof(FolderBlock));

            for (int j = 0; j < 4; j++) {

                if (fb.content[j].inode != -1 &&
                    folder == fb.content[j].name) {

                    currentInodeIndex = fb.content[j].inode;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            std::cout << "Directorio no existe: " << folder << "\n";
            file.close();
            return;
        }
    }

    // =============================
    // BUSCAR INODO LIBRE
    // =============================

    int newInode = -1;
    char one = '1';

    for (int i = 0; i < sb.inodes_count; i++) {

        char status;

        file.seekg(sb.bm_inode_start + i);
        file.read(&status, 1);

        if (status == '0') {

            newInode = i;

            file.seekp(sb.bm_inode_start + i);
            file.write(&one, 1);

            sb.free_inodes_count--;
            break;
        }
    }

    if (newInode == -1) {
        std::cout << "Sin inodos libres\n";
        file.close();
        return;
    }

    // =============================
    // CREAR INODO ARCHIVO
    // =============================

    Inode fileInode{};
    fileInode.uid = 1;
    fileInode.gid = 1;
    fileInode.size = fileContent.size();
    fileInode.type = 1;
    fileInode.perm = 664;

    for (int i = 0; i < 15; i++)
        fileInode.block[i] = -1;

    // =============================
    // ESCRIBIR BLOQUES DE ARCHIVO
    // =============================

    int blockIndex = 0;

    for (size_t i = 0; i < fileContent.size(); i += 64) {

        int newBlock = -1;

        for (int j = 0; j < sb.blocks_count; j++) {

            char status;

            file.seekg(sb.bm_block_start + j);
            file.read(&status, 1);

            if (status == '0') {

                newBlock = j;

                file.seekp(sb.bm_block_start + j);
                file.write(&one, 1);

                sb.free_blocks_count--;
                break;
            }
        }

        if (newBlock == -1) {
            std::cout << "Sin bloques libres\n";
            file.close();
            return;
        }

        FileBlock fb{};

        std::string chunk = fileContent.substr(i, 64);
        memcpy(fb.content, chunk.c_str(), chunk.size());

        file.seekp(sb.block_start + newBlock * sizeof(FileBlock));
        file.write(reinterpret_cast<char*>(&fb), sizeof(FileBlock));

        fileInode.block[blockIndex++] = newBlock;

        if (blockIndex >= 12)
            break;
    }

    // =============================
    // GUARDAR INODO
    // =============================

    file.seekp(sb.inode_start + newInode * sizeof(Inode));
    file.write(reinterpret_cast<char*>(&fileInode), sizeof(Inode));

    // =============================
    // AGREGAR A CARPETA PADRE
    // =============================

    Inode parent{};
    file.seekg(sb.inode_start + currentInodeIndex * sizeof(Inode));
    file.read(reinterpret_cast<char*>(&parent), sizeof(Inode));

    bool inserted = false;

    for (int i = 0; i < 12 && !inserted; i++) {

        if (parent.block[i] == -1)
            continue;

        FolderBlock fb{};

        file.seekg(sb.block_start + parent.block[i] * sizeof(FolderBlock));
        file.read(reinterpret_cast<char*>(&fb), sizeof(FolderBlock));

        for (int j = 0; j < 4; j++) {

            if (fb.content[j].inode == -1) {

                strcpy(fb.content[j].name, filename.c_str());
                fb.content[j].inode = newInode;

                file.seekp(sb.block_start + parent.block[i] * sizeof(FolderBlock));
                file.write(reinterpret_cast<char*>(&fb), sizeof(FolderBlock));

                inserted = true;
                break;
            }
        }
    }

    if (!inserted)
        std::cout << "No se pudo insertar archivo en carpeta\n";

    // =============================
    // GUARDAR SUPERBLOCK
    // =============================

    file.seekp(part.Start);
    file.write(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));

    file.flush();
    file.seekp(part.Start);
file.write(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));

// =============================
// CREAR ARCHIVO FISICO
// =============================

try {

    std::string realPath = "." + path;

    std::ofstream realFile(realPath);

    if (realFile.is_open()) {
        realFile << fileContent;
        realFile.close();
    }

} catch (const std::exception& e) {

    std::cout << "Error creando archivo fisico: "
              << e.what() << "\n";
}
file.flush();
file.close();

    std::cout << "Archivo creado correctamente\n";
    std::cout << "======FIN MKFILE======\n";
}

}
