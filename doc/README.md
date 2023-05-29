### [muse-serializer](#)
**介绍(introduce)**： A simple C++serialization library

----
**二进制序列化**: 序列化 （serialization） 是将对象的状态信息转化为可以存储或传输的形式的过程。在序列化期间，对象将其当前状态写入临时或持久性存储区。以后可以通过从存储区中读取或反序列化对象的状态，重新创建该对象。
* 序列化： 将数据结构或对象转换成二进制串的过程,序列化、反序列化速度快
* 反序列化：经在序列化过程中所产生的二进制串转换成数据结构或对象的过程,数据小，传输速度快

### [1. 内存拷贝函数](#)
实现序列化需要借助内存拷贝，实现内存拷贝需要使用如下的API，**std:memcpy** 和 **std::copy** 。

#### [1.1 memcpy](#)
从一个指针所指向的地址作为开始，复制一定大小的内容，给另一个指针所指向的地址。
```cpp
#include <cstring>
void * memcpy ( void * destination, const void * source, size_t num );
```
由src所指内存区域复制count个字节到dest所指内存区域。
```cpp
//数组长度
constexpr unsigned int array_length = 5; 

int vals[array_length] {10,20,30,40,50};

constexpr size_t buffer_size = sizeof(vals);

char des_memory[buffer_size] = {0};
//内存拷贝
std::memcpy(des_memory,vals, buffer_size);

int * ptr = (int *)des_memory;

for (int i = 0; i < array_length; ++i) {
std::cout << *(ptr + i) << " " ;
}//10 20 30 40 50
```

### [2. 大小端问题](#)
由于大多数主机都采用 主机字节序(小端)，也就是序列化采用小端字节序列。

