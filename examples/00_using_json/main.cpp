#include <iostream>
#include <iomanip>
#include <fstream>

#include <json.hpp>

using json = nlohmann::json;

namespace ns {
    // a simple struct to model a person
    struct person {
        std::string name;
        std::string address;
        int age;
    };
}

int main() {

    json j =
    {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {
            "answer", {
                {"everything", 42}
            }
        },
        {"list", {1, 0, 2}},
        {
            "object", {
                {"currency", "USD"},
                {"value", 42.99}
            }
        }
    };

    // count elements
    auto s = j.size();
    j["size"] = s; //viene inserito nella struttura del json, quindi nel file

    // pretty print with indent of 4 spaces
    std::cout << std::setw(4) << j << '\n'; //stampa indentata
    std::cout << j << std::endl; //stampa classica: tutto in una riga

    std::ofstream file("key.json");
    file << j;



    ns::person p = {"Ned Flanders", "744 Evergreen Terrace", 60};

    // convert to JSON: copy each value into the JSON object
    json j1;
    j1["name"] = p.name;
    j1["address"] = p.address;
    j1["age"] = p.age;
    std::ofstream file1("test1.json");
    file1 << j1;

    // convert from JSON: copy each value from the JSON object
    ns::person p1 {
        j1["name"].get<std::string>(),
        j1["address"].get<std::string>(),
        j1["age"].get<int>()
    };
}

