#include "Analyzer.h"
#include "../DiskManagement/DiskManagement.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <map>

namespace Analyzer {

void Analyze() {
    std::string input;
    std::cout << "Ingresa un comando: ";
    std::getline(std::cin, input);

    std::istringstream iss(input);
    std::string command;
    iss >> command;

    // convertir comando a lowercase
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    // Regex para extraer parámetros
    std::regex re(R"(-(\w+)=("[^"]+"|\S+))");
    std::sregex_iterator it(input.begin(), input.end(), re);
    std::sregex_iterator end;

    std::map<std::string, std::string> params;

    for (; it != end; ++it) {
        std::string key = it->str(1);
        std::string value = it->str(2);

        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        params[key] = value;
    }

    if (command == "mkdisk") {

        int size = std::stoi(params["size"]);
        std::string fit = params["fit"];
        std::string unit = params["unit"];

        DiskManagement::Mkdisk(size, fit, unit);

    } else if (command == "fdisk") {

        int size = std::stoi(params["size"]);
        std::string drive = params["drive"];
        std::string name = params["name"];
        std::string type = params.count("type") ? params["type"] : "p";
        std::string fit = params.count("fit") ? params["fit"] : "f";
        std::string unit = params.count("unit") ? params["unit"] : "m";

        DiskManagement::Fdisk(size, drive, name, type, fit, unit);

    } else if (command == "mount") {

        std::string drive = params["drive"];
        std::string name = params["name"];

        DiskManagement::Mount(drive, name);

    } else if (command == "exit") {
        std::cout << "Saliendo del programa\n";
        exit(0);
    } else {
        std::cout << "Error: Comando no reconocido\n";
    }
}

}
