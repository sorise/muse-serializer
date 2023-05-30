#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include "serializer/binarySerializer.h"
#include "serializer/IbinarySerializable.h"
#include <arpa/inet.h>
#include <tuple>

using namespace muse;



int main() {
    muse::BinarySerializer serializer;

    bool sex = false;
    uint16_t age = 25;
    std::list<std::string> names{"remix", "muse", "coco" , "tome", "alice" };
    serializer.input(sex).input(age).input(names);
    //serializer.inputArgs(sex, age, names);

    bool outSex = true;
    serializer.input(outSex);

    std::cout << std::boolalpha << outSex << std::endl;

    return 0;
}

