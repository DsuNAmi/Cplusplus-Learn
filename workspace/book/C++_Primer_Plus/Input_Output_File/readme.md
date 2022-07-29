# 输入、输出和文件

C++程序把输入输出看作字节流。输入时，程序从输入流中抽取字节；输出时，程序将字节插入到输出流中。

- 将流与输入去向的程序关联起来。
- 将流与文件连接起来。

输出时，程序首先填满缓冲区，然后把整块数据传输给硬盘，并清空缓冲区，以备下一批使用。这被称为刷洗缓冲区。

一些管理输入输出的类：

- `streambuff`：为缓冲区提供了内存，并提供了用于填充缓冲区、访问缓冲区内容、刷新缓冲区和管理缓冲区内存的类方法。
- `ios_base`：表示一般流的特征，如是否读取、是二进制还是文本流。
- `ios`：基于 `ios_base`，包含了指向 `streambuff`的指针。
- `ostream`：派生自 `ios`，提供输出方法。
- `istream`：派生自 `ios`，提供输入方法。
- `iostream`：继承了输入和输出方法。

###### 输出指针

```c++
int egg = 11;
char * amount = "done";

//输出地址
cout << &egg << endl;
cout << (void*)amount << endl;  //整个字符串的地址


long val = 560031841;
cout.write((char *)&val, sizeof(long));
//输出：aha!
```

###### 刷新缓冲区

`cout`输出不会立即发送到目标地址，而是被存储在缓冲区中，直到缓冲区填满。

```c++
cout << "Enter a number: ";
float num;
cin >> num;

//程序期待输入这件事，所以会立刻显示消息，即使内容中没有换行符。
//如果没有这种特性，程序将等待输入，而无法通过cout消息来提示用户

```

强制刷新

```c++
cout << "Hello" << flush;	//或者flush(cout);
cout << "World" << endl;	//会多加一个换行符
```

###### 格式化输出

```c++
cout << "Enter a integer: ";
int n;
cin >> n;

cout << n << " " << "(decimal)\n";
cout << hex; //set to hex;
cout << n << " " << "(hexadecimal)\n";
cout << oct << n << " " << "(octal)\n"; // set to oct;
dec(cout);
cout << n << " (decimal)\n";

输出：;
Enter a integer: 13
13 (decimal)
d (hexadecimal)
15 (octal)
13 (decimal)
```

```c++
//头文件
#include <iomanip>


cout.width(int);					//设置宽度，但是只会影响下一次。
cout.fill(char);					//填充字符。
cout.precision(int);				//设置小数的精度
cout.setf(ios_base::showpoint);		//设置末尾的小数点
//cout.setf()功能
ios_base::boolalpha;				//输入和输出布尔值，可以为true或false
ios_base::showbase;					//对于输出，使用C++前缀0x,0
ios_base::uppercase;				//对于16进制输出，使用大写
ios_base::showpos;					//在正数前面加上+
```

###### cin如何检查

它们跳过空白（空格、换行符和制表符）、直到遇到非空白字符。

参考下面的代码：

```c++
int elevation;
cin >> elevation;
//键入-123Z
只会读取-123，因为这是有效的整型部分， 而Z会留在输入流中，等待下一次cin输入。
  
//输入不是满足期望的值。
比如对上述例子输入Zscar;
那么其实整个表达式会返回0，elevation的值将不会被修改。;
while(cin >> elevation)能够在输入正确时一直输入;
```

其他 `ifstream`方法

- `get(char&),get(void)`方法提供了不跳过空白单字符输入功能。
- `get(char*, int, char),getline(char*,int,char)`在默认情况下读取整行而不是一个单词。

###### 单字符输入

在使用char参数或没有参数的情况下，`get()`方法读取下一个输入字符，即使该字符是空格、制表符、换行符。

`get(char& ch)`版本将输入字符赋给其参数，而 `get(void)`版本将输入字符转换为整型并将其返回。

```c++
int ct = 0;
char ch;
cin.get(ch);
while(ch != '\n')
{
    cout << ch;
    ct++;
    cin >> ch;
};
cout << ct << endl;

//另一端代码
int ct = 0;
char ch;
cin >> ch;
while(ch != '\n')
{
    cout << ch;
    ct++;
    cin >> ch;
};
cout << ct << endl;
```

当我们输入：

```bash
I C++ clearly.<Enter>
```

上面一段程序因为不会跳过各种符号，所以会读取到 `\n`退出。

而第二段因为 `cin`会跳过一些符号，所以读出的数据是 `IC++cleary.`。同样，也会跳过换行符，所以循环不会终止。

`get(char&)`返回的是 `istream`对象，所以能够进行拼接。

```c++
char c1, c2, c3;
cin.get(c1).get(c2) >> c3;
//顺序是c1,c2,c3。但是要注意c3接受的输入时不能接收空白字符之类的符号。
```

`get(void)`也是能够读取空白，但是用返回值来将输入传递给程序。所以就不能拼接，但是上述的第一段代码还是能够使用，只不过不是传递参数，而是接收返回值。

**当我们输入一个字符b，再按下回车时，其实我们输入的是b\n**

###### 字符串输入

```c++
istream & get(char *, int, char); //注意读取的内容要比最大长度大一，末尾有一个\0
istream & get(char *, int);
istream & getline(char *, int, char);
istream & getline(char *, int);

//第三个参数作用于分界的字符，默认是换行符。
char buff[50];
cin.get(buff,50);
//
该函数将在到达第49个字符或遇到换行符后停止输入到字符数组中。;
get()将换行符留在输入流中。;
getline()将抽取和丢弃输入流中的换行符。;


//ignore();
ignore(255,'\n');			//读取并丢弃接下来的255个字符或直到到达第一个换行符

```

### 文件输入和输出

写入文件的基本流程：

- 创建一个 `ofstream`对象来管理输出流。
- 将该对象与特定的文件关联起来。
- 以使用 `cout`的方式使用该对象，唯一的区别是输出将进入文件，而不是屏幕。

```c++
#inlcude <ofstream>

ofstream fout;
fout.open(fileName);
//
ofstream fout(fileName);
```

读取文件的基本流程：

- 创建一个 `ifstream`对象来管理输入流。
- 将该对象与特定的文件关联起来。
- 以使用 `cin`的方式使用该对象。

当输入输出流对象过期时，文件的连接自动关闭。也可以调用函数显式关闭。

但是要注意，关闭这样的连接并不会删除流，流和其管理的缓冲区依然存在。

来看一个例子：

```c++
string fileName;
cin >> fileName;

ofstream fout(fileName.c_str());
fout << "For your eyes only!\n";
float secret;
cin >> secret;
fout << "Your secret number is " << secret << endl;
fout.close();

ifstream fin(fileName.c_str());
char ch;
while(fin.get(ch)) cout << ch;
fin.close();
```

###### is_open()检测流

```c++
if(!fin.is_open()) //file open failed
```

###### 同时打开多个文件

```c++
ifstream fin;
fin.open(fileName1);
fin.close();
fin.open(fileName2);
fin.close();
...;
fin.open(fileNameN);
fin.close();
```

###### 文件模式

```c++
open(fileName, model);

//各种格式
ios_base::in;			//打开文件，以便读取
ios_base::out;			//写入
ios_base::ate;			//打开文件，并移动到文件尾
ios_base::app;			//追加到文件尾
ios_base::trunc;		//如果文件存在，则截断文件
ios_base::binary;		//二进制文件
```

使用二进制文件保存结构体

```c++
//对于以下结构体
const int LIM = 20;
struct planet
{
    char name[LIM];
    double d1;
    double d2;
};
planet p1;

//使用二进制方式保存该结构体
ofstream fout("planet.dat",ios::out | ios::app | ios::binary);
fout.write((char*)&p1,sizeof(p1));

//读取
ifstream fin("planet.dat",ios::in | ios::binary);
fin.read((char*)&p1,sizeof(p1));

同样适用于类，但是不适用于带虚函数的类。;
```
