//
// Created by remix on 23-7-11.
//

#ifndef SERIALIZER_MUSE_SERIALIZER_HPP
#define SERIALIZER_MUSE_SERIALIZER_HPP
#include <iostream>
#include <cstring>
#include <vector>
#include <exception>
#include <tuple>
#include <algorithm>
#include <climits>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint>

namespace muse::serializer{

    static_assert(__cplusplus >= 201103L, "C++ version is not right" );

    #define MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()        \
    if (byteSequence == ByteSequence::BigEndian){       \
        auto last = &byteStream[byteStream.size()];     \
        auto first = last - sizeof(uint32_t);           \
        std::reverse(first, last);                      \
    }

    #define MUSE_PREVENT_BOUNDARY() \
    if (readPosition == byteStream.size()){ \
        throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory); \
    }

    //初始化容量大小
    #define MUSE_DEFAULT_CAPACITY 32
    //字符串的最大长度, 1MB 等于 1048576 字节， 0.5M 524288
    #define MUSE_MAX_STRING_LENGTH 1048576
    //Byte 长度 上限
    #define MUSE_MAX_BYTE_COUNT 65536
    //Vector 长度 数量上限
    #define MUSE_MAX_VECTOR_COUNT 16777216
    //List 长度 数量上限
    #define MUSE_MAX_LIST_COUNT 16777216
    //Map 健值对 数量上限
    #define MUSE_MAX_MAP_COUNT 16777216
    //Tuple 数量上限
    #define MUSE_MAX_TUPLE_COUNT 64
    //SET 成员数量上限
    #define MUSE_MAX_SET_COUNT 16777216

    /*
     *  type: enum class BinaryDataType : char
     *  des: 支持序列化的数据类型
     *  大小必须是一个字节
     */
    enum class BinaryDataType : char
    {
        BOOL = 0,   //bool
        INT16,      //short
        INT32,      //int
        INT64,      //long
        UINT16,     //unsigned short
        UINT32,     //unsigned int
        UINT64,     //unsigned long
        FLOAT,      //float 浮点数
        DOUBLE,     //double 双精度浮点数
        CHAR,       //char 字符
        STRING,     //string 字服串
        BYTES,      //char[] 纯粹的二进制写入
        UBYTES,     //unsigned char[] 纯粹的二进制写入
        VECTOR,     //顺序表  std::vector<T>
        LIST,       //链表   std::list<T>
        MAP,        //字典 - 红黑树 map
        TUPLE,      //元组 - tuple
        HASHMAP,    //字典 - Hash map
        SET,        //集合 - Set
        MUSECLASS   //用户自定义类型
    };

    enum class ByteSequence: short
    {
        BigEndian = 0,      /* 大端序 网络字节序 */
        LittleEndian = 1,   /* 小端序 主机字节序 */
    };

    /* 错误号 */
    enum class ErrorNumber: short {
        DataTypeError = 0,                  /* 读取类型错误 */
        InsufficientRemainingMemory,        /* 剩余空间不足 */
        TheStringLengthExceedsTheLimit,     /* 字符串长度超过 1048576 */
        IllegalStringLength,                /* 字符串长度非法 1048576*/
        TheBytesCountExceedsTheLimit,       /* 字节数量超过 65536 */
        IllegalBytesCount,                  /* 字节数量非法 */
        ReadIllegalBytesCount,              /* 你存储了X个字节，但是读取了Y个， (X！=Y) */
        TheVectorCountExceedsTheLimit,      /* Vector长度超过 16777216   */
        IllegalVectorCount,                 /* Vector数量非法 */
        TheListCountExceedsTheLimit,        /* List长度超过 16777216   */
        IllegalListCount,                   /* List数量非法 */
        TheMapKVCountExceedsTheLimit,       /* Map长度超过 16777216   */
        IllegalMapKVCount,                  /* Map数量非法 */
        TheTupleCountExceedsTheLimit,       /* Tuple长度超过 64   */
        IllegalTupleCount,                  /* Tuple数量非法 */
        IllegalParameterTupleCount,         /* 传递的Tuple 元素数量不一致 */
        TheHashMapKVCountExceedsTheLimit,   /* HashMap长度超过 16777216   */
        IllegalHashMapKVCount,              /* HashMap数量非法 */
        ErrorReadingSubElements,            /* 读取子元素失败 */
        TheSetSizeExceedsTheLimit,          /* Set 成员数量超过 16777216   */
        IllegalSetCount,                    /* HashMap数量非法 */
        NoDataToStoreInFile,                /* 没有数据可以存储到文件中 */
    };

    /* 获取当前主机采用的字节序 */
    ByteSequence getByteSequence();

    //获取元组长度 01
    template<typename T>
    auto getTupleElementCount(T tp) -> decltype(std::tuple_size<T>::value) {
        return std::tuple_size<decltype(tp)>::value;
    }

    //获取元组长度 02
    template<typename ...Args>
    auto getTupleElementCount(std::tuple<Args...> tp) -> decltype(std::tuple_size<std::tuple<Args...>>::value) {
        return std::tuple_size<decltype(tp)>::value;
    }

    /* 错误异常消息 */
    class BinarySerializerException: public std::logic_error{
    public:
        explicit BinarySerializerException(const std::string &arg, ErrorNumber err);
        ~BinarySerializerException() override ;
        ErrorNumber getErrorNumber(); //返回错误号
    private:
        ErrorNumber errorNumber;
    };

    class BinarySerializer;
    //用户自定义类型必须具有
    class IBinarySerializable{
    public:
        virtual void serialize(BinarySerializer &serializer) const = 0;
        virtual void deserialize(BinarySerializer &serializer) = 0;
    };


    class IBinarySerializable; //define

    class BinarySerializer final{
    public:
        using Position_Type = long;
    private:
        template<unsigned int N>
        struct TupleWriter {
            template<typename... Args>
            static void write(const std::tuple<Args...>& t, BinarySerializer& serializer){
                serializer.input(std::get<N>(t));
                TupleWriter<N-1>::write(t, serializer);
            }
        };

        template<unsigned int N>
        struct TupleReader{
            template<typename... Args>
            static void read(std::tuple<Args...>& t, BinarySerializer& serializer){
                //typename std::tuple_element<N,std::tuple<Args...>>::type first;
                serializer.output(std::get<N>(t));
                TupleReader<N-1>::read(t, serializer);
            }
        };

        /* 存储字节流数据 */
        std::vector<char> byteStream;
        /* 读取指针 */
        Position_Type readPosition;
        /* 字节序 */
        ByteSequence byteSequence;
        /* 如果剩余空间不够了，就进行扩容，否则直接返回 */
        void expansion(size_t len);
        /* 读取 */
        bool read(char* data, const unsigned int& len) noexcept ;
    public:
        /* 最基础的写入方法 */
        BinarySerializer& write(const char* data, const unsigned int& len);
        using Container_type = std::vector<char>;
        void clear();                            //清除所有
        void reset();                            //重新从开始读取
        Container_type::size_type byteCount() const;                //多少字节数量
        Position_Type getReadPosition() const;                      //获得当前指针所在位置
        bool checkDataTypeRightForward(BinaryDataType type);              //检查类型是否正确，如果正确，移动 readPosition
        BinarySerializer();                                         //默认构造函数
        BinarySerializer(const BinarySerializer& other) = delete;   //禁止复制
        BinarySerializer(BinarySerializer &&other) noexcept;        //支持移动操作
        const char* getBinaryStream() const;                        //返回二进制流的指针

        void saveToFile(const std::string& path) const;    //二进制数据存储到文件中
        void loadFromFile(const std::string& path);  //从文件中加载二进制数据

        ~BinarySerializer();

        BinarySerializer& input(const bool&);
        BinarySerializer& input(const char &);
        BinarySerializer& input(const int16_t &);
        BinarySerializer& input(const int32_t &);
        BinarySerializer& input(const int64_t &);
        BinarySerializer& input(const uint16_t &);
        BinarySerializer& input(const uint32_t &);
        BinarySerializer& input(const uint64_t &);
        BinarySerializer& input(const float &);
        BinarySerializer& input(const double &);
        BinarySerializer& input(const std::string &);
        BinarySerializer& input(const char*, unsigned int);
        BinarySerializer& input(const unsigned char*, unsigned int);
        BinarySerializer& input(const muse::serializer::IBinarySerializable &serializable);
        BinarySerializer& input(const std::tuple<> & tpl);

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& input(const std::vector<T>& value){
            auto type = BinaryDataType::VECTOR;
            //写入类型
            write((char*)&type, sizeof(type));
            //获得成员数量,不能超过某个限制
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_VECTOR_COUNT){
                throw BinarySerializerException("the vector count exceeds the limit", ErrorNumber::TheVectorCountExceedsTheLimit);
            }
            //写入成员数量
            write((char*)&elementCount, sizeof(uint32_t));
            //存储长度, 注意大小端问题
            MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()
            //写入成员
            for (auto it = value.begin(); it != value.end(); it++)
            {
                input(*it);
            }
            return *this;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& input(const std::list<T> &value){
            auto type = BinaryDataType::LIST;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_LIST_COUNT){
                throw BinarySerializerException("the list count exceeds the limit", ErrorNumber::TheListCountExceedsTheLimit);
            }
            //写入成员数量
            write((char*)&elementCount, sizeof(uint32_t));
            //存储长度, 注意大小端问题
            MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()
            //写入成员
            for (auto it = value.begin(); it != value.end(); it++)
            {
                input(*it);
            }
            return *this;
        }

        template<typename K, typename V,
                typename = typename std::enable_if_t<std::is_default_constructible_v<K>>,
                typename = typename std::enable_if_t<std::is_default_constructible_v<V>>
        >
        BinarySerializer& input(const std::map<K,V> &value){
            auto type = BinaryDataType::MAP;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_MAP_COUNT){
                throw BinarySerializerException("the map key-value pairs count exceeds the limit", ErrorNumber::TheMapKVCountExceedsTheLimit);
            }
            //写入成员数量
            write((char*)&elementCount, sizeof(uint32_t));
            //存储长度, 注意大小端问题
            MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()
            for (auto kv = value.begin();  kv!=value.end(); kv++)
            {
                input(kv->first);
                input(kv->second);
            }
            return *this;
        }

        template<typename K, typename V,
                typename = typename std::enable_if_t<std::is_default_constructible_v<K>>,
                typename = typename std::enable_if_t<std::is_default_constructible_v<V>>
        >
        BinarySerializer& input(const std::unordered_map<K,V> &value){
            auto type = BinaryDataType::HASHMAP;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_MAP_COUNT){
                throw BinarySerializerException("the hash map key-value pairs count exceeds the limit", ErrorNumber::TheHashMapKVCountExceedsTheLimit);
            }
            //写入成员数量
            write((char*)&elementCount, sizeof(uint32_t));
            MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()
            for (auto kv = value.begin();  kv!=value.end(); kv++)
            {
                input(kv->first);
                input(kv->second);
            }
            return *this;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& input(const std::set<T> &value){
            auto type = BinaryDataType::SET;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_LIST_COUNT){
                throw BinarySerializerException("the set count exceeds the limit", ErrorNumber::TheSetSizeExceedsTheLimit);
            }
            //写入成员数量
            write((char*)&elementCount, sizeof(uint32_t));
            //存储长度, 注意大小端问题
            MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()
            //写入成员
            for (auto it = value.begin(); it != value.end(); it++)
            {
                input(*it);
            }
            return *this;
        }

        template<typename... Args>
        BinarySerializer &input(const std::tuple<Args...>& tpl) {
            auto type = BinaryDataType::TUPLE;
            //写入类型
            write((char*)&type, sizeof(type));
            //获得长度
            uint16_t tupleSize = getTupleElementCount(tpl);
            if (tupleSize > MUSE_MAX_TUPLE_COUNT){
                throw BinarySerializerException("the tuple size exceeds the limit", ErrorNumber::TheTupleCountExceedsTheLimit);
            }
            //写入长度
            write((char*)&tupleSize, sizeof(uint16_t));
            //防止大小端问题
            if (byteSequence == ByteSequence::BigEndian){
                auto last = &byteStream[byteStream.size()];
                auto first = last - sizeof(uint16_t);
                std::reverse(first, last);
            }
            constexpr size_t N = sizeof...(Args);
            TupleWriter<N - 1>::write(tpl,*this);
            return *this;
        }

        template<typename T, typename... Args>
        BinarySerializer& inputArgs(const T& value, const Args&... args) {
            input(value);
            inputArgs(args...);
            return *this;
        };
        BinarySerializer& inputArgs() { return *this; };

        /* 防止无端的类型转换错误 */
        BinarySerializer& input(const char*) = delete;
        BinarySerializer& input(char*) = delete;

        //反序列化
        BinarySerializer& output(bool &);
        BinarySerializer& output(char &);
        BinarySerializer& output(int16_t &);
        BinarySerializer& output(int32_t &);
        BinarySerializer& output(int64_t &);
        BinarySerializer& output(u_int16_t &);
        BinarySerializer& output(u_int32_t &);
        BinarySerializer& output(u_int64_t &);
        BinarySerializer& output(float &);
        BinarySerializer& output(double &);
        BinarySerializer& output(std::string &);
        BinarySerializer& output(char * , unsigned int ); /* 可能存在有复制消耗*/
        BinarySerializer& output(unsigned char * , unsigned int ); /* 可能存在有复制消耗*/
        BinarySerializer& output(std::tuple<> & tpl);

        template<class T>
        T output(){
            T t;
            this->output(t);
            return t;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& output(std::vector<T>& value)
        {
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)BinaryDataType::VECTOR)
                throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);

            auto defaultPosition = readPosition;
            value.clear(); //先清除数据
            //读取长度信息
            readPosition++;
            //获取 vector 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
            }
            //读取字符串长度 大小端处理
            auto vectorSize = *((uint32_t *)(&byteStream[readPosition]));
            if (byteSequence == ByteSequence::BigEndian)
            {
                char* first = (char*)&vectorSize;
                char* last = first + sizeof(uint32_t);
                std::reverse(first, last);
            }
            //检测长度是否非法
            if (vectorSize > MUSE_MAX_VECTOR_COUNT){
                readPosition = defaultPosition;
                throw BinarySerializerException("illegal vector count" , ErrorNumber::IllegalVectorCount);
            }
            //检测剩余空间是否足够
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i =0; i < vectorSize;i++)
                {
                    T v;
                    output(v);
                    value.emplace_back(v);
                }
            }catch(BinarySerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw BinarySerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& output(std::list<T>& value){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)BinaryDataType::LIST)
                throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            value.clear(); //先清除数据
            //读取长度信息
            readPosition++;
            //获取 list 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
            }
            //读取字符串长度 大小端处理
            auto listSize = *((uint32_t *)(&byteStream[readPosition]));
            if (byteSequence == ByteSequence::BigEndian)
            {
                char* first = (char*)&listSize;
                char* last = first + sizeof(uint32_t);
                std::reverse(first, last);
            }
            //检测长度是否非法
            if (listSize > MUSE_MAX_LIST_COUNT){
                readPosition = defaultPosition;
                throw BinarySerializerException("illegal list count" , ErrorNumber::IllegalListCount);
            }
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i = 0; i < listSize;i++)
                {
                    T v;
                    output(v);
                    value.emplace_back(v);
                }
            }catch(BinarySerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw BinarySerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& output(std::set<T>& value){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)BinaryDataType::SET)
                throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            value.clear();
            //读取长度信息
            readPosition++;
            //获取 list 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
            }
            //读取字符串长度 大小端处理
            auto setSize = *((uint32_t *)(&byteStream[readPosition]));
            if (byteSequence == ByteSequence::BigEndian)
            {
                char* first = (char*)&setSize;
                char* last = first + sizeof(uint32_t);
                std::reverse(first, last);
            }
            //检测长度是否非法
            if (setSize > MUSE_MAX_SET_COUNT){
                readPosition = defaultPosition;
                throw BinarySerializerException("illegal set count" , ErrorNumber::IllegalSetCount);
            }
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i = 0; i < setSize;i++)
                {
                    T v;
                    output(v);
                    value.emplace(v);
                }
            }catch(BinarySerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw BinarySerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename K, typename V,
                typename = typename std::enable_if_t<std::is_default_constructible_v<K>>,
                typename = typename std::enable_if_t<std::is_default_constructible_v<V>>
        >
        BinarySerializer& output(std::map<K,V> &value){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)BinaryDataType::MAP)
                throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            readPosition++;
            value.clear();
            //获取 list 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
            }
            //读取字符串长度 大小端处理
            auto mapKVSize = *((uint32_t *)(&byteStream[readPosition]));
            if (byteSequence == ByteSequence::BigEndian)
            {
                char* first = (char*)&mapKVSize;
                char* last = first + sizeof(uint32_t);
                std::reverse(first, last);
            }
            //检测长度是否非法
            if (mapKVSize > MUSE_MAX_MAP_COUNT){
                readPosition = defaultPosition;
                throw BinarySerializerException("illegal map k-v pair count" , ErrorNumber::IllegalMapKVCount);
            }
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i = 0; i < mapKVSize;i++)
                {
                    K k;  //必须具备默认构造函数
                    V v;  //必须具备默认构造函数
                    output(k);
                    output(v);
                    value[k] = v;
                }
            }catch(BinarySerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw BinarySerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename K, typename V,
                typename = typename std::enable_if_t<std::is_default_constructible_v<K>>,
                typename = typename std::enable_if_t<std::is_default_constructible_v<V>>
        >
        BinarySerializer& output(std::unordered_map<K,V> &value){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)BinaryDataType::HASHMAP)
                throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            readPosition++;
            value.clear();
            //获取 hashmap 数量
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
            }
            //读取字符串长度 大小端处理
            auto mapKVSize = *((uint32_t *)(&byteStream[readPosition]));
            if (byteSequence == ByteSequence::BigEndian)
            {
                char* first = (char*)&mapKVSize;
                char* last = first + sizeof(uint32_t);
                std::reverse(first, last);
            }
            //检测长度是否非法
            if (mapKVSize > MUSE_MAX_MAP_COUNT){
                readPosition = defaultPosition;
                throw BinarySerializerException("illegal hash map k-v pair count" , ErrorNumber::IllegalHashMapKVCount);
            }
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i = 0; i < mapKVSize;i++)
                {
                    K k;  //必须具备默认构造函数
                    V v;  //必须具备默认构造函数
                    output(k);
                    output(v);
                    value[k] = v;
                }
            }catch(BinarySerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw BinarySerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename T, typename... Args>
        BinarySerializer& outputArgs(T& value, Args&... args){
            output(value);
            outputArgs(args...);
            return *this;
        }
        BinarySerializer& outputArgs() { return *this; };

        template<typename... Args>
        BinarySerializer& output(std::tuple<Args...>& tpl){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)BinaryDataType::TUPLE)
                throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            readPosition++;
            //获取 tuple 元素个数
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint16_t)){
                readPosition = defaultPosition;
                throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
            }
            auto tupleSize = *((uint16_t *)(&byteStream[readPosition]));
            //处理大小端
            if (byteSequence == ByteSequence::BigEndian)
            {
                char* first = (char*)&tupleSize;
                char* last = first + sizeof(uint16_t);
                std::reverse(first, last);
            }
            //检测长度是否非法
            if (tupleSize > MUSE_MAX_TUPLE_COUNT){
                readPosition = defaultPosition;
                throw BinarySerializerException("illegal tuple count" , ErrorNumber::IllegalTupleCount);
            }
            constexpr size_t N = sizeof...(Args);
            if (tupleSize != N){
                readPosition = defaultPosition;
                throw BinarySerializerException("tuple count not match" , ErrorNumber::IllegalParameterTupleCount);
            }
            readPosition += sizeof(uint16_t);
            try {
                TupleReader<N - 1>::read(tpl,*this);
            }catch(BinarySerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw BinarySerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }
        BinarySerializer &output(muse::serializer::IBinarySerializable &serializable);

    };

    template<>
    struct BinarySerializer::TupleWriter<0> {
        template<typename... Args>
        static void write(const std::tuple<Args...>& t, BinarySerializer& serializer){
            serializer.input(std::get<0>(t));
        }
    };

    template<>
    struct BinarySerializer::TupleReader<0>{
        template<typename... Args>
        static void read(std::tuple<Args...>& t, BinarySerializer& serializer){
            //typename std::tuple_element<0,decltype(t)>::type first;
            serializer.output(std::get<0>(t));
        }
    };

    #define MUSE_IBinarySerializable(...)   \
    void serialize(muse::serializer::BinarySerializer &serializer) const override{     \
        auto type = muse::serializer::BinaryDataType::MUSECLASS;                             \
        serializer.write((char*)&type, sizeof(type));                \
        serializer.inputArgs(__VA_ARGS__);                           \
    };                                                               \
                                                                     \
    void deserialize(muse::serializer::BinarySerializer &serializer)  override {    \
        auto readPosition = serializer.getReadPosition();               \
        if (readPosition == serializer.byteCount()){                    \
            throw muse::serializer::BinarySerializerException("remaining memory is not enough ", muse::serializer::ErrorNumber::InsufficientRemainingMemory); \
        }                                                                                                                       \
        auto result = serializer.checkDataTypeRightForward(muse::serializer::BinaryDataType::MUSECLASS);                                          \
        if (!result) throw muse::serializer::BinarySerializerException("read type error", muse::serializer::ErrorNumber::DataTypeError);                      \
        serializer.outputArgs(__VA_ARGS__);                                                                                     \
    }


    /* 获取当前主机采用的字节序 */
    ByteSequence getByteSequence(){
        int32_t testValue = 0x01234567;
        char bytes[sizeof(int32_t)];
        std::memcpy(bytes, &testValue, sizeof(int32_t));
        if (bytes[0] == 0x01){
            return ByteSequence::BigEndian;
        }
        return ByteSequence::LittleEndian;
    }

    BinarySerializerException::BinarySerializerException(const std::string &arg, ErrorNumber number)
            :logic_error(arg), errorNumber(number) {}

    ErrorNumber BinarySerializerException::getErrorNumber() {
        return errorNumber;
    }

    BinarySerializerException::~BinarySerializerException() = default;


    /* 进行反序列合法性检测 */
    #define MUSE_CHECK_LEGITIMACY(TYPENAME, realName)                            \
    auto needLength = sizeof(BinaryDataType::TYPENAME) + sizeof(realName );        \
    if((byteStream.size() - readPosition) < needLength )                     \
        throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory); \
                                                                                    \
    if (byteStream[readPosition] != (char)BinaryDataType::TYPENAME)                       \
        throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);   \


    /* 反序列化过程化中：如果主机是大端序，则转换为大端序 */
    #define MUSE_CONVERT_TO_BIG_ENDIAN(realDataType)            \
    if (byteSequence == ByteSequence::BigEndian)            \
    {                                                       \
        char* first = (char*)&value;                        \
        char* last = first + sizeof(realDataType);          \
        std::reverse(first, last);                          \
    }                                                       \
    readPosition += static_cast<int>(sizeof(realDataType));


    /* 序列化过程化中：如果主机是大端序，则转换为小端序 */
    #define MUSE_CONVERT_TO_LITTLE_ENDIAN()                 \
    write((char*)&type, sizeof(type));                  \
    write((char*)&value, (int)dataTypeSize);            \
    if (byteSequence == ByteSequence::BigEndian){       \
        auto last = &byteStream[byteStream.size()];     \
        auto first = last - dataTypeSize;               \
        std::reverse(first, last);                      \
    }


    #define MUSE_PREVENT_CROSSING_BOUNDARIES()              \
    if (readPosition == byteStream.size()){             \
        throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory); \
    }

    BinarySerializer::BinarySerializer():readPosition(0){
        byteStream.reserve(MUSE_DEFAULT_CAPACITY); //先给一个初始容量 32 字节
        byteSequence = getByteSequence();
    };
    BinarySerializer::~BinarySerializer()  = default;

    BinarySerializer::BinarySerializer(BinarySerializer &&other) noexcept  {
        byteStream = std::move(other.byteStream); //容器必须支持移动
        readPosition = other.readPosition;
        byteSequence = other.byteSequence;

        other.readPosition = 0;
    }

    void BinarySerializer::expansion(size_t len){
        if (byteStream.capacity() - byteStream.size() < len ){
            auto hasSize= byteStream.size();
            auto cap = byteStream.capacity();
            while ((cap - hasSize) < len){
                cap = cap * 2;
            };
            byteStream.reserve(cap);
        }
    }

    BinarySerializer& BinarySerializer::write(const char *data, const unsigned int& len) {
        if (len > 0){
            expansion(len); //先扩容
            auto length = byteStream.size();   //长度
            byteStream.resize(length + len);  //重新设置长度
            std::memcpy((char *)&byteStream[length], data, len); //内存拷贝
        }
        return *this;
    }

    bool BinarySerializer::read(char *data, const unsigned int &len) noexcept{
        if (readPosition + len > byteStream.size()){
            return false;
        }
        std::memcpy(data, byteStream.data() + readPosition, len);
        readPosition += (int)len;
        return true;
    }

    void BinarySerializer::clear() {
        byteStream.clear();
        byteStream.reserve(MUSE_DEFAULT_CAPACITY);
        readPosition = 0;
    }

    void BinarySerializer::reset() {
        readPosition = 0;
    }

    BinarySerializer::Container_type::size_type BinarySerializer::byteCount() const{
        return byteStream.size();
    }

    BinarySerializer& BinarySerializer::input(const bool &value) {
        auto type = BinaryDataType::BOOL;
        write((char*)&type, sizeof(type)); //类型
        write((char*)&value, sizeof(char)); //值
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const char & value) {
        auto type = BinaryDataType::CHAR;
        write((char*)&type, sizeof(type)); //类型
        write((char*)&value, sizeof(char)); //值
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const int16_t & value) {
        auto type = BinaryDataType::INT16;
        auto dataTypeSize = sizeof(int16_t);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const int32_t & value) {
        auto type = BinaryDataType::INT32;
        auto dataTypeSize = sizeof(int32_t);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const int64_t & value) {
        auto type = BinaryDataType::INT64;
        auto dataTypeSize = sizeof(int64_t);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const uint16_t & value) {
        auto type = BinaryDataType::UINT16;
        auto dataTypeSize = sizeof(uint16_t);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const uint32_t & value) {
        auto type = BinaryDataType::UINT32;
        auto dataTypeSize = sizeof(uint32_t);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const uint64_t & value) {
        auto type = BinaryDataType::UINT64;
        auto dataTypeSize = sizeof(uint64_t);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const float & value) {
        auto type = BinaryDataType::FLOAT;
        auto dataTypeSize = sizeof(float);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const double & value) {
        auto type = BinaryDataType::DOUBLE;
        auto dataTypeSize = sizeof(double);
        MUSE_CONVERT_TO_LITTLE_ENDIAN()
        return *this;
    }

    BinarySerializer& BinarySerializer::input(const std::tuple<> & tpl){
        return *this;
    }

    BinarySerializer &BinarySerializer::input(const std::string & value) {
        uint32_t len = value.length();              //存储字符串长度
        //字符串长度不能超过  MUSE_MAX_STRING_LENGTH 规定
        if (len > MUSE_MAX_STRING_LENGTH){
            throw BinarySerializerException("the string length exceeds the limit", ErrorNumber::TheStringLengthExceedsTheLimit);
        }
        auto type = BinaryDataType::STRING;
        write((char*)&type, sizeof(type)); //类型
        //存储长度
        write((char*)&len, sizeof(uint32_t));
        //存储长度
        if (byteSequence == ByteSequence::BigEndian){
            auto last = &byteStream[byteStream.size()];
            auto first = last - sizeof(uint32_t);
            std::reverse(first, last);
        }
        //存储数据
        write(value.c_str(), (int)len);
        return *this;
    }

    /*  BYTES
     * [  1byte 类型  ][   4byte 字节长度   ][ 存储字节数据....  ]
     */
    BinarySerializer &BinarySerializer::input(const char * value, unsigned int len) {
        if (len > MUSE_MAX_BYTE_COUNT){
            throw BinarySerializerException("the bytes count exceeds the limit", ErrorNumber::TheBytesCountExceedsTheLimit);
        }
        auto type = BinaryDataType::BYTES;
        write((char*)&type, sizeof(type)); //类型
        //存储长度
        write((char*)&len, sizeof(uint32_t));
        //存储长度 注意大小端问题
        if (byteSequence == ByteSequence::BigEndian){
            auto last = &byteStream[byteStream.size()];
            auto first = last - sizeof(uint32_t);
            std::reverse(first, last);
        }
        //写入数据
        write((char*)value, len); //值
        return *this;
    }

    /*  UBYTES
     * [  1byte 类型  ][   4byte 字节长度   ][ 存储字节数据....  ]
     */
    BinarySerializer &BinarySerializer::input(const unsigned char * value, unsigned int len) {
        if (len > MUSE_MAX_BYTE_COUNT){
            throw BinarySerializerException("the bytes count exceeds the limit", ErrorNumber::TheBytesCountExceedsTheLimit);
        }
        auto type = BinaryDataType::UBYTES;
        write((char*)&type, sizeof(type)); //类型
        //存储长度
        write((char*)&len, sizeof(uint32_t));
        //存储长度, 注意大小端问题
        if (byteSequence == ByteSequence::BigEndian){
            auto last = &byteStream[byteStream.size()];
            auto first = last - sizeof(uint32_t);
            std::reverse(first, last);
        }
        //写入数据
        write(( char*)value, len); //值
        return *this;
    }

    BinarySerializer& BinarySerializer::output(bool & value) {
        MUSE_CHECK_LEGITIMACY(BOOL,bool)
        value = (bool)byteStream[++readPosition];
        ++readPosition;
        return *this;
    }

    BinarySerializer& BinarySerializer::output(char & value) {
        MUSE_CHECK_LEGITIMACY(CHAR,char)
        value = (char)byteStream[++readPosition];
        ++readPosition;
        return *this;
    }

    BinarySerializer& BinarySerializer::output(int16_t & value) {
        MUSE_CHECK_LEGITIMACY(INT16,int16_t)
        value = *((int16_t *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(int16_t)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(int32_t & value) {
        MUSE_CHECK_LEGITIMACY(INT32,int32_t)
        value = *((int32_t *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(int32_t)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(int64_t & value) {
        MUSE_CHECK_LEGITIMACY(INT64,int64_t)
        value = *((int64_t *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(int64_t)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(u_int16_t & value) {
        MUSE_CHECK_LEGITIMACY(UINT16,u_int16_t)
        value = *((u_int16_t *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(u_int16_t)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(u_int32_t & value) {
        MUSE_CHECK_LEGITIMACY(UINT32,u_int32_t)
        value = *((u_int32_t *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(u_int32_t)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(u_int64_t & value) {
        MUSE_CHECK_LEGITIMACY(UINT64,u_int64_t)
        value = *((u_int64_t *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(u_int64_t)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(float & value) {
        MUSE_CHECK_LEGITIMACY(FLOAT,float)
        value = *((float *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(float)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(double & value) {
        MUSE_CHECK_LEGITIMACY(DOUBLE,double)
        value = *((double *)(&byteStream[++readPosition]));
        //如果主机是大端序 将其转换为大端序列
        MUSE_CONVERT_TO_BIG_ENDIAN(double)
        return *this;
    }

    BinarySerializer& BinarySerializer::output(std::tuple<> & tpl){
        return *this;
    }

    BinarySerializer& BinarySerializer::output(std::string & str) {
        //防止访问越界
        MUSE_PREVENT_CROSSING_BOUNDARIES()
        auto defaultPosition = readPosition;
        //类型检测
        if (byteStream[readPosition] != (char)BinaryDataType::STRING)
            throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
        //剩余空间检测
        readPosition++;
        auto leftSize =  byteStream.size() - readPosition;
        if (leftSize < sizeof(uint32_t)){
            readPosition = defaultPosition;
            throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
        }
        //字符串长度,大小端处理
        auto stringLength = *((uint32_t *)(&byteStream[readPosition]));
        if (byteSequence == ByteSequence::BigEndian)
        {
            char* first = (char*)&stringLength;
            char* last = first + sizeof(uint32_t);
            std::reverse(first, last);
        }
        //检测长度是否非法
        if (stringLength > MUSE_MAX_STRING_LENGTH){
            readPosition = defaultPosition;
            throw BinarySerializerException("illegal string length" , ErrorNumber::IllegalStringLength);
        }
        //检测剩余空间是否足够
        readPosition += sizeof(uint32_t);
        leftSize =  byteStream.size() - defaultPosition;
        if (leftSize < stringLength){
            readPosition = defaultPosition;
            throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
        }
        //正确读取
        str.assign((char*)&byteStream[readPosition], stringLength);
        readPosition += static_cast<int>(stringLength); //移动
        return *this;
    }

    BinarySerializer& BinarySerializer::output(char *value, unsigned int len) {
        //防止初次访问越界
        MUSE_PREVENT_CROSSING_BOUNDARIES()
        auto defaultPosition = readPosition;
        //类型检测，可能会有越界错误
        if (byteStream[readPosition] != (char)BinaryDataType::BYTES)
            throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
        //读取长度信息
        readPosition++;
        auto leftSize =  byteStream.size() - readPosition;
        if (leftSize < sizeof(uint32_t)){
            readPosition = defaultPosition;
            throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
        }
        //字符串长度 大小端处理
        auto stringLength = *((uint32_t *)(&byteStream[readPosition]));
        if (byteSequence == ByteSequence::BigEndian)
        {
            char* first = (char*)&stringLength;
            char* last = first + sizeof(uint32_t);
            std::reverse(first, last);
        }
        //检测长度是否非法
        if (stringLength > MUSE_MAX_BYTE_COUNT){
            readPosition = defaultPosition;
            throw BinarySerializerException("illegal bytes count" , ErrorNumber::IllegalBytesCount);
        }
        if (stringLength != len){
            throw BinarySerializerException("read illegal bytes count" , ErrorNumber::ReadIllegalBytesCount);
        }
        //检测剩余空间是否足够
        readPosition += sizeof(uint32_t);
        leftSize =  byteStream.size() - defaultPosition;
        if (leftSize < stringLength){
            readPosition = defaultPosition;
            throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
        }
        std::memcpy(value,(char*)&byteStream[readPosition], stringLength);
        readPosition += static_cast<int>(stringLength);
        return *this;
    }

    BinarySerializer& BinarySerializer::output(unsigned char *value, unsigned int len) {
        //防止初次访问越界
        MUSE_PREVENT_CROSSING_BOUNDARIES()
        auto defaultPosition = readPosition;
        //类型检测
        if (byteStream[readPosition] != (char)BinaryDataType::UBYTES)
            throw BinarySerializerException("read type error", ErrorNumber::DataTypeError);
        //读取长度信息
        readPosition++;
        auto leftSize =  byteStream.size() - readPosition;
        if (leftSize < sizeof(uint32_t)){
            readPosition = defaultPosition;
            throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
        }
        //字符串长度 大小端处理
        auto stringLength = *((uint32_t *)(&byteStream[readPosition]));
        if (byteSequence == ByteSequence::BigEndian)
        {
            char* first = (char*)&stringLength;
            char* last = first + sizeof(uint32_t);
            std::reverse(first, last);
        }
        //检测长度是否非法
        if (stringLength > MUSE_MAX_BYTE_COUNT){
            readPosition = defaultPosition;
            throw BinarySerializerException("illegal bytes count" , ErrorNumber::IllegalBytesCount);
        }
        if (stringLength != len){
            throw BinarySerializerException("read illegal bytes count" , ErrorNumber::ReadIllegalBytesCount);
        }
        //检测剩余空间是否足够
        readPosition += sizeof(uint32_t);
        leftSize =  byteStream.size() - defaultPosition;
        if (leftSize < stringLength){
            readPosition = defaultPosition;
            throw BinarySerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
        }
        std::memcpy(value,(char*)&byteStream[readPosition], stringLength);
        readPosition += static_cast<int>(stringLength);
        return *this;
    }

    BinarySerializer::Position_Type BinarySerializer::getReadPosition() const {
        return readPosition;
    }

    bool BinarySerializer::checkDataTypeRightForward(BinaryDataType dt){
        //防止初次访问越界
        MUSE_PREVENT_CROSSING_BOUNDARIES()
        if (byteStream[readPosition] == (char)dt){
            readPosition++;
            return true;
        }
        return false;
    }

    BinarySerializer &BinarySerializer::input(const IBinarySerializable &serializable) {
        serializable.serialize(*this);
        return *this;
    }

    BinarySerializer &BinarySerializer::output(IBinarySerializable &serializable) {
        serializable.deserialize(*this);
        return *this;
    }

    const char *BinarySerializer::getBinaryStream() const {
        return byteStream.data();
    }

    void BinarySerializer::saveToFile(const std::string& path) const{
        if (byteStream.empty()){
            throw BinarySerializerException("no data to store in file!", ErrorNumber::NoDataToStoreInFile);
        }
        std::ofstream saver(path,std::ios_base::binary | std::ios_base::trunc | std::ios_base::out );
        if (!saver.fail() && saver.is_open()){
            //数据量过大，会出现问题 unsigned long  到 long 的转换！ 且 long 能表示的数据量有限！
            saver.write(byteStream.data(), (long)byteStream.size());
            saver.flush();
        }else {
            //文件路径错误
            throw std::logic_error("the file path is not right!");
        }
        saver.close();
    }

    void BinarySerializer::loadFromFile(const std::string &path) {
        std::ifstream loader(path,std::ios_base::binary  | std::ios_base::in );
        if (!loader.fail() && loader.is_open()){
            byteStream.clear();
            readPosition = 0;
            constexpr size_t bufferSize = 1024;
            char loaderBuffer[bufferSize];
            size_t readBytes = loader.readsome(loaderBuffer, bufferSize);
            while (readBytes > 0){
                write(loaderBuffer, readBytes);
                readBytes = loader.readsome(loaderBuffer, bufferSize);
            }
        }else {
            //文件路径错误
            throw std::logic_error("the file path is not right!");
        }
    }
}
#endif //SERIALIZER_MUSE_SERIALIZER_HPP
