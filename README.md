# [muse-serializer](#)
**introduction**： A simple C++ Serialization library base on C++ 11/14/17.

----
### [1. Support data types](#)
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
| std::tuple\<...\>         | 1+4 = 5       | 元组元素需要支持默认构造函数                    |
| std::set\<T\>             | 1+4 = 5       | T需要支持默认构造函数                       |
| **Class**                 | 1             | 用户自定义类型，需要实现接口，继承抽象类！             |

使用例子：
```cpp
muse::BinarySerializer serializer;

int32_t age = 65;
bool sex = false;
double Pi = 3.141592;

serializer.input(age).input(sex).input(Pi);

int32_t ageOut = 0;
bool sexOut = true;
double piOut = 0.0;

serializer.outputArgs(ageOut, sexOut, piOut);

std::cout << "ageOut: " << ageOut << std::endl;  //65
std::cout << "sexOut: " << sexOut << std::endl;  //0
std::cout << "piOut: " << piOut << std::endl;    //3.141592
```