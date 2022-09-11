# 条款5，优先选用auto，而非显式型别声明

`auto`可以解决未初始化问题

```c++
int x1;				//有未初始化风险

auto x2;			//编译错误！必须初始化

auto x3 = 0;		//没问题
```

`auto`也可以用来表示只有编译器才掌握的型别

```c++
auto dereFUPLess = 
    [](const std::unique_ptr<Widget>& p1,
       const std::unique_ptr<Widget>& p2)
	{return *p1 < *p2;}


//甚至可以应用在lambda表达式
auto dereFUPLess = 
    [](auto & p1, auto & p2)
	{return *p1 < *p2;}
```



`std::function`是C++11标准中的一个模板，它把函数指针的思想加以推广。可以指涉任何可调用对象，即任何可以像函数一样实施调用之物。

```c++
bool(const std::unique_ptr<Widget> &,
     const std::unique_ptr<Widget> &)
    
std::function<bool(const std::unique_ptr<Widget>&,
                   const std::unique_ptr<Widget>&)> func;

因为lambda表达式可以产生可调物对象，std::function对象中就可以存储闭包。;
bool(const std::unique_ptr<Widget> &,
     const std::unique_ptr<Widget> &)
    dereFUPLess = 
    [](const std::unique_ptr<Widget>& p1,
       const std::unique_ptr<Widget>& p2)
	{return *p1 < *p2;}
```



`auto`可以避免**型别捷径**

```c++
std::vector<int> v;
...;
unsinged sz = v.size();

v.size()的返回值类型为std::vector<int>::size_type，在32为上，使用unsinged就足够，但是在64位的电脑上，unsinged是32位，而std::vector<int>::size_type是64位。;

auto sz = v.size();		//sz的型别为std::vector<int>::size_type
```



```c++
std:unordered_map<std::string, int> m;
...;
for(const std::pair<std::string, int> & p : m)
{
    ...;
}

要注意的是，其中std::unordered_map的键值部分是const,所以哈希表中的类型是std::pair<const std::string, int>,但是声明的p变量并不是这个。;
这会导致一个问题，m中的每一个对象都会做一次复制操作，形成一个p要绑定的临时对象，在结束时析构，降低效率。如果对p取址，就是临时对象的地址，而不是m中对象的地址。

for(const auto & p : m)
```



###### 总结

- `auto`变量必须初始化，基本上对会导致兼容性和效率问题的型别不匹配现免疫，还可以简化重构流程，通常也比下暗示指定型别要少打一些字。
- `auto`型别的变量都有着条款2和条款6中所描述的毛病



# 条款6，当auto推导的型别不符合要求时，使用带显式型别的初始化物习惯用法

有的情况下，你想往东，但是`auto`偏偏往西。

```c++
假设我有一个函数接受一个Widget并返回一个std::vector<bool>，其中每一个bool元素都代表着Widget是否提供一种特定功能;

std::vector<bool> features(const Widge & w);

//这里面第5个比特的意思是：Widget是否具有高优先级。
Widget w;
...;
bool highPriority = features(w)[5];		//w具有高优先级吗？

processWidget(w, highPriority);		//按照w的优先级来处理

但是如果我们将bool修改为auto;

auto highPriority = features(w)[5];		//这是因为返回了std::vector<bool>::reference 而不是bool类型

processWidget(w,highPriority);		//会发生未定义行为


//
之所以要有一个std::vector<bool>::reference是因为std::vector<bool>做过特化了，用了一种压缩形式表示其持有的bool元素吗，每个bool元素用一个比特来表示。;
按理说operator[]应该返回一个T&，但是C++禁止比特的引用。;

bool highProiority = features(w)[5];		//这其中有隐式转换


```



所以，要防止写出这样的代码：

```c++
auto someVar = "隐形"代理型别表达式
```



###### 总结

- “隐形”的代理型别可以导致`auto`根据初始化表达式推导出“错误的”型别。
- 带显示型别的初始化物习惯用法强制`auto`推导出你想要的型别。

