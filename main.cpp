#include <iostream>
#include <stdint.h>
#include <vector>

#include "Map.h"



std::string random_str(){
    std::string str;
    str.reserve(10);

    for(int i = 0; i < 10; i++){
        str += (char)(65 + rand() % 25);
    }
    return str;
}

class Car{
    int cost = 23000;
    std::string name = "Jigul";

    friend std::ostream& operator <<(std::ostream& os, const Car& car){
        os << "Cost: " << car.cost << ", Name: " << car.name << '\n';
        return os;
    }
};


int main() {

    srand(time(NULL));

    Map<int, std::string> map;

    for(int i = 0; i < 25; i++){
        map.insert(rand() % 135, random_str());
    }
    Map map_copy = map;


    map.print();

    std::vector<int> vector;

    for(auto& k : map.keys()){
        vector.push_back(k);
    }

    size_t vector_size = vector.size();

    int entry = 0;
    for(int i = 0; i < vector_size; i++){
        entry = *std::next(map.keys().begin(), rand() % map.keys().size());
        map.remove(entry);
    }

    std::cout << "map.remove test " << ((map.size() == 0) ? "passed" : "not passed") << '\n';

    std::cout << "map.copy test " << ((map_copy.size() > 0) ? "passed" : "not passed") << '\n';

    auto map_move = std::move(map_copy);
    map_copy.print();
    std::cout << "map.move test " << (((map_copy.size() == 0) && map_move.size() > 0) ? "passed" : "not passed") << '\n';

    try {
        map_move[999] = "asd";
    } catch(std::exception &exception) {
        std::cerr << exception.what() << '\n';
        std::cout << "map assign [] exeption test: passed\n";
    }

    map_move[entry] = "sdasd";
    if(map_move[entry] == "sdasd")
        std::cout << "assign value via []: passed\n";

    if(map_move.contains(entry) && !map_move.contains(999))
        std::cout << "map.contains test: passed\n";
    else
        std::cout << "map.contains test: not passed\n";

    if(map_move.find(entry) && (map_move.find(999) == nullptr))
        std::cout << "map.find(key) test: passed\n";
    else
        std::cout << "map.find(key) test: not passed\n";


    return 0;
}
