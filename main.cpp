#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include "serializer/binarySerializer.h"
#include <arpa/inet.h>
#include <tuple>

using muse::BinarySerializer;

class user{
private:
    std::string _name;
public:
    explicit user(const std::string& name):_name(name){};
    ~user() = default;
    bool operator<(const user& other){
        return _name > other._name;
    }
};

int main() {
    muse::BinarySerializer serializer;

    std::unordered_map<std::string, uint32_t> dic;
    dic["remix"] = 45;
    dic["sorise"] = 74;

    serializer.input(dic);

    std::unordered_map<std::string, uint32_t> dicOut;
    serializer.output(dicOut);

    std::cout << dicOut.size() << std::endl;
    return 0;
}

