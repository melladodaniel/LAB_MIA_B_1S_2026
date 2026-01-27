#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct LineAnalysis {
    std::string linea;
    bool valida;
    std::string mensaje;
};

inline void to_json(nlohmann::json& j, const LineAnalysis& l) {
    j = nlohmann::json{{"linea", l.linea}, 
        {"valida", l.valida}, 
        {"mensaje", l.mensaje}
    };
}