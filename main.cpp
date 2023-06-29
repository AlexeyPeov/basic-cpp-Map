#include <iostream>

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

    for(int i = 0; i < 25; i++){
        map.insert(random_str(), i);
    }

    std::cout << map.size() << '\n';

//    for (auto &key : map.keys()) {
//        std::cout << key << '\n';
//    }

    map.print();

    // для проверки деструкторов
    {
        Map map_new(map);
        std::cout << "sz: " << map_new.size() << '\n';

    }

    Map<uint64_t, Car> car_map;

    car_map.insert(123, Car());

    car_map.print();


    return 0;
}
