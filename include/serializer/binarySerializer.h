// 二进制序列化库
// Created by remix on 23-5-26.
// 字节编码顺序：小端序

#ifndef MUSE_SERIALIZER_BINARY_SERIALIZER_H
#define MUSE_SERIALIZER_BINARY_SERIALIZER_H
#include "serializer/util.h"
#include "serializer/IbinarySerializable.h"
#include <algorithm>
#include <climits>
#include <list>
#include <map>
#include <unordered_map>
#include <set>

static_assert(__cplusplus >= 201103L, "C++ version is not right" );

#define MUSE_TEMPLATE_CONVERT_TO_LITTLE_ENDIAN()        \
    if (byteSequence == ByteSequence::BigEndian){       \
        auto last = &byteStream[byteStream.size()];     \
        auto first = last - sizeof(uint32_t);           \
        std::reverse(first, last);                      \
    }

#define MUSE_PREVENT_BOUNDARY() \
    if (readPosition == byteStream.size()){ \
        throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory); \
    }

namespace muse{
    class IBinarySerializable; //define

    class BinarySerializer final{
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
        int readPosition;
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
        int getReadPosition() const;                                //获得当前指针所在位置
        bool checkDataTypeRightForward(DataType type);              //检查类型是否正确，如果正确，移动 readPosition
        BinarySerializer();                                         //默认构造函数
        BinarySerializer(const BinarySerializer& other) = delete;   //禁止复制
        BinarySerializer(BinarySerializer &&other) noexcept;        //支持移动操作
        const char* getBinaryStream() const;                        //返回二进制流的指针

        bool saveToFile();

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
        BinarySerializer &input(const muse::IBinarySerializable &serializable);

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& input(const std::vector<T>& value){
            auto type = DataType::VECTOR;
            //写入类型
            write((char*)&type, sizeof(type));
            //获得成员数量,不能超过某个限制
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_VECTOR_COUNT){
                throw SerializerException("the vector count exceeds the limit", ErrorNumber::TheVectorCountExceedsTheLimit);
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
            auto type = DataType::LIST;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_LIST_COUNT){
                throw SerializerException("the list count exceeds the limit", ErrorNumber::TheListCountExceedsTheLimit);
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
            auto type = DataType::MAP;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_MAP_COUNT){
                throw SerializerException("the map key-value pairs count exceeds the limit", ErrorNumber::TheMapKVCountExceedsTheLimit);
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
            auto type = DataType::HASHMAP;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_MAP_COUNT){
                throw SerializerException("the hash map key-value pairs count exceeds the limit", ErrorNumber::TheHashMapKVCountExceedsTheLimit);
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
            auto type = DataType::SET;
            //写入类型
            write((char*)&type, sizeof(type));
            auto elementCount = value.size();
            if (elementCount > MUSE_MAX_LIST_COUNT){
                throw SerializerException("the set count exceeds the limit", ErrorNumber::TheSetSizeExceedsTheLimit);
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
            auto type = DataType::TUPLE;
            //写入类型
            write((char*)&type, sizeof(type));
            //获得长度
            uint16_t tupleSize = getTupleElementCount(tpl);
            if (tupleSize > MUSE_MAX_TUPLE_COUNT){
                throw SerializerException("the tuple size exceeds the limit", ErrorNumber::TheTupleCountExceedsTheLimit);
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

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& output(std::vector<T>& value)
        {
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)DataType::VECTOR)
                throw SerializerException("read type error", ErrorNumber::DataTypeError);

            auto defaultPosition = readPosition;
            value.clear(); //先清除数据
            //读取长度信息
            readPosition++;
            //获取 vector 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
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
                throw SerializerException("illegal vector count" , ErrorNumber::IllegalVectorCount);
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
            }catch(SerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw SerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& output(std::list<T>& value){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)DataType::LIST)
                throw SerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            value.clear(); //先清除数据
            //读取长度信息
            readPosition++;
            //获取 list 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
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
                throw SerializerException("illegal list count" , ErrorNumber::IllegalListCount);
            }
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i = 0; i < listSize;i++)
                {
                    T v;
                    output(v);
                    value.emplace_back(v);
                }
            }catch(SerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw SerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        template<typename T, typename = typename std::enable_if_t<std::is_default_constructible_v<T>>>
        BinarySerializer& output(std::set<T>& value){
            //防止越界
            MUSE_PREVENT_BOUNDARY()
            //类型检测
            if (byteStream[readPosition] != (char)DataType::SET)
                throw SerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            value.clear();
            //读取长度信息
            readPosition++;
            //获取 list 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
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
                throw SerializerException("illegal set count" , ErrorNumber::IllegalSetCount);
            }
            readPosition += sizeof(uint32_t);
            try {
                for (unsigned int i = 0; i < setSize;i++)
                {
                    T v;
                    output(v);
                    value.emplace(v);
                }
            }catch(SerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw SerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
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
            if (byteStream[readPosition] != (char)DataType::MAP)
                throw SerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            readPosition++;
            value.clear();
            //获取 list 长度
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
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
                throw SerializerException("illegal map k-v pair count" , ErrorNumber::IllegalMapKVCount);
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
            }catch(SerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw SerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
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
            if (byteStream[readPosition] != (char)DataType::HASHMAP)
                throw SerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            readPosition++;
            value.clear();
            //获取 hashmap 数量
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint32_t)){
                readPosition = defaultPosition;
                throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
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
                throw SerializerException("illegal hash map k-v pair count" , ErrorNumber::IllegalHashMapKVCount);
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
            }catch(SerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw SerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
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
            if (byteStream[readPosition] != (char)DataType::TUPLE)
                throw SerializerException("read type error", ErrorNumber::DataTypeError);
            auto defaultPosition = readPosition;
            readPosition++;
            //获取 tuple 元素个数
            auto leftSize =  byteStream.size() - readPosition;
            if (leftSize < sizeof(uint16_t)){
                readPosition = defaultPosition;
                throw SerializerException("remaining memory is not enough ", ErrorNumber::InsufficientRemainingMemory);
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
                throw SerializerException("illegal tuple count" , ErrorNumber::IllegalTupleCount);
            }
            constexpr size_t N = sizeof...(Args);
            if (tupleSize != N){
                readPosition = defaultPosition;
                throw SerializerException("tuple count not match" , ErrorNumber::IllegalParameterTupleCount);
            }
            readPosition += sizeof(uint16_t);
            try {
                TupleReader<N - 1>::read(tpl,*this);
            }catch(SerializerException &serializerException) {
                readPosition = defaultPosition;
                throw serializerException;
            }catch (std::exception &ex) {
                readPosition = defaultPosition;
                throw ex;
            }catch (...){
                readPosition = defaultPosition;
                throw SerializerException("not know error", ErrorNumber::ErrorReadingSubElements);
            }
            return *this;
        }

        BinarySerializer &output(muse::IBinarySerializable &serializable);
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
    void serialize(muse::BinarySerializer &serializer) const override{     \
        auto type = muse::DataType::MUSECLASS;                             \
        serializer.write((char*)&type, sizeof(type));                \
        serializer.inputArgs(__VA_ARGS__);                           \
    };                                                               \
                                                                     \
    void deserialize(muse::BinarySerializer &serializer)  override {    \
        auto readPosition = serializer.getReadPosition();               \
        if (readPosition == serializer.byteCount()){                    \
            throw muse::SerializerException("remaining memory is not enough ", muse::ErrorNumber::InsufficientRemainingMemory); \
        }                                                                                                                       \
        auto result = serializer.checkDataTypeRightForward(muse::DataType::MUSECLASS);                                          \
        if (!result) throw muse::SerializerException("read type error", muse::ErrorNumber::DataTypeError);                      \
        serializer.outputArgs(__VA_ARGS__);                                                                                     \
    }


}
#endif //MUSE_SERIALIZER_BINARY_SERIALIZER_H
