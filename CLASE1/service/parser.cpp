#include <fstream>
#include <regex>
#include <vector>
#include "../model/result.h"

std::vector<LineAnalysis> AnalizarArchivo(const std::string& path){
    std::ifstream file(path);
    if(!file.is_open()){
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    std::regex mkdiskRegex(R"(^mkdisk\s+-Size=\d+\s+-unit=[K|M]\s+-path=\/[\/\w\.-]+\.mia$)");

    std::vector<LineAnalysis> resultados;
    std::string linea;

    while (std::getline(file, linea)) {
        bool valida = std::regex_match(linea, mkdiskRegex);
        resultados.push_back({
            linea,
            valida,
            valida ? "Comando válido" : "Comando inválido"
        });
    }

    return resultados;
}