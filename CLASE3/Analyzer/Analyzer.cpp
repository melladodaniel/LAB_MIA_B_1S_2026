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

    if (command == "mkdisk") {

        std::regex re(R"(-(\w+)=("[^"]+"|\S+))");
        std::sregex_iterator it(input.begin(), input.end(), re);
        std::sregex_iterator end;

        std::map<std::string, std::string> params;

        for (; it != end; ++it) {
            params[it->str(1)] = it->str(2);
        }

        int size = std::stoi(params["size"]);
        std::string fit = params["fit"];
        std::string unit = params["unit"];

        DiskManagement::Mkdisk(size, fit, unit);
    }
}

}