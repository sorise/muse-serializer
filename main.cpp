#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include "serializer/binarySerializer.h"
#include "serializer/IbinarySerializable.h"
#include <arpa/inet.h>
#include <tuple>

using namespace muse::serializer;

int main() {
    BinarySerializer serializer;

    std::array<int , 10>  nums {0,1,2,3,4,5,6,7,8,9};

    for (auto &vl: nums) {
        std::cout <<vl << " ";
    }

    serializer.input(nums);

    std::array<int , 10> values {0};

    std::cout << std::endl;

    serializer.output(values);
    for (auto &vl: values) {
        std::cout <<vl << " ";
    }

//
//    std::tuple<> name;
//    serializer.output(name);
//
//    bool sex = false;
//    uint16_t age = 25;
//    std::list<std::string> names{"remix", "muse", "coco" , "tome", "alice" };
//    serializer.inputArgs(sex, age, names);
//
//    BinaryDeserializeView view(serializer.getBinaryStream(), serializer.byteCount());
//
//    bool outSex = view.output<bool>();
//    uint16_t outAge = view.output<uint16_t>();
//    std::list<std::string> outNames = view.output<std::list<std::string>>();
//
//    for (auto &it: outNames) {
//        std::cout << " - " <<it << std::endl;
//    }
//
//    std::cout << "sex:" << outSex << std::endl;
//    std::cout << "age:" << outAge << std::endl;
    return 0;
}

