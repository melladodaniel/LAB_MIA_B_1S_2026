#include "../external/crow_all.h"
#include <nlohmann/json.hpp>
#include "../service/parser.cpp"

void AnalizarArchivoHandler(const crow::request& req, crow::response& res) {
    auto file = req.url_params.get("file");

    if(!file){
        res.code = 400;
        res.body = "Parámetro 'file' es requerido.";
        res.end();
        return;
    }

    try {
        auto resultados = AnalizarArchivo(file);
        nlohmann::json jronResp = resultados;

        res.set_header("Content-Type", "application/json");
        res.write(jronResp.dump(4));
    } catch (const std::exception& e) {
        res.code = 500;
        res.body = std::string("Error al analizar el archivo: ") + e.what();
    }

    res.end();
}