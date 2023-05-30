## [muse-serializer](#)
**introduction**： A simple C++ Serialization library base on C++ 11/14/17.

----

```cpp
#include "serializer/binarySerializer.h"

muse::BinarySerializer serializer;

bool sex = false;
uint16_t age = 25;
std::string name {"remix"};
std::vector<double> scores {95.5, 89.5, 90.5, 97.0};
std::list<std::string> friends {"muse", "coco", "uni", "tomes"};

//序列化 serializer
serializer.input(sex).input(age).input(name).input(scores).input(friends);


bool sexOut;
uint16_t ageOut;
std::string nameOut;
std::vector<double> scoresOut;
std::list<std::string> friendsOut;

//反序列化 deserializer
serializer.output(sexOut).output(ageOut).output(nameOut).output(scoresOut).output(friendsOut);

std::cout << nameOut << std::endl; //remix
```

### [1. Usage](#)
使用方法， API 非常简单！

| API               | 功能                   | 异常说明                                       |
|:------------------|:---------------------|:-------------------------------------------|
| input(parameter)  | 将参数二进制序列化            | 大量数据下可能抛出 `std::bad_alloc` 内存不足异常          |
| template\<typename... Args\>inputArgs(arg)  | 将不定参数二进制序列化          | 大量数据下可能抛出 `std::bad_alloc` 内存不足异常          |
| output(parameter) | 将参数二进制反序列化           | 如果失败，抛出自定义异常 `SerializerException`(util.h) |
| template\<typename... Args\>output(arg) | 将不定参数二进制反序列化         | 如果失败，抛出自定义异常 `SerializerException`(util.h) |
| clear() | 清除所有内容               | 无                                          |
| reset() | 将读取指针设置到第一个位置，重新反序列化 | 无                                          |
| byteCount()       | 返回已经存储的字节数量          | 无                                          |

#### [1.1 input/output](#)
input/output 是最基本的API（It is the most basic API）： 

```cpp
#include "serializer/binarySerializer.h"
    
muse::BinarySerializer serializer;
//将元组序列化
std::tuple<std::string ,int ,float> tplOne { "remix", 25, 173.5};
serializer.input(tplOne);

//反序列化
std::tuple<std::string ,int ,float> tplOneOut;
serializer.output(tplOneOut);
```

#### [1.2 inputArgs/outputArgs](#)
支持可变参数，可以使得代码更加简洁(Support for variable parameters can make the code more concise:):

```cpp
muse::BinarySerializer serializer;
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
std::cout << nameOut << std::endl; //remix
```



### [2. Support data types](#)
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
| std::string               | 1+4(标识长度) = 5 | `std::string`                     |
| char*                     | 1+4 = 5       | 纯粹的字节流                            |
| unsigned char*            | 1+4 = 5       | 纯粹的无符号字节流                         |
| std::vector\<T\>          | 1+4 = 5       | T是支持类型，并且需要支持默认构造函数               |
| std::list\<T\>            | 1+4 = 5       | T是支持类型，并且需要支持默认构造函数               |
| std::map\<K,V\>           | 1+4 = 5       | K,V是支持类型，并且需要支持默认构造函数             |
| std::unordered_map\<K,V\> | 1+4 = 5       | 哈希字典,K,V是支持类型，并且需要支持默认构造函数        |
| std::tuple\<...\>         | 1+2 = 3       | 元组元素需要支持默认构造函数                    |
| std::set\<T\>             | 1+4 = 5       | T需要支持默认构造函数                       |
| **Class**                 | 1             | 用户自定义类型，需要实现接口，继承抽象类！             |

