#include <iostream>
#include <stdint.h>
#include <vector>
#include <thread>

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

    for(int i = 0; i < 19; i++){
        map.insert(i, random_str());
    }

    map.print();

    std::cout << "\n\n\nBALANCED:\n";

    map.balance();
    map.print();

    Map map_copy = map;

    auto keys_vec = std::vector<int>(map.keys().begin(), map.keys().end());

    size_t vector_size = keys_vec.size();

    int entry = 0;
    for(int i = 0; i < (float)vector_size * 0.8; i++){
        entry = *std::next(map.keys().begin(), rand() % map.keys().size());
        map.remove(entry);

    }
    std::cout << "REMOVED A BUNCH OF STUFF: \n";
    map.print();


    std::cout << "map.remove test " << ((map.size() == (size_t)((float)vector_size * 0.2)) ? "passed" : "not passed") << '\n';

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


    Map threaded_map = map_move;

    auto insert_elements = [](Map<int, std::string> &map, int amount) -> void{

        for (int i = 0; i < amount; i++)
            map.insert(rand() % 995,random_str());

    };

    auto remove_elements = [](Map<int, std::string> &map, int amount) -> void{
        for (int i = 0; i < amount; i++)
            map.remove(*map.keys().begin());
    };

    std::thread thread_1(insert_elements, std::ref(threaded_map), 100);

    std::thread thread_2(remove_elements, std::ref(threaded_map), 25);

    std::thread thread_3(&Map<int, std::string>::print, &threaded_map);


    thread_1.join();
    thread_2.join();
    thread_3.join();

    std::cout << "BALANCED A BIGGER MAP:\n";
    threaded_map.balance();
    threaded_map.print();


    return 0;
}
