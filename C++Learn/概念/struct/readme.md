# 一文搞懂C和C++中struct

## C中struct

- 在C中`struct`只是单纯的用作数据的符合类型，也就是说，在结构体声明中只能将数据成员放在里面，而不能将函数放在里面。
- 在C结构体声明中不能使用C++访问修饰符，如：`public`，`protected`，`private`，而在C++中可以使用。
- 在C中定义结构体变量，如果使用了下面定义必须加`struct`。
- C的结构体不能继承。
- 若结构体的名字与函数名字相同，可以正正常运行且正常调用！列如：可以定义与`struct Base`不冲突`void Base(){}`。

完整案列：见代码`structFunc.c`

## C++中struct

- C++结构体不仅可以定义数据，还可以定义函数。
- C++结构体中可以使用访问修饰符
- C++结构体使用可以直接使用不带`struct`。
- C++继承。
- 若结构体的名字与函数名相同，可以正常运行正常的调用！但是定义结构体变量只能用带`struct`。

例如：

> 情形1：不适用`typedef`定义结构体别名

未添加同名函数前：

```c++
struct Student
{
    
};

struct Student s; //ok
Student s; //ok
```

添加同名函数后：

```c++
struct Student
{
    
};

Student(){}
struct Student s; //ok
Student s; //error
```



> 情形2：使用`typedef`定义结构体别名

```c++
typedef struct Base
{
    int v1;
    int v3;
    public:
    	int v2;
    	void print(){
            printf("%s\n","hello world");
        }
}B;

//void B() {} //error B已经是一个别名了
```



参考案例：`structFunc.cpp`

## 总结

| c                                                  | c++                                       |
| -------------------------------------------------- | ----------------------------------------- |
| 不能将函数放在结构体声明                           | 能将函数放在结构体声明                    |
| 在C结构体声明中不能使用c++访问修饰符               | public、protected、private在C++中可以使用 |
| 在C中定义结构体变量，如果使用了定义必须加`struct`  | 可以不加`struct`                          |
| 结构体不能继承                                     | 可以继承                                  |
| 若结构体的名字与函数名相同，可以正常运行且正常调用 | 同左，但是只能使用带`struct`定义。        |

# struct与class的区别

区别：

- 最本质的一个区别就是默认的访问控制，`struct`是`public`，而`class`是`private`的。
