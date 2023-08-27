// Created by remix on 23-5-27.

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "serializer/binarySerializer.h"
#include "serializer/binaryDeserializeView.hpp"

TEST_CASE("binarySerializer - bool", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    bool valOne = true;
    bool valTwo = false;

    serializer.input(valOne).input(valTwo);

    bool outOne;
    bool outTwo;
    serializer.output(outOne).output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);
    REQUIRE(serializer.byteCount() == 4);
}

TEST_CASE("binarySerializer - int16", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    int16_t valOne = 45;
    int16_t valTwo = 22;

    serializer.input(valOne).input(valTwo);

    int16_t outOne;
    int16_t outTwo;
    serializer.output(outOne).output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);
    REQUIRE(serializer.byteCount() == 6);
}


TEST_CASE("binarySerializer - int32", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    int32_t valOne = 32;
    int32_t valTwo = 64;

    serializer.input(valOne).input(valTwo);

    int32_t outOne;
    int32_t outTwo;
    serializer.output(outOne);
    serializer.output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);
}

TEST_CASE("binarySerializer - int64", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    int64_t valOne = 48465464;
    int64_t valTwo = 48465442;

    serializer.input(valOne).input(valTwo);

    int64_t outOne;
    int64_t outTwo;

    serializer.output(outOne);
    serializer.output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);
    REQUIRE(serializer.byteCount() == 18);
}


TEST_CASE("binarySerializer - uint32", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    u_int32_t  valOne = 322;
    u_int32_t valTwo = 614;

    serializer.input(valOne).input(valTwo);

    u_int32_t outOne;
    u_int32_t outTwo;
    serializer.output(outOne).output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);

    REQUIRE(serializer.byteCount() == 10);
}

TEST_CASE("binarySerializer - uint64", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    u_int64_t valOne = 845470;
    u_int64_t valTwo = 740000;

    serializer.input(valOne).input(valTwo);

    u_int64_t outOne;
    u_int64_t outTwo;

    serializer.output(outOne).output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);
    REQUIRE(serializer.byteCount() == 18);
}

TEST_CASE("binarySerializer - char", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    char valOne = 32;
    char valTwo = 64;

    serializer.input(valOne).input(valTwo);

    char outOne;
    char outTwo;
    serializer.output(outOne);
    serializer.output(outTwo);

    REQUIRE(valOne == outOne);
    REQUIRE(valTwo == outTwo);
}

TEST_CASE("binarySerializer - string","[binarySerializer]"){
    std::string names { "test file" };
    auto pastLength = names.length();

    muse::serializer::BinarySerializer serializer;
    serializer.input(names);

    std::string outName;
    serializer.output(outName);
    auto nowLength = outName.length();

    REQUIRE(outName == names);
    REQUIRE(pastLength == nowLength);
}


TEST_CASE("binarySerializer - byteCount","[binarySerializer]"){
    uint64_t value = 11845;
    muse::serializer::BinarySerializer serializer;
    serializer.input(value);
    auto size = serializer.byteCount();

    REQUIRE(size == 9);

    std::string message {"0123456789"};
    serializer.input(message); //4
    size = serializer.byteCount();

    REQUIRE(size == 24);
}

TEST_CASE("binarySerializer - const char*","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    char bytes[7] = {0x12,0x11,0x48,0x12,0x19,0x45, -0x41};
    serializer.input(bytes, 7);

    auto size = serializer.byteCount();
    REQUIRE(size == 12);

    char bytesOut[7];
    serializer.output(bytesOut, 7);

    REQUIRE(bytesOut[0] == bytes[0]);
    REQUIRE(bytesOut[2] == bytes[2]);
    REQUIRE(bytesOut[4] == bytes[4]);
    REQUIRE(bytesOut[6] == bytes[6]);
}

TEST_CASE("binarySerializer - const unsigned char*","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    unsigned char bytes[7] = {0x12,0x11,0x98,0x12,0x19,0x45, 0xA1};
    serializer.input(bytes, 7);

    auto size = serializer.byteCount();
    REQUIRE(size == 12);

    unsigned char bytesOut[7];
    serializer.output(bytesOut, 7);

    REQUIRE(bytesOut[0] == bytes[0]);
    REQUIRE(bytesOut[2] == bytes[2]);
    REQUIRE(bytesOut[4] == bytes[4]);
    REQUIRE(bytesOut[6] == bytes[6]);
}


TEST_CASE("binarySerializer - byte count exceed","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    bool isSuccess = true;
    try {
        serializer.input("", UINT32_MAX); //4
    } catch (muse::serializer::BinarySerializerException& ex) {
        isSuccess = false;
    }
    REQUIRE_FALSE(isSuccess);
}


TEST_CASE("binarySerializer - inputArgs", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;

    int32_t age = 65;
    bool sex = false;
    double Pi = 3.141592;

    serializer.inputArgs<int32_t, bool, double>(age, sex, Pi);

    int32_t ageOut = 0;
    bool sexOut = true;
    double PiOut = 0.0;

    REQUIRE(serializer.byteCount() == 16);

    serializer.outputArgs<int32_t, bool, double>(ageOut, sexOut, PiOut);

    REQUIRE(ageOut == age);
    REQUIRE(sexOut == sex);
    REQUIRE(PiOut == Pi);
}

TEST_CASE("binarySerializer - vector<T>","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;

    std::vector<std::string> names {"remix", "coco", "muse", "uix", "lin"};
    std::vector<float> values {57.12,95.15,85.5,99.5};

    serializer.input(names).input(values);

    std::vector<std::string> vNames;
    std::vector<float> vValue;
    serializer.output(vNames).output(vValue);

    for (int i = 0; i < names.size(); ++i) {
        REQUIRE(names[i] == vNames[i] );
    }

    for (int i = 0; i < values.size(); ++i) {
        REQUIRE(values[i] == vValue[i] );
    }
    REQUIRE( serializer.byteCount() == 49 + 25);
}

TEST_CASE("binarySerializer - list<T>","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    std::list<std::string> names {"remix", "coco", "muse", "uix", "lin"};
    std::list<float> values {57.12,95.15,85.5,99.5};

    serializer.input(names).input(values);

    std::list<std::string> vNames;
    std::list<float> vValue;

    serializer.output(vNames).output(vValue);

    auto startOutName = vNames.begin();
    auto startOutValue = vValue.begin();

    for (auto &name:  names) {
        REQUIRE( name == *startOutName);
        startOutName++;
    }

    for (auto &value:  vValue) {
        REQUIRE( value == *startOutValue);
        startOutValue++;
    }
}

TEST_CASE("binarySerializer - map<K,v>","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    std::map<std::string , double> kvs;
    kvs["sd"] = 75.15;
    kvs["sd23"] = 175.15;

    serializer.input(kvs);

    std::map<std::string , double> kvsOut;
    serializer.output(kvsOut);

    REQUIRE( kvsOut["sd"] == kvs["sd"]);
    REQUIRE( kvsOut["sd23"] == kvs["sd23"]);
}


TEST_CASE("binarySerializer - component","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    bool sex = false;
    uint16_t age = 25;
    std::string name {"remix"};
    std::vector<double> scores {95.5, 89.5, 90.5, 97.0};
    std::list<std::string> friends {"muse", "coco", "uni", "tomes"};

    serializer.inputArgs(sex, age, name, scores, friends);

    bool sexOut;
    uint16_t ageOut;
    std::string nameOut;
    std::vector<double> scoresOut;
    std::list<std::string> friendsOut;


    serializer.outputArgs(sexOut, ageOut, nameOut, scoresOut, friendsOut);

    REQUIRE(nameOut == name);
    REQUIRE(friends.front() == friendsOut.front());
}

TEST_CASE("binarySerializer - tuple<...>","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;
    //将元组序列化
    std::tuple<std::string ,int ,float> tplOne { "remix", 25, 173.5};
    serializer.input(tplOne);

    //反序列化
    std::tuple<std::string ,int ,float> tplOneOut;
    serializer.output(tplOneOut);

    REQUIRE(std::get<0>(tplOneOut) == std::get<0>(tplOne));
    REQUIRE(std::get<2>(tplOneOut) == std::get<2>(tplOne));
}

TEST_CASE("binarySerializer - unordered_map<K,V>","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;

    std::unordered_map<std::string, uint32_t> dic;
    dic["remix"] = 45;
    dic["sorise"] = 74;

    serializer.input(dic);

    std::unordered_map<std::string, uint32_t> dicOut;
    serializer.output(dicOut);

    REQUIRE(dicOut.size() == 2);
}

TEST_CASE("binarySerializer - set<T>","[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;

    std::set<std::string> names{"remix", "muse", "coco" , "tome", "alice" };

    serializer.input(names);

    std::set<std::string> namesOut;

    serializer.output(namesOut);

    REQUIRE(namesOut.size() == 5);
}


TEST_CASE("binarySerializer - muse class", "[binarySerializer]"){
    class user: public muse::serializer::IBinarySerializable{
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

    muse::serializer::BinarySerializer serializer;
    user me("remix", 25);

    serializer.inputArgs(me);

    user you("", 18);

    serializer.output(you);

    REQUIRE(you.getName() == me.getName());
    REQUIRE(you.getAge() == me.getAge());
}

TEST_CASE("binarySerializer - save - load", "[binarySerializer]"){
    muse::serializer::BinarySerializer serializer;

    int a = 10;
    serializer.input(a);

    std::list<std::string> names{"remix", "muse", "coco" , "tome", "alice" };
    std::vector<double> scores = {84.01,98.1,15.2,98.2,15.89,84.01,98.1,15.2,98.2,15.89};

    serializer.inputArgs(names, scores);

    auto size = serializer.byteCount();
    const char* streamPtr = serializer.getBinaryStream();

    //存储到文件中
    serializer.saveToFile("./serializer.dat");

    //从文件中加载
    muse::serializer::BinarySerializer loadSerializer;
    loadSerializer.loadFromFile("./serializer.dat");

    int aLoad;
    std::list<std::string> namesLoad;
    std::vector<double> scoresLoad;

    loadSerializer.outputArgs(aLoad, namesLoad, scoresLoad);

    REQUIRE( aLoad == 10 );
    REQUIRE( scoresLoad[0] == scores[0]);
}


TEST_CASE("BinaryDeserializeView", "basic"){
    muse::serializer::BinarySerializer serializer;

    std::tuple<> name;
    serializer.output(name);

    bool sex = false;
    uint16_t age = 25;
    std::list<std::string> names{"remix", "muse", "coco" , "tome", "alice" };
    serializer.inputArgs(sex, age, names);

    muse::serializer::BinaryDeserializeView view(serializer.getBinaryStream(), serializer.byteCount());

    bool outSex = view.output<bool>();
    uint16_t outAge = view.output<uint16_t>();
    std::list<std::string> outNames = view.output<std::list<std::string>>();

    REQUIRE( outSex == sex  );
    REQUIRE( outAge == age  );
}

