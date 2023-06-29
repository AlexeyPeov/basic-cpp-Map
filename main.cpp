#include <iostream>
#include <stdint.h>

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

    Map<std::string, int> map;


    map.insert("aboba",1);
    map.insert("aboba",30000);

    std::string key;
    for(int i = 0; i < 25; i++){
        key = random_str();
        map.insert(key, i);
    }

    auto value = map.find(key);

    std::cout << "CONTAINS: " << map.contains(key) << '\n';

    if(value) std::cout << "FOUND VALUE: " <<  *value << '\n';
    else std::cout << "DIDNT FIND VALUE\n";


    try{
        std::cout << "VALUE AT KEY " << key << " : " << *map[key] << '\n';
        std::cout << "VALUE AT KEY sd : " << *map["sd"] << '\n';
    } catch (const std::exception &exception) {
        std::cerr << exception.what() << '\n';
    }

    //std::cout << map.size() << '\n';

    map.print();

    // для проверки деструкторов
    {
        Map map_new(map);
        std::cout << '\n' << "MAP COPY: " << '\n';
        map_new.print();

    }

    Map<uint64_t, Car> car_map;

    car_map.insert(123, Car());

    car_map.print();


    return 0;
}
