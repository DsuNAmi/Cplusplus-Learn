# 仿Nginx服务器项目框架总结

## 整体思维导图

![](D:\Learn\C++Learn\workspace\project\LinuxNetwork\MyServer.png)

## 源码目录

```bash
- Ngnix-Learn
	- \_include
		- ngx_c_conf.h	#配置类（单例写法）
		- ngx_c_crc32.h #crc校验类（单例写法）
		- ngx_c_lockmutex.h #信号量类
		- ngx_c_memory.h #自定义内存分配和释放（单例写法）
		- ngx_c_slogic.h #服务器业务逻辑子类，包含各种业务处理函数
		- ngx_c_socket.h #服务器业务逻辑类，整个服务器运行的函数
		- ngx_c_threadpool.h #管理线程池的类
		- ngx_comm.h #规定发送的包的规格和结构
		- ngx_func.h #重写打印的函数
		- ngx_global.h #定义全局变量
		- ngx_logiccomm.h #定义客户端发包的结构
		- ngx_macro.h #各种宏定义
	- \app
		- makefile
		- nginx.cxx #主程序
		- ngx_c_conf.cxx #读取配置文件的函数实现
		- ngx_log.cxx #写入日志的函数实现
		- ngx_printf.cxx #重写打印的函数实现
		- ngx_setproctitle.cxx #移动Linux环境变量修改进程名称
		- ngx_string.cxx #字符串处理相关函数
	- \logic
		- makefile
		- ngx_c_slogic.cxx #服务器业务逻辑子类的实现
	- \logs
		- (error.log) #临时生成的日志文件
    - \misc
    	- makefile
    	- ngx_c_crc32.cxx #CRC3校验的函数实现
    	- ngx_c_memory.cxx #自定义内存管理的函数实现
    	- ngx_c_threadpool.cxx #线程池相关的函数实现
    - \net
    	- makefile
    	- ngx_c_socket_accept.cxx #接受客户端连接的函数实现
    	- ngx_c_socket_conn.cxx #管理连接池的函数实现
    	- ngx_c_socket_inet.cxx #其他辅助公能函数实现
    	- ngx_c_socket_request.cxx #接受数据包的函数实现
    	- ngx_c_socket_time.cxx #客户端超时处理的函数实现
    	- ngx_c_socket.cxx #服务器核心类的实现
    - \proc
    	- makefile
    	- ngx_daemon.cxx #守护进程函数实现
    	- ngx_process_cycle.cxx #子进程运行的主要函数
    	- ngx_event.cxx #调用运行函数
    - signal
    	- makefile
    	- ngx_signal.cxx #屏蔽与接受系统信号相关函数
    - common.mk #makefile配置文件
    - config.mk #makefile配置文件
    - makefile 
    - nginx.conf #配置文件
```



## 程序运行流程分段

### 配置文件读取

该服务器在启动时，第一步是读取配置文件。

配置文件的写法如下：

```conf
[Type1]
Type1::VarName = value

# something to ins
[Type2]
Type2::VarName = value
```

> 这样得写法是为了方便管理人员去阅读。
>
> 在读取配置文件的时候，会屏蔽（跳过）`[]`，`#`开头以及空行的内容。只关心`=`左右两端的内容。
>
> 同时在编写配置文件时，每一行的最大长度要限定。



文件读取的核心函数是`CConfig::Load(const char * pconfName)`函数：

```c++
//文件读取的函数，核心思路如下：
//创建一个文件对象
//使用fgets对文件进行读取，这里读取时要按照文件设定的每行最大限额长度读取
//之后判断读取的这一行的第一个字符是不是空字符，如果是空字符直接跳过这一行，如果不是，则进行后面的判断
//接下来判断该行的第一个字符是否是注释符（同样这里可以加一些别的符号来防止恶意修改配置文件），如果满足注释符，那么跳过这一行；如果不是，则进行后面的判断
//过滤了上述的条件后，首先判断句子的长度，如果存在字符，就把该行的后面的空格、回车、换行符都替换成空字符。
//这里删除完这些符号之后，需要再判断一遍是不是空行了，因为把这些空格都删了。
//如果不是空行，判断开头是否是[，如果是也要跳过，因为这个是标记格式
//如果判断到这里，那么就一定是Type()::VarName = value 的格式
//那么就按照`=`切分字符串，左右的字符串内容分别赋值给配置类中。

bool CConfig::Load(const char * pconfName)
{
    FILE * fp;
    fp = fopen(pconfName,"r");
    if(fp == nullptr)
    {
        return false;
    }

    //读取每一行配置文件，但是限定每一行的长度
    char lineBuf[501];

    while(!feof(fp))        //这里是当文件不走到最后的时候
    {
        if(fgets(lineBuf,500,fp) == nullptr)
        {
            continue;
        }


        if(lineBuf[0] == 0) //这里表示是空行
        // if(lineBuf[0] == '\0') ?
        {
            continue;
        }

        //处理注释
        if(*lineBuf == ';'  ||
           *lineBuf == ' '  ||
           *lineBuf == '#'  ||
           *lineBuf == '\t' ||
           *lineBuf == '\n')
        {
            continue;
        }

    
    lblprocstring:
        //后面有换行，回车，空格都截取
        if(strlen(lineBuf) > 0)
        {
            if(lineBuf[strlen(lineBuf) - 1] == 10 ||
               lineBuf[strlen(lineBuf) - 1] == 13 ||
               lineBuf[strlen(lineBuf) - 1] == 32)
            {
                lineBuf[strlen(lineBuf) - 1] = 0;
                goto lblprocstring;
            }
        }

        if(0 == lineBuf[0])
        {
            continue;
        }
        //这里是防止出现非有效信息的时候，就是删除完了后面的内容，结果出现空行
        if(*lineBuf == '[')     //跳过[开头的内容，直接continue
        {
            continue;
        }

        char * pTemp = strchr(lineBuf, '=');

        if(pTemp != nullptr)
        {
            LPCConfItem pConfItem = new CConfItem;
            memset(pConfItem, 0, sizeof(CConfItem));
            //等号两边的分别复制过去
            strncpy(pConfItem->itemName,lineBuf,(int)(pTemp - lineBuf));
            //右边不涉及到长度了
            strcpy(pConfItem->itemContent, pTemp + 1);

            Rtrim(pConfItem->itemName);
            Ltrim(pConfItem->itemName);
            Rtrim(pConfItem->itemContent);
            Ltrim(pConfItem->itemContent);

            // printf("itemname=%s | itemcontent=%s\n",pConfItem->itemName,pConfItem->itemContent);
            this->m_configItemList.emplace_back(pConfItem);     //内存在结束的时候要放掉

        }
    }

    fclose(fp);     //很重要
    return true;
}
```

### 日志读写

本项目讲所有输出的内容全部写入日志当中，同时根据不同的错误等级标记输出内容的重要性。

> 关于日志的重要性：在宏定义文件中把日志输出的内容分为了不同的等级，一共分为9个不同的等级，所以我们在记录日志文件的同时，同时还要观察记录时的等级。

首先我们需要定义一个日志类：

```c++
class NgxLogT
{
  	int logLevel;
    int fd;				
};
```

> 该类主要涉及到两个变量，一个用来存储文件标识符，一个用来表示日志的等级，也就是该条日志的重要性。

在服务器启动的时候，我们需要读取日志文件路径，如果是第一次运行程序，那么程序会在需要记录日志时创建文件并在该日志文件中记录该条日志；如果程序不是第一次运行，那么会在该文件的后续继续记录日志。

在Linux系统中，我们要注意文件的打开方式：

```c++
ngxLog.fd = open((const char*)pLogName, O_WRONLY | O_APPEND | 0_CREAT, 0644);
//表示只写、追加、创建、同时设置创建文件的权限
```

主要的日志输出函数定义为：

```c++
/// @brief 打印结果到日志中
/// @param err , 日志等级
/// @param fmt , 输出的字符串格式
/// @param printResult(...), 这里是要打印的变量
void NgxLogStderr(int err, const char * fmt);
```

> 该函数主要是需要把`fmt`中输出的字符串格式按照`printf`的公能生成一个新的字符串，然后把这个新的字符串写入到文件中，本质是构造字符串的过程。
>
> 但是该函数中用了可变参数。
>
> 对于传递进来的`fmt`变量，形如格式：`"There is something wrong in %s, error value is %d !"`，之后的可变参数`...`传递的为`logPoision = "core", err = 4`。那么我们就需要把`fmt`构成新的字符串：
>
> `There is something wrong in core, error value is 4`。

那么问题就是构造该字符串的问题，以及如何处理可变参数。

构造新字符串的核心函数为：

```c++
//对于nginx自定义的数据结构进行标准化输出
/****
 * buf 数据放在这里
 * last 放置的数据不要超过这里
 * fmt 以这个为首的可变参数
 * NgxLogStderr(0,"invalid option: \"%s\", %d", "testinfo", 123);
 * fmt = "invalid option: \"%s\", %d"
 * args = "testinfo", 123
*/
u_char * NgxVSLPrintf(u_char * buf, u_char * last, const char * fmt, va_list args);
```

###### va_list

我们首先来看如果处理可变参数：

```c++
va_list args; //首先定义一个可变参数的变量
va_start(args, fmt);		//这个函数的作用就是把args定位到某个参数的后面一个参数，因为在NgxLogStderr函数中，...在fmt的后面，所以需要定位到fmt

//如果移动可变参数的地址？比如我要取第一个可变参数应该如何取
typeName varName = va_arg(args, typeName);
//该函数va_arg()，可以把可变参数的当前指针指向的参数按照函数中第二个参数的类型取出来，比如：
int varName = va_arg(args, int);
//同时，将指针指向下一个参数
char varName - va_arg(args, char);

va_end(args); //释放这个可变参数
```

###### fmt

之后我们来看如何构造新的字符串，整个思路其实非常简单，关键的问题在于如果对字符串进行判断。

```c++
//思路如下
//首先对每个字符进行判断，循环结束的条件就是到设定的字符串末尾，或者是字符串结束。
//
while(*fmt && buf < last);
//首先，只有涉及到%后的字符才是需要替换为变量的结果的，如果是非%后的字符是正常输出的，所以涉及到的逻辑就是
if(*fmt == '%')
{
    //做替换变量为字符串的操作
}
else
{
    *buf++ = *fmt++;//移动字符串
}
//这时候我们关心if(*fmt =='%')中的内容，我们首先需要判断%后面是否有0的存在，因为在printf中，是需要%00d,按照这个规则，是需要将其存在0填充的。
zero = (u_char)((*++fmt == '0') ? '0' : ' ');
//之后我们需要判断后面的数字，因为存在对其的位数
//判断完对其的情况之后，我们就需要对各种情况进行处理了
//这里有一个小技巧，就是使用for(;;)无限循环和switch的组合进行每个字符的判断
for(;;)
{
    switch(*fmt)
    {
            //进行各种情况的判断
    }
}
//这里判断时，有u,X,x,.等，这些都是后面还有跟d,f的，所以要先判断好
//之后我们再判断d,f各种类型，然后如果是d,f类型就需要把数字转换成字符串，如果是s，那么就直接把字符串复制过去就行，也可以直接给地址过去。
//构造完成之后，我们返回该构造好的字符串
return buf;
```



这里我们获取到构造完成的字符之后，我们把日志的等级信息与该字符串进行拼接，然后我们写入对应的内容中。

```c++
write(ngxLog.fd, errstr, p - errstr);

//我们也可以试着往屏幕输出
write(STDERR_FILENO,errstr,p - errstr);

//同样，我们可以在日志文件失效的时候，把文件流改到屏幕
ngxLog.fd = STDERR_FILENO;
```



### 信号的接受与屏蔽

在Linux系统中，进程和线程的中止可以通过屏幕信号或者是开放该信号的接受来处理，我们可以通过系统函数来判断信号的接受，之后我们可以定义一个句柄函数来处理不同的信号。

```c++
//我们首先定义一个和信号有关的结构体或者是类
class NgxSignalT
{
  	int 			signo;		//信号对应的数字缩写
    const char		*sigName;	//信号的名字
    void(*handler)(int signo, siginfo_t * signfo, void * uContext);	//信号处理函数
};

//之后我们把对应的信号和与接受到该信号需要进行的操作进行绑定
ngxSignalT signals[] = {
    //signo         signame             handler
    {SIGHUP,        "SIGHUP",           ngxSignalHandler},  //终端断开信号，对于守护进程常用于reload重载配置文件
    {SIGINT,        "SIGINT",           ngxSignalHandler},  //标识2
    {SIGTERM,       "SIGTERM",          ngxSignalHandler},  //标识15
    {SIGCHLD,       "SIGCHLD",          ngxSignalHandler},  //子进程推出时，父进程会收到这个信号
    {SIGQUIT,       "SIGQUIT",          ngxSignalHandler},  //标识3
    {SIGIO,         "SIGIO",            ngxSignalHandler},  //指一个异步I/O事件
    {SIGSYS,        "SIGSYS, SIG_IGN",  nullptr},   //我们需要忽略这个信号，SIGSYS表示收到了一个无效系统调用，如果我们不忽略，就会被操作系统杀死
                                                    //所以我们把句柄设置为nullptr，要求忽略这个信号
    //...以后添加
    {0 ,            nullptr,            nullptr}    //信号对应的数字至少是1,所以0可以是一个特殊标记
};
```



之后我们需要把我们定义的函数要挂在系统的信号处理功能对象中

```c++
struct sigaction sa;		//这是一个跟信号有关的结构体
//之后，我们通过遍历上述定义的这个类数据，把每一个信号需要处理的情况挂到系统中。
for(sig = signals; sig->signo !=0; ++sig)   //0刚好可以终止这个信号操作
    {
        memset(&sa, 0, sizeof(struct sigaction));

        if(sig->handler) //如果句柄不为空   
        {
            sa.sa_sigaction = sig->handler; //指定信号处理函数
            sa.sa_flags = SA_SIGINFO;       //表示信号附带的参数可以被传递到信号处理函数中
                                            //为了让自定义函数生效，必须用这个
        }
        else
        {
            sa.sa_handler = SIG_IGN;        //忽略信号的处理
        }

        // 比如我们处理某个信号，比如SIGUSR1信号时不希望SIGUSR2信号，那我们就可以用sigaddset(&sa, sa_mask, SIGUSR2)，
        // sa_mask就是要阻塞的信号
        sigemptyset(&sa.sa_mask);       //把信号集清零，表示不阻塞任何信号

        //信号处理函数
        //参数1,要操作的信号
        //参数2,针对信号的信号处理和函数，以及要屏蔽的信号
        //参数3,返回以往对应信号的处理方式，我们不需要
        if(sigaction(sig->signo, &sa, nullptr) == -1)
        {
            NgxLogErrorCore(NGX_LOG_EMERG,errno,"sigaction(%s) failed",sig->sigName);
            return -1;      //有错误直接返回
        }
        else
        {
            // NgxLogStderr(0,"sigaction(%s) succeed!", sig->sigName);
        }
    }
```

> 这个里面涉及到子进程成为僵尸进程的问题，为了防止子进程成为僵尸进程，我们需要使用`waitpid()`函数来控制子进程的死亡，正常情况下，当父进程创建完子进程之后，当子进程被杀死之后，由于父进程并没有释放子进程的资源，这时候子进程会变成一个僵尸进程，占用系统的进程号，但是父进程并没有办法获得子进程的状态，但是当子进程被杀死之后，父进程会收到一个`SIGCHLD`的信号，所以在信号接受中，我们可以设定`ngxSignalHandler`函数当收到这个信号时，来调用`waitpid()`函数使父进程获得子进程被杀死的状态，以保证子进程不会变成僵尸进程。

### 控制进程名（进程名的修改）

控制进程名的逻辑其实很简单，当我们在Linux系统上运行一个C++编译好的程序时，我们可以在文件名后添加参数，而这些参数就是会和文件名一起组成进程的名称，那么我们为了修改进程的名称，也就是需要把这些参数内存上的内容给替换成我们需要的名称即可。也就是修改环境变量`environ`数组的内容，但是我们为了保证之前的内容还在，所以我们需要把之前的内容移到另一个地方来保存这些内容，移动之后，我们再把环境变量指向新的内存就行了。然后我们修改旧内存的内容就可以修改标题了。

```c++
//移动内存
for(i = 0; environ[i]; ++i)
{
    g_environlen += strlen(environ[i]) + 1;//这里是因为\0占一个内存
}

gp_envmem = new char[g_environlen];
memset(gp_envmem,0,g_environlen);

char * pTemp = gp_envmem;

//把原来的内存内容搬到新地方来
for(i = 0; environ[i]; ++i)
{
    size_t size = strlen(environ[i]) + 1;
    strcpy(pTemp, environ[i]);
    environ[i] = pTemp;         //环境变量指向这块新内存
    pTemp += size;
}
```

### 服务器启动

**服务器在启动时，第一步会将配置文件中的内容进行读取**，包括和包大小、监听的端口数量，超时时间，允许的最大数量客户端。

之后会启动服务器的端口对进行监听，等待客户端的连接。（**TCP**）

基本的思路就是`socket`编程的流程：

1. 创建`socket`
2. `bind`
3. `listen`
4. `accept`，不过在初始化阶段我们只需要保存我们创建的`socket`信息就行，并不需要立即调用`accept`，因为我们需要使用`epoll`技术来实现高并发，我们只需要保存好创建的`socket`信息就行。

与一般的`socket`编程不一样的地方是，这里需要修改一下`socket`的设置

```c++
//setsockopt() 设置一些套接字参数选项
//参数2，表示级别，和参数3配套使用，
//参数3,允许重用本地地址
//设置 SO——REUSEADDR
setsockopt(isock, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuseaddr, sizeof(reuseaddr));

//同时设置成非阻塞模式
Setnonblocking(isock);
```

### 创建守护进程

守护进程是一个在后台运行并且不受任何终端控制的进程。使用守护进程的主要目的是为了让其独立于终端，使得用户在登录不同的终端的时候，该进程不会收到关闭终端和开启终端的影响。

```c++
//采用经典的switch(fork())方法来创建子进程。
switch(fork())
{
    case -1:
        //错误的情况，直接标记错误
        return -1;		//我们直接返回错误的结果
    case 0:
        break;
        //如果是子进程，那么就直接进行后面的程序，也就是子进程的才会运行到这里
    case 1:
        //这里是父进程的情况，那么就要处理父进程的业务，但是如果是创建守护进程，那么就直接关闭这个，因为我们需要创建一个新的手守护进程来作为我们的主进程。
        return 1;
}

//之后出来之后，我们需要把原来父进程的资源给释放了
```

> 这一段代码相当于是，把原来的主进程直接杀掉，创建的新进程（守护进程作为新的主进程）



然后我们来看看是如何创建一个守护进程的。

```c++
//这里是守护进程走到的地方
ngxPid = getpid();
//不要限制文件权限
umask(0);

//打开黑洞设备，这里就是需要把该守护进程的输出，设定到黑洞设备中。
int fd = open("/dev/null",O_RDWR);

//之后我们关闭标准的输入输出
dup2(fd, STDIN_FILENO); //这里我们把屏幕的输入给关了，就只能输出到黑洞设备
dup2(fd, STDOUT_FILENO);//这里我们把屏幕的输出给关了，就只能输出到黑洞设备

//之后所有的输出都不会走到屏幕上了，之前那个输出到屏幕上的代码就会出错，之后所有的内容都会输出到日志文件里了。
```



### 创建子进程进行业务处理

在这个服务器的框架中，设定的是一个服务器父进程创建多个子进程，父进程用来控制子进程，子进程才是进行业务操作的核心。所以我们需要创建多个子进程来进行业务处理，但是创建子进程时我们需要先屏蔽父进程的某些信号。因为我们要保证父进程一直在，这样才能稳定保证把我们需要创建的子进程创建完，并且保证顺利运行。

```c++
//首先开始我们屏蔽一些信号
sigset_t		set;
//首先我们需要把一些信号在父进程中都屏蔽了
sigemptyset(&set);			//这时候把信号集清空

//之后把需要屏蔽的信号集
sigaddset(&set,SIGCHLD);		//还有很多信号

//之后我们设置将信号屏蔽
sigprocmask(SIG_BLOCK,&set,nullptr);
```



之后我们开始创建业务子进程，我们首先需要通过一个循环来创建多个子进程。

```c++
//我们之前从配置文件中读取了子进程的数量
for(int i = 0; i < threadNums; ++i)
{
    //创建子进程的函数
    NgxSpawnProcess(i,"woker process");
}
```

> 这里我们循环创建需要创建的子进程。



创建完子进程之后，我们需要把父进程屏蔽的信号恢复。但是我们又不希望父进程直接结束，这个时候我们可以把父进程锁死在一个循环里，并让它屏蔽部分的一些信号，防止父进程以外被杀掉

```c++
//控制父进程进入一个循环
for(;;)
{
    sigsuspend(&set);
    sleep(1);
}
```

###### 子进程初始化

首先我们需要对子进程进行初始化，子进程初始化的第一步，需要把之前父进程屏蔽的信号给释放掉（父进程取消进程屏蔽是在创建完所有子进程之后）。

```c++
sigprocmask(SIG_SETMASK, &set, nullptr);
```

**之后我们需要创建线程池**，创建完线程池之后，我们对子进程的各类参数进行初始化，因为子进程才是真正处理业务逻辑的进程，初始化的内容包括了**互斥量的初始化、信号量的初始化、首发数据的线程初始化、回收的线程初始化、管理超时的初始化。**

最后我们来初始化`epoll`

###### 子进程死循环

我们将子进程进行了初始化之后，我们需要将子进程设定到一个循环里面，这个循环就是需要子进程一直去处理的。

```c++
//同样是死循环
for(;;)
{
    //循环里面的操作会一直等待事件，也就是epoll技术得核心部分。
    epoll_wait(mEpollHandle, mEvents, NGX_MAX_EVENTS, timer);
    //等待事件到来然后进行处理。
}
```



那么整个程序的流程到这里就结束了，子进程退出或者是意外关闭会有代码进行内存关系，释放对应的资源。

## 核心运行逻辑

本服务器的核心逻辑主要有两个，一个是`epoll`技术，另一个是线程池。

我们先来看`epoll`技术是如何实现的。

###### epoll_create函数

```c++
int epoll_create(int size);
//创建一个epoll对象，返回一个对象（文件）描述符来表示该epoll对象，后续要通过该描述符进行数据的收发

该对象最终是要被close的，因为这就是类似套接字的一个句柄;
size的值不管怎么解释，这个值一定要大于0;


struct eventpoll * ep = (struct eventpoll *)calloc(1, sizeof(struct eventpoll));
//生成一个eventpoll对象。
eventpoll中我们只关心rbr和rblist;
#rbr,可以将该成员理解为一颗红黑树根节点;
#rdlist,可以将成员理解为一个双向链表的表头指针;
```

###### epoll_ctl函数

```c++
int epoll_ctl (int efpd, int op, int sockid,  struct epoll_event * event);
//把一个socket及该socket相关的事件添加到epoll对象描述符中，以通过该epoll对象来监视该socket上数据的来往情况，当有数据来往时，系统会通知程序
参数efpd，从epoll_create返回的对象描述符;
参数op，一个操作类型，可以理解为一个数字，一般是1，2，3
    1 添加sockid上关联的事件，对应一个宏定义EPOLL_CTL_ADD
    2 删除sockid上关联的事件，对应一个宏定义EPOLL_CTL_DEL
    3 修改sockid上关联的事件，对应一个宏定义EPOLL_CTL_MOD
    添加就是把socket绑定在红黑树上，让epoll对象去监听这个对象，有消息就通知。
    这里要注意删除只是把socket从红黑树上删除，而不会关闭这个socket连接，删除了就不会被通知到这个epoll对象。
参数sockid，一个TCP连接。sockid就是红黑树的key值;
参数event，传递对应的信息;
```

###### epoll_wait函数

```c++
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
//阻塞一小段时间并等待事件发生，返回事件集合，即获取内核的事件通知。
//这里就要是遍历epoll对象中的双向链表，数据都在里面记录
参数epfd，从epoll_create返回的epoll对象;
参数events，一个数组，长度为maxevents，表示此次调用可以最多收集的读写事件。;
参数timeout，阻塞等待的时长。;

这就是epoll高效的原因，是因为不是从所有的socket中遍历是否有事件，而是从有事件的中寻找socket;
```

EPOLL有两种工作模式

- LT，Level Trigged，即水平触发，低速模式

  默认采用LT。

  发生一个事件，如果程序不处理，那么该事件就一直触发。

- ET，Edge Trigged，即边缘触发，高速模式

  这种触发方式只对非阻塞socket有用。发生一个事件，内核只会通知程序1次，不会再通知。如果没有处理就没有处理了，不会一直喊着让处理。

###### 一道面试题

**题目**：使用Linux Epoll模型，水平触发模式。当socket可写时，会不停地触发socket可写的事件。

**答案**：（1）需要向socket写数据的时候才把socket可写事件通知加入epoll的红黑树节点，等待可写事件。当程序收到来自系统的可写事件通知后，调用write或read发送数据。所有数据都发送完毕后，把socket可写事件通知从epoll的红黑树节点中**移除（移除的是可写事件通知，不是红黑树节点）**。这种方式的缺点是，即使发送很少的数据，也要把socket可写事件通知加入epoll的红黑树节点，写完后再把可写事件通知从epoll的红黑树节点中移除，有一定的操作代价。

    （2）开始不把socket可写事件通知加入epoll的红黑树节点，需要发数据时，直接调用write或send发送，如果write或者send返回EAGAIN（缓冲区满了，需要等待可写事件才能继续往发送缓冲区写数据），再把socket的写事件通知加入epoll的红黑树节点。这就变成了在epoll的驱动下发送数据，全部数据发送完毕后，再把可写事件通知从epoll的红黑树节点中移除。这种方式的优点是，数据不多时可以避免epoll的红黑树节点中针对写事件通知的增删，提高了程序执行效率。



在本项目中，我们需要定义一个**连接池**来作为`epoll`的辅助工具。

连接池中是每一个连接，那么我们需要做这个池的作用相当于就是我们有一个连接的池子，当我们一个连接时，我们就从连接池中拿出一个连接来保存当前连接的各种信息，方便我们后续对该连接进行各种操作。每一个连接都是一个结构体：

```c++
struct NgxConnectionS
{

    NgxConnectionS();       //构造函数
    virtual ~NgxConnectionS();
    void GetOneToUse();         
    void PutOneToFree();

    int                 fd;
    lpngxListeningT     listening;      //如果这个链接被分配了一个监听套接字，那么这个里边就指向那个内存的首地址

    //--------------------
    // unsigned            instance:1;    //【位域】就是有多少位二进制，0表示有效，1表示实效
    uint64_t            iCurrSequence;      //引入的一个序号，每次分配出去+1，可以检测废包
    struct sockaddr     sSockAddr;          //保存对方地址信息

    //和读有关的标志
    // uint8_t             wReady;         //写标记

    ngxEventHandlerPT   rHandler;           //读相关的处理方法
    ngxEventHandlerPT   wHandler;           //写事件的处理方法


    //和epoll事件有关
    uint32_t                events;


    //和收包有关
    unsigned char           curStat;            //当前收包的状态
    char                    dataHeadInfo[_DATA_BUFSIZE_]; //用于保存收到的数据包头的信息
    char                    *precvbuf;          //接受数据缓冲区的头指针，对收到不全的包非常有用
    char                    irecvLen;           //要收到多少数据，由这个变量指定
    char                    *precvMemPointer;     //new出来用于收包的内存首地址，和上一个变量配对使用

    // bool                    ifnewrecvMem;       //如果我们成功的收到了包头，那么我们就要分配内存开始保存，包头+消息头+包体内容

    //和发包有关
    std::atomic<int>        iThrowSendCount;    //发送消息
    char                    *pSendMemPoint;     //发送完成释放用的
    char                    *pSendBuf;          //发送数据缓冲区的指针
    unsigned                iSendLen;           //要发的数据

    //逻辑处理的互斥量
    pthread_mutex_t         logicProcMutex;    


    //和回收有关
    time_t                  inRecyTime;


    //和心跳包有关
    time_t                  lastPingTime;


    //和网络安全有关
    uint64_t                FloodKickLastTime;          //flood攻击上次收到包的时间
    int                     FloodAttackCount;           //flood攻击在该时间内收到包的次数统计

    //---------------------
    lpngxConnectionT    next;           //这是一个后继指针

};
```

> 其实这个连接池类似链表，但是我们有一个指针指向当前空闲的位置，每次需要使用时，我们从连接池中取出一个空闲的连接来保存当前的连接信息，同时，指向空闲连接的指针会指向**下一个空闲的连接**，（这里并不是下一个连接，因为下一个连接可能并非空闲）。
>
> 这个连接中，除了保存套接字句柄、地址外，还要保存读写事件处理函数，收包的相关变量，以及超时信息和统计信息。

那我们有了这样的连接来保存每一个连接之后，我们如何结合`epoll`技术来处理相应的事件呢。

```c++
//首先，我们先创造一个eopll对象
mEpollHandle = epoll_create(mWorkConnections); //参数就是需要创建的连接池数量，其实后面这个参数只要大于0，就行了。
//之后我们创建连接池，这个连接池要把所有的连接都连接起来
//连接的方式可以使用链表，可以直接使用vector
InitConnection();
//然后我们先遍历监听端口，这里我们为每一个连接池和一个监听的端口绑定，绑定之后我们需要绑定接受客户端的事件，这样等客户端发送来连接请求时，我们能够进行处理。
//这里看一些对于每个监听端口是如何处理的
//首先分配一个连接，
lpngxConnectionT pConn = NgxGetConnection((*pos)->fd);
//之后把连接池和之前构造的端口信息类进行绑定
pConn->listening = (*pos);
(*pos)->connection = pConn;
//然后我们需要把该连接的事件处理函数绑定到接受客户端的函数上
pConn->rHandler = &CSocket::NgxEventAccept;
//然后我们把对应的内容添加到epoll中
epoll_ctl(mEpollHandle, eventType, fd, &ev);
```

> 其中的`ev`是`struct epoll_event ev`，是`epoll`技术的事件结构体，这样主要是判断当前的事件类型，然后把对应的连接保存到该事件中，等待事件发生时，那么就把对应的连接取出来，然后按照事件的类型进行对应的处理。因为事件中保存的是连接池的地址。

之后就是需要让子进程一直等待，事件的发生

```c++
for(;;)
{
    int events = epoll_wait(mEpollHandle, mEvents, NGX_MAX_EVENTS, timer);
    //timer = -1一直阻塞，timer = 0 立即返回
    //然后我们进行错误判断
    //之后我们处理对应的事件
for(int i = 0; i < events; ++i)
    {
        pConn = (lpngxConnectionT)(mEvents[i].data.ptr);

        //处理正常事件
        rEvents = mEvents[i].events;
        // if(rEvents & (EPOLLERR | EPOLLHUP)) //发生了错误或者客户端断连
        // {
        //     rEvents |= EPOLLIN | EPOLLOUT;
        // }

        if(rEvents & EPOLLIN)   //如果是读事件,这里其实就是三次握手成功后会调用这个
        {
            (this->*(pConn->rHandler))(pConn);
            //新连接已经接入，那就是执行NgxEventAccept(c)
            //如果是已经连入，那就是执行NgxWaitRequestHandler(c)
        }

        if(rEvents & EPOLLOUT) //这里就是写事件了
        {
            //.....待扩展
            // NgxLogStderr(errno,"22222222222222222222.");
            if(rEvents & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                //三个错误分别是
                //对应的连接发生错误
                //对应的连接被挂起
                //TCP连接关闭或者是半关闭
                --pConn->iThrowSendCount;
            }
            else
            {
                (this->*(pConn->wHandler))(pConn);
            }

        }
    }
}
```

> `mEvents`是自定义的一个`struct epoll_event`数组，目的是为了获得在阻塞期间有事件的所有事件，`events`是收到事件的数量，然后我们遍历每一个事件，根据事件的类型，进行对应的处理，调用对应的处理函数
>
> `(this->*(pConn->rHandler))(pConn)`
>
> 上述的例子就是当客户端连接来了之后，进行接受客户端处理。



###### Accept事件函数

```c++
//首先我们调用accept函数
//然后我们判断是否在可连接范围内
//然后我们给连接的客户端套接字句柄分配一个连接，同样，连接池中的连接也要够。
//之后的操作和当时绑定监听端口的操作是类似的
//同时还需要把绑定读写的操作函数，以便进行通信,主要是接受客户端发送的数据，还是给客户端发送数据。
newc->rHandler = &CSocket::NgxReadRequestHandler;
newc->wHandler = &CSocket::NgxWriteRequestHandler;
//之后把连接挂到epoll上。
```



###### 消息接受函数

我们来看看，系统在接受到客户端的连接请求之后，是如何接客户端发来的包。



因为存在粘包的可能。

> ### TCP粘包
>
> ###### 客户端粘包
>
> 客户端采用了**Nagle算法**，客户端3次调用write函数发送数据，Nagle算法可能会把这3次调用打成一个数据包发送给服务器。这就是客户端粘包。但是可以关闭Nagle算法，这样就是3个包，但是每个包都打包包头，这样效率会低一些。
>
> ###### 服务器粘包
>
> 服务器端不可能随时都在recv数据，可能recv一次之后要做一些其他操作，需要时间，假设这期间客户端发送的3个数据包到了服务器端，保存在服务器端的接收缓冲区中。等待服务器完成其他操作，过来之后，服务器还是会把3个包的数据全部一次收到。这就是服务器粘包。
>
> 服务器在收包时，因为网络拥堵情况等因素，收到多少数据都有可能，可能数据包小，一次性就收完了，可能数据包大，要**几次才能收完一个完整的包（缺包）**。
>
> ###### 问题解决
>
> 想要正确解决粘包问题，需要程序员给收发的数据包定义一个统一的格式，服务器端及客户端都按照该格式来收发数据。
>
> 这个格式就是**包头+包体**的格式。收发的任何一个数据包，都要遵循这种包头+包体的格式。其中包头是固定长度的。包头其实是一个结构，在包头中要有一个成员，用于记录整个包的长度（包头+包体）。因为包头长度固定，且能够从包头中获取整个包的长度，用整个包的长度减去包头的长度，就得到了包体的长度。



所以程序中规定了，发包的标准结构：

```c++
//结构定义
#pragma pack(1)     //对齐方式，1字节对齐

typedef struct _COMM_PKG_HEADER
{
    unsigned short pkgLen;      //包头+包体总长度，--2字节
    unsigned short msgCode;     //消息类型2字节，用于区别每个不同的命令
    int            crc32;       //crc校验，4字节，
} COMM_PKG_HEADER, *LPCOMM_PKG_HEADER;

#pragma pack()      //取消指定对齐
```



规定好包的结构之后，我们接受包也有一套规定的流程，这样就不会随意接收不同类型的包。

```c++
//因为我们提前设定好了包的结构，包头和包体
//那么我们收的时候，就可以按照先收包头，再收包尾的这个顺序来首发从客户端发来的包
//因为在每一个连接中保存了包头的长度，所以我们调用
reco = recv(c->fd, buff, pConn->irecvLen, 0);
//其中pConn->irecvLen就是一个包头的长度，我们先收一个包头的长度，之后会因为没有收满而修改
//其实会做很多错误判断，但是讲实现逻辑都跳过了
//之后我们先判断reco的值，如果小于等于0，那么就表示没有收到，那么直接退出。
//之后我们判断当前的接受状态：
//如果是初始状态，那么判断
if(reco == miLenPkgHeader)
{
    //拆解包头
    //这里拆解包头的过程，就是添加消息头，然后把包头的内容写到服务器的内存里,之后把状态切换到接受包体
    pConn->curStat = _PKG_BD_INIT;
}
else
{
    //没收完这里就是直接切换包头接受状态
    pConn->curStat = _PKG_HD_RECVING;
    pConn->precvbuf = pConn->precvbuf + reco;
    pConn->irecvLen = pConn->irecvLen - reco;
}


//如果当前状态是，正在接受包头状态
if(pConn->irecvLen == reco)
{
    //收完了就处理包头
}
else 
{
    //没有的话那就接着收
}

//如果是包体接受状态
if(pConn->irecvLen == reco)	//在包头拆解函数中，这里的长度已经修改为包体的长度，pConn->irecvLen的长度为包体的长度。
{
    //将收到的包，直接加入接受包的队列
    //同时把状态切换为初始状态
}
else
{
    //同理接着收
}

//如果是包体接受中状态，那么就和包头的情况做相同操作。
```

> 接受状态主要是有四个：
>
> - _PKG_HD_INIT，初始状态
> - _PKG_HD_RECVING，接受包头中，没接受完整
> - _PKG_BD_INIT，包头刚好收完，准备接受包体
> - _PKG_BD_RECVING，接受包体，直到接受完毕，回到初始状态。
>
> 





### 线程池

就是提前创建一批线程，并创建一个类来统一管理和调度这批线程（这一批线程构成一个池子，形象称为线程池。）收到一个任务（一个消息），就从这批线程中找一个空闲的去做这个任务（处理这个消息）。任务完成之后，线程中有个循环语句，可以使其循环回来等待新任务，就好像这个线程可以回收再利用一样。

之前在**子进程初始化**的部分，讲到初始化的时候初始化了一些线程，这些线程在创建之后，就一直处于运行阶段，只不过一直没有触发，那么如何触发这些线程的启动。

因为涉及到不同的线程做不同的事，所以不同的触发方式。

在**子进程初始化**部分有一个步骤就是创建线程池

```c++
//创建线程中的线程，要手工调用，不在构造函数里调用了
//return value，所有线程创建成功返回ture,出错误返回false
bool CThreadPool::Create(int threadNum)
{
    ThreadItem * pNew;      //首先定一个线程对象
    int err;

    miThreadNum = threadNum; //保存要创建线程的数量

    for(int i = 0; i < miThreadNum; ++i)
    {
        mThreadVector.push_back(pNew = new ThreadItem(this));
        err = pthread_create(&pNew->_Handle, nullptr, ThreadFunc, pNew);
        if(0 != err)
        {
            //创建线程失败
            NgxLogStderr(err, "CTheadPool::Create() create thread %d falied, return error value is %d",i, err);
            return false;
        }
        else
        {
            //创建成功，那就不做什么，也可以记录日志来标记谁做错了
        }
    }

    //我们必须保证每个线程都启动并运行到pthread_cond_wait(),本函数返回，只有这样，这几个线程才能进行后续的工作
    std::vector<ThreadItem*>::iterator iter;
lblfor:
    for(iter = mThreadVector.begin(); iter != mThreadVector.end(); ++iter)
    {
        if((*iter)->ifrunning == false) //这个条件保证所有线程启动起来，以保证整个线程池中的线程正常工作
        {
            //进到这里表示有没有完全启动的线程
            usleep(100 * 1000); //
            goto lblfor;
        }
    }
    return true;

}
```

> 注意，所有初始化的线程池，绑定的该线程需要运行的函数都一个名叫`ThreadFunc`的函数，这个函数只是一个控制函数，这个函数会主要的功能就是卡在一个循环里，等到满足条件时，才会执行功能，这个功能执行完，就继续卡在这个循环里面等待满足条件。
>
> `while((pThreadPoolObj->mMsgRecvQueue.size() == 0)&& mShutDown == false)`
>
> 这个条件满足了，也就是说会一直卡在这个循环里，当队列里面有队列后，就跳出这个循环，执行取出收到的消息的操作。

我们来看看一直让线程等待的函数是什么逻辑

```c++
void * CThreadPool::ThreadFunc(void * threadData)
{
    //这个是静态成员函数，是不存在this指针的
    ThreadItem * pThread = static_cast<ThreadItem*>(threadData);
    //这个代码就写的很无语，这里又用强制转换
    CThreadPool * pThreadPoolObj = pThread->_pThis;

    CMemory * pMemory = CMemory::GetInstance();

    int err;

    pthread_t tid = pthread_self();     //获取线程自身id，以方便调试打印信息等

    while(true)
    {
        //锁一下这个线程的互斥量
        err = pthread_mutex_lock(&mPthreadMutex);
        if(0 != err)
        {
            NgxLogStderr(err,"CThreadPool::ThreadFunc() pthread_mutex_lock() failed, return error value is %d",err);
        }

        //注意这种写法
        while((pThreadPoolObj->mMsgRecvQueue.size() == 0) && mShutDown == false)
        {
            //如果这个pthread_cond_wait被唤醒，【被唤醒后程序执行流程往下走的前提是拿到了锁】
            //pthread_cond_wait()返回时，互斥量再次被锁住
            //那么会立即执行outMsgRecvQueue()如果拿到了一个nullptr,则继续在这里wait

            if(pThread->ifrunning == false)
            {
                pThread->ifrunning = true;
                //标记了true了才允许调用StopAll(),否则会线程混乱
            }


            //刚开始执行pthread_cond_wait()，会卡在这里，而且mPthreadMutex会被释放掉
            //这里就是让线程开启一个等待状态，因为既没有拿到队列中的数据，线程也不是关闭状态
            pthread_cond_wait(&mPthreadCond, &mPthreadMutex);

        }

        //能走下来的必然是拿到了真正消息队列中的数据，或者是mShutDown ==  true;
        //跳出循环要注意还是处于锁状态，因为这个时候不会有别的线程往里面加数据
        if(mShutDown)
        {
            pthread_mutex_unlock(&mPthreadMutex);
            break;
        }

        //这里就是因为之前把那个取出队列消息的给换了判断，所以就是这里必须要把消息取走
        char * jobBuf = pThreadPoolObj->mMsgRecvQueue.front();
        pThreadPoolObj->mMsgRecvQueue.pop_front();
        --pThreadPoolObj->mIRecvMsgQueueCount;

        //然后再解锁互斥量
        err = pthread_mutex_unlock(&mPthreadMutex);
        if(0 != err) 
        {
            NgxLogStderr(err,"CTheadPool::ThreadFunc() pthread_cond_wait() failed");
        }

        //能走到这里的就是有数据处理
        ++pThreadPoolObj->miRunningThreadNum;       //正在运行的数量加一，这是一个原子变量，所以不用加互斥量

        //对提出的消息进行处理
        gSocket.ThreadRecvProcFunc(jobBuf);

        // NgxLogStderr(0,"execute begin----------tid=%ui",tid);
        // sleep(5);//测试代码
        // NgxLogStderr(0,"execute end------------tid=%ui",tid);

        pMemory->FreeMemory(jobBuf);
        --pThreadPoolObj->miRunningThreadNum;       //执行完了就-1嘛


    }//这里处理改变mShutdown的状态，没有别的办法走出这个循环阿

    return (void*)0;
}
```



如果来任务了，也就是之前的**收包**过程放入了对象，会调用

```c++
//来任务了，就调用一个线程下来干活
void CThreadPool::Call()
{
    int err = pthread_cond_signal(&mPthreadCond);
    //这个函数可以至少激发一个线程，激发有两种情况
    //1
    //线程常规情况下是卡在pthread_cond_wait，但是pthread_cond_signal
    //就能激发这个线程执行流程走下来。下来是一个while循环，这里第一个取出队列
    //元素的判断条件肯定不成功，所以就跳出循环，执行后面的逻辑
    //唤醒一个等待该条件的线程
    if(0 != err)
    {
        NgxLogStderr(err,"CThreadPool::Call()'s thread_cond_signal() failed return error value is %d",err);
    }

    //如果线程全部忙，那就要报警
    if(miThreadNum == miRunningThreadNum)
    {
        //线程不够用了
        time_t curTime = time(NULL);
        if(curTime - miLastEmgTime > 10) //每间隔10秒报一次警
        {
            miLastEmgTime = curTime;
            NgxLogStderr(0,"CTheadPool::Call*() have no thread, ");
        }
    }

    return;
}
```





除了该初始的线程之外，也就是是将接受的消息进行处理的线程函数之外，在子进程初始化时还有几个线程的函数，同样也是通过这种方法进行调用。**卡死在一个死循环里，有变量的信息改变后，进行处理。**



到此时，我们可以写出该`epoll`技术结合线程池的运行流程图。

![](D:\Learn\C++Learn\workspace\project\LinuxNetwork\serverFlowChart.png)





