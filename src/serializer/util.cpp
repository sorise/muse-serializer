//
// Created by remix on 23-5-27.
//
#include "serializer/util.h"

namespace muse{

    ByteSequence getByteSequence(){
        int32_t testValue = 0x01234567;
        char bytes[sizeof(int32_t)];
        std::memcpy(bytes, &testValue, sizeof(int32_t));
        if (bytes[0] == 0x01){
            return ByteSequence::BigEndian;
        }
        return ByteSequence::LittleEndian;
    }

    SerializerException::SerializerException(const std::string &arg, ErrorNumber number)
    :logic_error(arg), errorNumber(number) {}

    SerializerException::~SerializerException() = default;
}