# 多进程服务器端——Linux部分

## 进程概念及应用

- **多进程服务器：**通过创建多个进程提供服务。（windows不支持）
- **多路复用服务器：**通过捆绑并统一I/O对象提供服务器。
- **多线程服务器：**通过生成与客户端等量的线程提供服务。

## 理解进程

进程定义：**占用内存空间的正在运行的程序**

CPU中核的个数与可同时运行的进程数相同。相反，如果进程数超过核数，进程将分时使用CPU资源。但因为CPU运转速度极快，我们会感到进程同时运行。

进程ID：**其值为大于2的整数，因为在Linux中，1号进程要分配给首个进程**



###### 通过fork函数创建进程

```c++
#include <unistd.h>

pid_t fork(void);
//成功时返回进程ID，失败时返回-1
```

`fork`函数将创建调用的进程副本。也就是说，并非根据完全不同的程序创建进程，而是复制正在运行的、调用`fork`函数的进程。另外，两个进程都将执行`fork`函数调用后的语句。但因为通过同一个进程、复制相同的内存空间，之后的程序流要根据`fork`函数的返回值加以区分。即利用`fork`函数的如下特点区分程序执行流程

- 父进程：`fork`函数返回子进程ID
- 子进程：`fork`函数返回0

此处父进程是指原进程，即调用`fork`函数的主体，而子进程是通过父进程调用`fork`函数复制出的进程。

```c++
//父进程
int gval = 10;
int main(void)
{
    int lval = 20;
    lval += 5;
    gval++;
    pid_t pid = fork();//pid为子进程，复制发生点。
    if(pid == 0)
    {
        gval++;
	}
    else
    {
        lval++;
    }
    ...;
}

//子进程
//gval复制为11
int main()
{
    //lval复制为25
    pid_t pid = fork();
    if(pid == 0)
    {
        gval++;		//执行这一句话
    }
    else
    {
        lval++;
    }
}

复制完成后根据fork函数的返回值区分父子进程。父进程将lval的值加一，但这不会影响到子进程的lval值，同样，子进程将gval的值加1，也不会影响到父进程的gval。因为fork函数调用后分成了完全不同的进程，只是二者共享同一段代码而已。
```



来看一个`fork`的例子：

```c++
#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char * argv[])
{
    pid_t pid;
    int lval = 20;
    gval++;
    lval += 5;
    pid = fork();
    if(pid == 0)	//if child process
    {
        gval += 2;
        lval += 2;
    }
    else 			//if parent process
    {
        gval -= 2;
        lval -= 2;
    }
    if(pid == 0)
    {
        printf("Child Proc: [%d, %d] \n", gval, lval);
	}
    else
    {
        printf("Parent Proc: [%d, %d] \n", gval, lval);
    }
    
    return 0;
}

//输出
Child Proc : [13,27]
Parent Proc : [9, 23]
```



## 僵尸进程

进程完成工作后应该被销毁，但有时这些进程将变成僵尸进程，占用系统中的重要资源。

###### 产生僵尸进程的原因

我们先来看看程序的终止方式：

- 传递参数并调用`exit`函数。
- `main`函数中执行`return`语句并返回值。

向`exit`函数传递的参数值核`main`函数的`return`语句返回的值都会传递给操作系统。而操作系统不会销毁子进程，直到把这些值传递给产生该子进程的父进程。处在这种状态下的进程就是僵尸进程。

那么僵尸进程应该何时被销毁呢？

**应该向创建子进程的父进程传递子进程的exit参数或return语句的返回值**

但是操作系统不会主动把这些值传递给父进程。只有父进程主动发起请求时，操作系统才会传递该值。

来看一个创建僵尸进程的例子：

```c++
#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    pid_t pid = fork();
    
    if(pid == 0)		//if child process
    {
        puts("Hi, I am a child process.");
	}
    else
    {
        printf("Child Process ID %d \n",pid);
        sleep(30);		//sleep 30 sec;
    }
    
    if(pid == 0)
    {
        puts("End child process");
    }
    else
    {
        puts("End parent process");
    }
    
    return 0;
}

//输出
Hi, I am a child process
End child process
Child process ID : 10977
```



要等到父进程结束后，僵尸子进程才会销毁。



###### 销毁僵尸进程1：利用wait函数

```c++
#include <sys/wait.h>

pid_t wait(int * statloc);
//成功时返回终止子进程的ID，失败时返回-1
//调用此函数时如果已有子进程终止，那么子进程终止时传递的返回值（exit函数的参数值，main函数的return返回值）将保存到该函数的参数所指内存空间。但函数参数指向的单元中还包含其他信息，因此需要下列宏来分离：
WIFEXITED 子进程正常终止时返回TRUE;
WEXITSTATUS 返回子进程的返回值;
```



来看一个例子：

```c++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
    int status;
    pid_t pid = fork();
    if(pid == 0)
    {
        return 3;
    }
    else
    {
        printf("Child PID : %d \n", pid);
        pid = fork();
        if(pid == 0)
        {
            exit(7);
        }
        else
        {
            printf("Child PID : %d \n", pid);
            wait(&status);
            if(WIFEXITED(status))		//是否正常终止？
            {
                printf("Child send one : %d \n", WEXITSTATUS(status));	//返回值是多少？
            }
            wait(&status);
            if(WIFEXITED(status))
            {
                printf("Child send two : %d \n", WEXITSTATUS(status));
            }
            sleep(30);
        }
    }
    
    return 0;
}


//输出
Child PID : 12337
Child PID : 12338
Child send one : 3
Child send two : 7
```

> 调用`wait`函数时，如果没有已终止的子进程，那么程序将阻塞直到子进程终止，因此需要谨慎调用该函数。



###### 销毁僵尸进程2：使用waitpid函数

```c++
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int * statloc, int options);
//成功时返回终止的子进程ID（或0），失败时返回-1
pid：等待终止的目标子进程ID，若传递-1，则与wait函数相同，可以等待任意子进程结束;
statloc: 与wait函数中含义相同;
options: 传递WNOHANG，即使没有终止的子进程也不会进入阻塞状态，而是返回0并退出函数
```



```c++
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
    int status;
    pid_t pid = fork();
    if(pid == 0)
    {
        sleep(15);
        return 24;
	}
    else
    {
        while(!waitpid(-1,&status,WNOHANG))
        {
            sleep(3);
            puts("sleep 3sec.");
        }
        
        if(WIFEXITED(status))
        {
            printf("Child send %d \n", WEXITSTATUS(status));
        }
    }
    
    return 0;
}

//输出
sleep 3sec.
sleep 3sec.
sleep 3sec.
sleep 3sec.
sleep 3sec.
Child send 24
```



## 信号处理

子进程究竟何时终止？调用waitpid函数后要无休止的等待吗？

###### 信号与signal函数

```c++
#include <signal.h>

void (*signal(int signo, void (*func)(int)))(int);
//为了在产生信号时调用，返回之前注册的函数指针。
函数名：signal
参数类型：int signo, void (*func)(int)
返回类型：参数类型int，返回类型void的函数指针。
    
//一些对应的常数;
SIGALRM: 已到通过带哦也能够alarm函数注册的时间;
SIGINT: 输入CTRL+C;
SIGCHLD: 子进程终止。
```



注册过程。

```c++
void keycontrol(int);
signal(SIGINT,keycontrol);
```



```c++
//alarm函数
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
//返回0或以秒为单位的距SIGALRM信号发生所剩的时间
```



如果调用该函数的同时为它传递一个正整数参数，相应时间后将产生SIGALRM信号。如果向该函数传递0，则之前对SIGALRM信号的预约将取消。如果通过该函数预约信号后未指定该信号对应的处理函数，则终止进程，不做任何处理。



来看一个例子：

```c++
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if(sig == SIGALRM)
    {
        puts("Time out!");
    }
    alarm(2);		//每隔两秒产生SIGALRM信号
}

void keycontrol(int sig)
{
    if(sig == SIGALRM)
    {
        printf("CRTL+C pressed");
    }
}

int main(int argc, char * argv[])
{
    int i;
    signal(SIGALRM, timeout);		
    signal(SIGINT,keycontrol);			//注册
    alarm(2);							//预约两秒后发生SIGALRM信号
    
    for(int i = 0; i < 3; ++i)
    {
        puts("wait....");
        sleep(100);
    }
    
    return 0;
}

//输出
wait...
Time out!
wait...
Time out!
wait...
Time out!
```



上述是没有任何输入时的输出结果。下面在运行过程中输入CTRL+C，可以看到输出`CRTL+C pressed`。



注意：

**发生信号时将唤醒由于调用sleep函数而进入阻塞状态的进程。**

因为每隔两秒都会调用信号，所以不需要等300秒程序就会结束。



###### 利用sigaction函数进行信号处理

```c++
#include <signal.h>

int sigaction(int signo, const struct sigaction * act, struct sigaction * oldact);
//成功时返回0，失败时返回-1
act: 对应于第一个参数的信号处理函数信息;
oldact: 通过此参数获取之前注册的信号处理函数指针，若不需要则传递0;

//结构体定义
struct sigaction
{
    void (*sa_handler)(int);
    sigset_t sa_mask;
    int sa_flags;
}
```



来看一个例子：

```c++
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if(sig == SIGALRM)
    {
        puts("Time out");
    }
    alarm(2);
}

int main(int argc, char * argv[])
{
    int i;
    strcut sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);//初始化为0
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, 0);
    
    alarm(2);
    
    for(int i = 0; i < 3; ++i)
    {
        puts("wait...");
        sleep(100);
    }
    return 0;
}

//输出相同
```



###### 利用信号处理技术消灭僵尸进程

```c++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1,&status,WNOHANG);
    if(WIFEXITED(status))
    {
        printf("Removed proc id: %d \n", id);
        printf("Child send : %d \n", WEXITSTATUS(status));
    }
}


int main(int argc, char * argv[])
{
    pid_t pid;
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD,&act,0);
    pid = fork();
    if(pid == 0)	//子进程执行
    {
        puts("Hi I'm child process.");
        sleep(10);
        return 12;
    }
    else			//父进程执行
    {
        printf("Child proc id : %d \n", pid);
        pid = fork();
        if(pid == 0)
        {
            puts("Hi! I'm child process.");
            sleep(10);
            exit(24);
		}
        else
        {
            int i;
            printf("Child porc id: %d \n",pid);
            for(int i = 0; i < 5; ++i)
            {
                puts("wait...");
                sleep(5);
            }
        }
    }
    
    return 0;
}

//输出
Hi! I m child process;
Child proc id: 9529;
Hi! I m child process;
Child proc id : 9530;
wait...;
wait...;
Removed proc id : 9530;
Child send 24;
wait...;
Removed proc id : 9529;
Child send 12;
wait...;
wait...;
```



## 基于多任务的并发服务器

每当有客户端请求服务时，回声服务器端都创建子进程以提供服务。请求的客户端有5个，那将创建5个子进程提供服务。

- 第一阶段：回声服务器（父进程）通过调用accept函数受理连接请求。
- 第二阶段：此时获取的套接字文件描述符创建并传递给子进程。
- 第三阶段：子进程利用传递来的文件描述符提供服务。

```c++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void error_handing(char *message);
void read_childproc(int sig);

int main(int argc, char * argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serc_adr, clnt_adr;
    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];
    if(argc != 2)
    {
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
	}
    
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHILD,&act,0);
    serv_sock = socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr,sin_port = htons(atoi(argv[1]));
    
    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    
    listen(serv_sock,5);
    
    while(1)
    {
        adr_sz = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr,&adr_sz));
        pid = fork();
        if(pid == -1)
        {
            close(clnt_sock);
            continue;
        }
        else if(pid == 0)
        {
            close(serv_sock);
            while((str_len = read(clnt_sock,buf,BUF_SIZE)) != 0)
            {
                write(clnet_sock,buf,str_len);
            }
            close(clnt_sock);
            return 0;
        }
        else close(clnt_sock);
    }
    
    close(serv_sock);
}

void read(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1,&status,WNOHANG);
}

```





