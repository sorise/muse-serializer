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
    int32_t age = 65;
    bool sex = false;
    double Pi = 3.141592;

    serializer.inputArgs(age, sex, Pi);

    int32_t ageOut = 0;
    bool sexOut = true;
    double piOut = 0.0;

    serializer.outputArgs(ageOut, sexOut, piOut);

    std::cout << "ageOut: " << ageOut << std::endl;
    std::cout << "sexOut: " << sexOut << std::endl;
    std::cout << "piOut: " << piOut << std::endl;

    return 0;
}

