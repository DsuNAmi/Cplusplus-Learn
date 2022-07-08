# extern

## C++调用C函数

###### C++调用C函数

```c++
//add.h
#ifndef ADD_H
#define ADD_H


int add(int x, int y);

#endif

//add.c
#include "add.h"

int add(int x, int y)
{
    return x + y;
}


//add.cpp
#include <iostream>

#include "add.h"

using namespace std;

int main()
{
    add(2,3);
    return 0;
}
```

如果没有添加`extern "C"`会报错：

```bash
> g++ add.cpp add.o -o main                                   
add.o：在函数‘main’中：
add.cpp:(.text+0x0): `main'被多次定义
/tmp/ccH65yQF.o:add.cpp:(.text+0x0)：第一次在此定义
/tmp/ccH65yQF.o：在函数‘main’中：
add.cpp:(.text+0xf)：对‘add(int, int)’未定义的引用
add.o：在函数‘main’中：
add.cpp:(.text+0xf)：对‘add(int, int)’未定义的引用
collect2: error: ld returned 1 exit status
```

添加`extern "C"`:

```c++
#include <iostream>

extern "C" {
    #include "add.h"
}


using namespace std;

int main()
{
    add(2,3);
    return 0;
}
```

通常为了C代码能够通用，既能被C调用，又能被C++调用，头文件通常会有如下写法：

```c++
#ifdef _cplusplus
extern "C"{
    #endif
    int add(int x, int y);
    #ifdef _cplusplus
}
#endif
```

