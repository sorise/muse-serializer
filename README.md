## [muse-serializer](#)
**介绍**： 一个实现简单的 C++ 序列化库，支持丰富的数据类型：整数、浮点数、布尔值、字符、字符串、二进制流、元组、stl的部分容器、用户自定义类型，支持 C++ 11/14/17， 字节编码顺序为小端序。

----
### 提供的接口类型
* muse::serializer::BinarySerializer  序列化和反序列化器
* muse::serializer::BinaryDeserializeView 仅仅支持反序列化，用户可以使用reset_bytestream方法自己指定字节流，用于避免复制拷贝。

**使用例子**：
```cpp
#include "serializer/binarySerializer.h"

using namespace muse::serializer;

//创建一个二进制序列化器
BinarySerializer serializer;

int a = 10;
//单个序列化
serializer.input(a);

std::list<std::string> names{"remix", "muse", "coco" , "tome", "alice" };
std::vector<double> scores = {84.01,98.1,15.2,98.2,15.89,84.01,98.1,15.2,98.2,15.89};
//序列化
serializer.inputArgs(names, scores);

//存储到文件中
serializer.saveToFile("./serializer.dat");

//从文件中加载
BinarySerializer loadSerializer;

loadSerializer.loadFromFile("./serializer.dat");

int aLoad;
std::list<std::string> namesLoad; 
std::vector<double> scoresLoad;

//反序列化
loadSerializer.outputArgs(aLoad, namesLoad, scoresLoad);

std::cout << scoresLoad[0] << std::endl; //84.01
```

### [1. 支持的数据类型](#)
二进制序列化器 binarySerializer 支持数据类型：

| 数据类型                      | 额外信息字节        | 说明                                |
|:--------------------------|:--------------|:----------------------------------|
| bool                      | 1(标识类型)       | `无`                               |
| int16_t                   | 1             | `short`                           |
| int32_t                   | 1             | `int`                             |
| int64_t                   | 1             | `long`                            |
| uint16_t                  | 1             | 类型别名：`u_int16_t`,`unsigned short` |
| uint32_t                  | 1             | 类型别名：`u_int32_t`,`unsigned int`   |
| uint64_t                  | 1             | 类型别名： `u_int64_t`,`unsigned long` |
| float                     | 1             | `float`                           |
| double                    | 1             | `double`                          |
| char                      | 1             | `char`                            |
| unsigned char             | 1             | `unsigned char`                   |
| std::string               | 1+4(标识长度) = 5 | `std::string`                     |
| char*                     | 1+4 = 5       | 纯粹的字节流                            |
| unsigned char*            | 1+4 = 5       | 纯粹的无符号字节流                         |
| std::vector\<T\>          | 1+4 = 5       | T是支持类型，并且需要支持默认构造函数               |
| std::array\<T,Len\>       | 1+4 = 5       | T是支持类型，并且需要支持默认构造函数               |
| std::list\<T\>            | 1+4 = 5       | T是支持类型，并且需要支持默认构造函数               |
| std::map\<K,V\>           | 1+4 = 5       | K,V是支持类型，并且需要支持默认构造函数             |
| std::unordered_map\<K,V\> | 1+4 = 5       | 哈希字典,K,V是支持类型，并且需要支持默认构造函数        |
| std::tuple\<...\>         | 1+2 = 3       | 元组元素需要支持默认构造函数                    |
| std::set\<T\>             | 1+4 = 5       | T需要支持默认构造函数                       |
| **MUSECLASS**             | 1             | 用户自定义类型，需要实现接口，继承抽象类！             |

### [2. 使用方法](#)
使用方法， API 非常简单！

| API                                        | 功能                   | 异常说明                                       |
|:-------------------------------------------|:---------------------|:-------------------------------------------|
| input(parameter)                           | 将参数二进制序列化            | 大量数据下可能抛出 `std::bad_alloc` 内存不足异常          |
| template\<typename... Args\><br/>inputArgs(arg) | 将不定参数二进制序列化          | 大量数据下可能抛出 `std::bad_alloc` 内存不足异常          |
| output(parameter)                          | 将参数二进制反序列化           | 如果失败，抛出自定义异常 `BinarySerializerException`(util.h) |
| template\<typename... Args\><br/>outputArgs(arg)    | 将不定参数二进制反序列化         | 如果失败，抛出自定义异常 `BinarySerializerException`(util.h) |
| clear()                                    | 清除所有内容               | 无                                          |
| reset()                                    | 将读取指针设置到第一个位置，重新反序列化 | 无                                          |
| byteCount()                                | 返回已经存储的字节数量          | 无                                          |

#### [2.1 单个参数的序列化](#)
input/output 是最基本的API（It is the most basic API）： 

```cpp
#include "serializer/binarySerializer.h"
    
muse::serializer::BinarySerializer serializer;
//将元组序列化
std::tuple<std::string ,int ,float> tplOne { "remix", 25, 173.5};
serializer.input(tplOne);

//反序列化
std::tuple<std::string ,int ,float> tplOneOut;
serializer.output(tplOneOut);
```

#### [2.2 可变参数序列化](#)
支持可变参数，可以使得代码更加简洁(Support for variable parameters can make the code more concise:):

```cpp
muse::serializer::BinarySerializer serializer;

bool sex = false;
uint16_t age = 25;
std::string name {"remix"};

serializer.inputArgs(sex, age, name);


bool sexOut;
uint16_t ageOut;
std::string nameOut;

serializer.outputArgs(sexOut, ageOut, nameOut);

std::cout << nameOut << std::endl; //remix
```

#### [2.3 用户自定义类型序列化](#)
用户自定义类型需要继承抽象类 **IBinarySerializable** ,然后调用宏函数 **MUSE_IBinarySerializable** 指定字段名称就可以完成序列化。 

```cpp
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

muse::serializer::BinarySerializer serializer;
user me("remix", 25);

serializer.inputArgs(me);

user you("", 18);

serializer.output(you);

std::cout << you.getName() == me.getName() << std::endl; //true
```

#### [2.4 二进制流信息](#)
获得序列化后的字节流指针：
```cpp
muse::serializer::BinarySerializer serializer;
/*
 * 其他操作
*/
auto size = serializer.byteCount();  //当前已经存储了多少字节的数据

const char* streamPtr = serializer.getBinaryStream(); //获取数据流的指针
```

#### [2.5 将数据存储到文件中](#)
调用方法 **saveToFile** 将数据流存储到文件中， 如果没有任何数据会抛出异常。
```cpp
muse::serializer::BinarySerializer serializer;
/*
 * 其他操作
*/
serializer.saveToFile("./serializer.dat");
```

#### [2.6 从文件中加载数据](#)
从文件中直接加载数据，可能需要优化！
```cpp
muse::serializer::BinarySerializer serializer;
/*
 * 其他操作
*/
serializer.loadFromFile("./serializer.dat");
```

### [3. 约束信息](#)
一些限制:

| 类型                   | 约束                                            | 宏                      |
|----------------------|-----------------------------------------------|------------------------|
| std::string          | 字符串长度上限：1048576`/`1M                          | MUSE_MAX_STRING_LENGTH |
| 字节流                  | 字节流最长为 65536                                  | MUSE_MAX_BYTE_COUNT    |
| tuple\<...\>         | 元组类型长度最多 64, **T** 需要具有默认构造函数                 | MUSE_MAX_TUPLE_COUNT   |
| vector\<T\>          | 元素个数最多 16777216, **T** 需要具有默认构造函数             | MUSE_MAX_VECTOR_COUNT  |
| list\<T\>            | 元素个数最多 16777216, **T** 需要具有默认构造函数             | MUSE_MAX_LIST_COUNT    |
| set\<T\>             | 元素个数最多 16777216, **T** 需要具有默认构造函数             | MUSE_MAX_SET_COUNT     |
| map\<K,V\>           | 元素个数最多 16777216, **K,V** 需要具有默认构造函数           | MUSE_MAX_MAP_COUNT     |
| unordered_map\<K,V\> | 元素个数最多 16777216, **K,V** 需要具有默认构造函数           | MUSE_MAX_LIST_COUNT    |
| 用户自定义类型              | 需要继承 纯虚类 **IBinarySerializable** ，使用宏函数来实现接口。 | MUSE_IBinarySerializable(...)                     |

### [4. BinaryDeserializeView](#)
仅仅用于反序列化，避免内存拷贝开销。
```cpp
//从levelDB中读取二进制数据，存放读取的内容
std::string state_data_out;
//levelDB 查找 state 是否存在
status = chain_ptr->Get(
        leveldb::ReadOptions(), 
        application_state::STORE_STATE_KEY, 
        &state_data_out);

muse::serializer::BinaryDeserializeView viewer;

//指定字节流和长度
viewer.reset_bytestream(state_data_out.c_str(), state_data_out.size());

viewer.output(this->state);

std::cout << "block chain current height: " <<this->state.current_height << std::endl;
std::cout << "top block hash: " << this->state.top_block_hash.get_hex() << std::endl;
std::cout << "genesis block hash: " << this->state.initial_block_hash.get_hex() << std::endl;
```