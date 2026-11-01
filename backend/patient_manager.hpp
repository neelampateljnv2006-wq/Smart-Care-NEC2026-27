#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Patient {
    std::string id;
    std::string name;
    int age;
    std::string disease;
    std::string admissionDate;
    std::string gender;
    std::string contact;
    std::string bloodGroup;
    std::string address;
};

class PatientManager {
private:
    std::string filename;

public:
    PatientManager(std::string file = "patients.json") : filename(file) {}

    // 1. Read all records from file
    json getAllPatients() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return json::array();
        }
        json data;
        try {
            file >> data;
        } catch (...) {
            data = json::array();
        }
        file.close();
        return data;
    }

    // 2. Save records to file
    void saveToFile(const json& data) {
        std::ofstream file(filename);
        file << data.dump(4);
        file.close();
    }

    // 3. Add Record
    json addPatient(json& newPatient) {
        json patients = getAllPatients();
        std::string newId = "P-" + std::to_string(100 + patients.size() + 1);
        newPatient["id"] = newId;
        
        patients.push_back(newPatient);
        saveToFile(patients);
        return newPatient;
    }

    // 4. Search by ID or Name
    json searchPatients(const std::string& query) {
        json patients = getAllPatients();
        json results = json::array();
        
        for (const auto& p : patients) {
            std::string id = p.value("id", "");
            std::string name = p.value("name", "");
            if (id.find(query) != std::string::npos || name.find(query) != std::string::npos) {
                results.push_back(p);
            }
        }
        return results;
    }

    // 5. Update Record
    bool updatePatient(const std::string& id, const json& updatedData) {
        json patients = getAllPatients();
        bool found = false;
        
        for (auto& p : patients) {
            if (p["id"] == id) {
                if (updatedData.contains("name")) p["name"] = updatedData["name"];
                if (updatedData.contains("age")) p["age"] = updatedData["age"];
                if (updatedData.contains("disease")) p["disease"] = updatedData["disease"];
                if (updatedData.contains("admissionDate")) p["admissionDate"] = updatedData["admissionDate"];
                found = true;
                break;
            }
        }
        if (found) {
            saveToFile(patients);
        }
        return found;
    }

    // 6. Delete Record
    bool deletePatient(const std::string& id) {
        json patients = getAllPatients();
        json updatedList = json::array();
        bool found = false;

        for (const auto& p : patients) {
            if (p.value("id", "") == id) {
                found = true; // Skip this to delete
            } else {
                updatedList.push_back(p);
            }
        }

        if (found) {
            saveToFile(updatedList);
        }
        return found;
    }
};