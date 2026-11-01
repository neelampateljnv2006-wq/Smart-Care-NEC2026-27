#include "crow.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>

using json = nlohmann::json;

class StorageManager {
private:
    std::string basePath;
    std::mutex fileMutex;
public:
    StorageManager(const std::string& path = "storage/") : basePath(path) {
        // Ensure storage directory concept works seamlessly
    }

    json readData(const std::string& filename) {
        std::lock_guard<std::mutex> lock(fileMutex);
        std::ifstream file(basePath + filename);
        if (!file.is_open()) return json::array();
        json j;
        try { file >> j; } catch (...) { j = json::array(); }
        return j;
    }

    bool writeData(const std::string& filename, const json& data) {
        std::lock_guard<std::mutex> lock(fileMutex);
        std::ofstream file(basePath + filename);
        if (!file.is_open()) return false;
        file << data.dump(4);
        file.close();
        return true;
    }
};

crow::response cors(crow::response res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return res;
}

int main() {
    crow::SimpleApp app;
    StorageManager storage("storage/");

    CROW_ROUTE(app, "/api/<path>").methods("OPTIONS"_method)([](const std::string&) {
        return cors(crow::response(200));
    });

    CROW_ROUTE(app, "/")([]() {
        std::ifstream t("frontend/index.html");
        if (!t.is_open()) t.open("index.html");
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        crow::response res(str);
        res.add_header("Content-Type", "text/html");
        return res;
    });

    // --- GET ALL MODULES ---
    CROW_ROUTE(app, "/api/patients").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("patients.json").dump()));
    });
    CROW_ROUTE(app, "/api/doctors").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("doctors.json").dump()));
    });
    CROW_ROUTE(app, "/api/appointments").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("appointments.json").dump()));
    });
    CROW_ROUTE(app, "/api/emergencies").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("emergencies.json").dump()));
    });
    CROW_ROUTE(app, "/api/beds").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("beds.json").dump()));
    });
    CROW_ROUTE(app, "/api/pharmacy").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("pharmacy.json").dump()));
    });
    CROW_ROUTE(app, "/api/lab").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("lab.json").dump()));
    });
    CROW_ROUTE(app, "/api/billing").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("billing.json").dump()));
    });
    CROW_ROUTE(app, "/api/discharges").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("discharges.json").dump()));
    });
    CROW_ROUTE(app, "/api/ambulances").methods("GET"_method)([&storage]() {
        return cors(crow::response(200, storage.readData("ambulances.json").dump()));
    });

    // --- POST: ADD PATIENT ---
    CROW_ROUTE(app, "/api/patients").methods("POST"_method)([&storage](const crow::request& req) {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return cors(crow::response(400, "{\"error\":\"Invalid JSON\"}"));
        
        json list = storage.readData("patients.json");
        body["id"] = "P-" + std::to_string(100 + list.size() + 1);
        list.push_back(body);
        storage.writeData("patients.json", list);
        return cors(crow::response(201, body.dump()));
    });

    // --- GET: SEARCH PATIENTS (ID or Name) ---
    CROW_ROUTE(app, "/api/patients/search").methods("GET"_method)([&storage](const crow::request& req) {
        char* q_str = req.url_params.get("q");
        std::string query = q_str ? q_str : "";
        
        json list = storage.readData("patients.json");
        json results = json::array();

        for (const auto& p : list) {
            std::string id = p.value("id", "");
            std::string name = p.value("name", "");
            // Case-insensitive or direct substring check
            if (query.empty() || id.find(query) != std::string::npos || name.find(query) != std::string::npos) {
                results.push_back(p);
            }
        }
        return cors(crow::response(200, results.dump()));
    });

    // --- DELETE: REMOVE PATIENT BY ID ---
    CROW_ROUTE(app, "/api/patients/<string>").methods("DELETE"_method)([&storage](std::string id) {
        json list = storage.readData("patients.json");
        json updatedList = json::array();
        bool found = false;

        for (const auto& p : list) {
            if (p.value("id", "") == id) {
                found = true; // Skip adding to remove it
            } else {
                updatedList.push_back(p);
            }
        }

        if (found) {
            storage.writeData("patients.json", updatedList);
            return cors(crow::response(200, "{\"message\":\"Patient deleted successfully\"}"));
        }
        return cors(crow::response(404, "{\"error\":\"Patient not found\"}"));
    });

    // --- PORT CONFIGURATION FOR RENDER ---
    int port = 8080;
    const char* env_port = std::getenv("PORT");
    if (env_port != nullptr) {
        port = std::stoi(env_port);
    }

    std::cout << "SmartCare C++17 Backend running on port " << port << "\n";
    app.port(port).multithreaded().run();
}