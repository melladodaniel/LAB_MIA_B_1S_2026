#include "external/crow_all.h"
#include "controller/handler.cpp"
#include <iostream>

int main(){
    crow::SimpleApp app;

    CROW_ROUTE(app, "/analizar")
        .methods(crow::HTTPMethod::GET)
        ([](const crow::request& req, crow::response& res){
            AnalizarArchivoHandler(req, res);
        });

    std::cout << "Servidor iniciado en http://localhost:8080\n";
    app.port(8080).multithreaded().run();
}