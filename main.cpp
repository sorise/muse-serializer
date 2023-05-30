#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include "serializer/binarySerializer.h"
#include "serializer/IbinarySerializable.h"
#include <arpa/inet.h>
#include <tuple>

using namespace muse;

class user: public muse::IBinarySerializable{
private:
    std::string _name;
    uint16_t _age;
public:
    user(): user("",0){};
    explicit user(const std::string& name, const uint16_t& age):_name(name), _age(age){};

    MUSE_IBinarySerializable(_name, _age);

    std::string getName() const{ return _name; };
    uint16_t getAge() const{ return _age; };
    ~user() = default;
};

int main() {
    muse::BinarySerializer serializer;
    user me("remix", 25);

    serializer.inputArgs(me);

    user you("", 18);

    serializer.output(you);
    std::cout << "name: " << you.getName() << std::endl;
    std::cout << "age: " << you.getAge() << std::endl;

    return 0;
}

