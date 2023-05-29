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
//    template<unsigned N>
//    struct TupleReader {
//        template<typename... Args>
//        static TupleReader func(std::tuple<Args...>& t, BinarySerializer& serializer){
//            typename std::tuple_element<N,decltype(t)>::type first;
//            serializer.output(first);
//            std::get<N>(t) = first;
//            TupleReader<N-1>::func();
//        }
//    };
//
//    template<>
//    struct TupleReader<0> {
//        template<typename... Args>
//        static TupleReader func(std::tuple<Args...>& t, BinarySerializer& serializer){
//            typename std::tuple_element<0,decltype(t)>::type first;
//            serializer.output(first);
//            std::get<0>(t) = first;
//        }
//    };

template<unsigned N>
struct power{
    template<class T>
    static T value(T x){
        return x*power<N-1>::value(x);
    }
};
//递归结束条件
template<> struct power<1>{
    template<class T>
    static T value(T x){
        return x;
    }
};


//template<unsigned N>
//struct TupleReader {
//    template<typename... Args>
//    static void func(std::tuple<Args...>& t){
//        typename std::tuple_element<N, std::tuple<Args...>>::type first;
//        std::cout << std::get<N>(t) << std::endl;
//        TupleReader<N-1>::func(t);
//    }
//};
//
//template<> struct TupleReader<0> {
//    template<typename... Args>
//    static void func(std::tuple<Args...>& t){
//        typename std::tuple_element<0, std::tuple<Args...>>::type first;
//        std::cout << std::get<0>(t) << std::endl;
//    }
//};


//template<unsigned N>
//struct TupleWriter {
//    template<typename... Args>
//    static void func(std::tuple<Args...>& t){
//        typename std::tuple_element<N, std::tuple<Args...>>::type first;
//        std::cout << std::get<N>(t) << std::endl;
//        TupleWriter<N-1>::func(t);
//    }
//};
//
//template<> struct TupleWriter<0> {
//    template<typename... Args>
//    static void func(std::tuple<Args...>& t){
//        typename std::tuple_element<0, std::tuple<Args...>>::type first;
//        std::cout << std::get<0>(t) << std::endl;
//    }
//};

//template<> struct BinarySerializer::TupleWriter<0> {
//    template<typename... Args>
//    static void write(std::tuple<Args...>& t, BinarySerializer& serializer){
//        serializer.input(std::get<0>(t));
//    }
//};

int main() {
    muse::BinarySerializer serializer;
    std::tuple<short,int,float> tplOne { 45, 25, 173.5};

    serializer.input(tplOne);

    //TupleReader<std::tuple_size<decltype(tplOne)>::value - 1 >::func(tplOne);

//    std::tuple<std::string,int,float> tplTwo { "muse", 75, 173.5};
//
//    constexpr size_t size_tuple = std::tuple_size<std::tuple<std::string,int,float>>::value;
//
//
//    std::cout << result << std::endl;

    return 0;
}

