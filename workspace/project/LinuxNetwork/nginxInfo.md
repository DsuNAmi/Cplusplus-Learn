# NGINX

###### 源码目录

```bash
- root
	-- auto (主要是shell脚本)
		-- cc    (里面主要是检查编译器的脚本)
		-- lib   (里面主要是检查依赖库的脚本)
		-- os    (里面主要是检查操作系统的脚本)
		-- types (里面主要是检查平台类型的脚本)
	-- CHANGES (修复的bug和新增功能的说明)
	-- CHANGES.ru (俄语版本)
	-- conf (默认的配置文件)
	-- configure (编译文件)
	-- contrib (脚本和工具)
	-- html (有欢迎界面和错误的界面的文件)
	-- man (帮助目录)
	-- src (核心源码目录)
		-- core (核心代码)
		-- event (内有event事件模块相关代码)
		-- http (内有http模块相关代码)
		-- mail (内有邮件模块相关代码)
		-- stream (流处理相关代码)
```

![](D:\Learn\C++Learn\workspace\project\LinuxNetwork\nginxConfigure.png)

查看 `nginx`是否启动

```bash
ps -ef | grep nginx
```

启动 `nginx`

```bash
pwd
- /usr/local/nginx/sbin
sudo ./nginx
```

查看虚拟机处理器个数

```bash
grep - c processor /proc/cpuinfo
```

修改 `nginx`的默认处理器个数

```bash
vim /urs/local/nginx/conf/nginx.conf

#change the work_processors 1 to 4
```

不中断 `nginx`服务器的情况下，重新生效新的配置

```c++
sudo ./nginx -s reload
```

关闭服务器

```bash
sudo ./nginx -s quit 	#优雅的关闭方式，只是不接受新的连接
sudo ./nginx -s stop 	#粗暴的关闭方式，直接down服务器
```



现在对于`linux`系统，如果直接杀掉终端的话，通过该终端创建的`nginx`程序也会被断掉，我们来看一个例子

```c
#include <stdio.h>
#include <unistd.h> //预编译

int main(int argc, char * const * argv)
{
    printf("Happy to learn Linux Communication\n");
    
    for(;;)
    {
        sleep(1);
        printf("rest for one second\n");
    }
    
    printf("exit gooddbye!\n");
    
    return 0;
}
```



我们要通过何种方法使得该程序能够在终端杀死时，继续运行？

###### 拦截SIGHUP信号

```c
#include <stdio.h>
#include <unistd.h> //预编译
#include <signal.h>

int main(int argc, char * const * argv)
{
    printf("Happy to learn Linux Communication\n");
    
    //系统函数，设置收到某个信号时的处理程序
    signal(SIGHUP,SIG_IGN);
    //SIG_IGN: 代表我要求忽略该信号，请求操作系统不要执行默认的该信号处理动作，也就是把该进程杀掉
    
    for(;;)
    {
        sleep(1);
        printf("rest for one second\n");
    }
    
    printf("exit gooddbye!\n");
    
    return 0;
}
```



###### nginx进程和bash进程在不同的session中

```c
//这时候要创建一个新的子进程
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char * const * argv)
{
    pid_t pid;
    
    printf("Happy to learn Linux Communication\n");
    
    //系统函数，用来创建新进程。子进程会从这句fork调用之后开始执行，原来的父进程也会接着往下运行
    pid = fork();
    
    if(pid < 0)
    {
        printf("Error to create subProcess\n");
	}
    else if(pid == 0)
    {
        //如果执行的是子进程，就进入这个条件
        printf("suProcess area\n");
        
        //创建新的session
        setsid();
        for(;;)
        {
            sleep(1);
            printf("subProcess rest for one second\n");
        }
        return 0;
	}
    else
    {
        //父进程
        for(;;)
        {
            sleep(1);
            printf("fatherProcess rest for one second\n");
        }
        return 0;
	}
    
    printf("程序退出\n");
    
    return 0;
    
}
```



###### 系统方法

代码还是最初的代码

```bash
setsid ./nginx
```



```bash
nohup ./nginx
```



```bash
./nginx &

fg #切换后台进程前台
```



如果遇到杀不死的进程，使用命令`kill -9`，因为这个终止信号是不能被程序忽略的。



###### 可重入函数

指在信号处理函数中调用是安全是的函数。

###### 信号处理的规范

1. 在信号处理函数中，应尽量使用简单的语句做简单的事，尽量不要调用系统函数以免引起麻烦。
2. 如果必须在信号处理函数中调用系统函数，只调用可重入函数，不要调用不可重入函数。
3. 如果必须在信号处理函数中调用那些可能修改`errno`值的可重入系统函数，应该事先备份`errno`的值，事后再从信号处理函数返回之前恢复`errno`的值。



###### 信号处理的一个案例

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sig_quit(int signo)
{
    printf("Recive SIGQUIT signal!\n");
}


int main(int argv, char * const * argc)
{
    sigset_t newmask;
    sigset_t oldmask;
    sigset_t pendmask;


    if(signal(SIGQUIT, sig_quit) == SIG_ERR)
    {
        printf("can not catch the SIGUSR1 signal\n");
        exit(1);

    }


    sigemptyset(&newmask);  //newmask信号都置为零
    sigaddset(&newmask, SIGQUIT);// 设置newmask信号集中的SIGQUT信号位1，
                                 // 再来信号就收不到

    //设置该进程所对应的信号集
    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    //第1个参数用了SIG_BLOCK，表明设置进程新的信号屏蔽字为当前信号屏蔽字
    //和第2个参数指向的信号集的并集。一个进程的的当前信号屏蔽字，开始全部为0
    //相当于把当前进程的信号屏蔽字设置成newmask（屏蔽了SIGQUIT）第三个参数不为空
    //则进程老信号集会保存到第3个参数里，以备后续恢复使用
    {
        printf("sigprocmask(SIG_BLOCK)失败\n");
        exit(1);
    }

    printf("Start sleep 10 seconds ------begin------\n");
    sleep(10);
    printf("End Sleep 10 seconds-------end------\n");
    
    if(sigismember(&newmask, SIGQUIT))  //测试一个位置的信号是否被置为
    {
        printf("SIGOUIT out\n");
    }
    else 
    {
        printf("SIGQUIT in !!!!\n");
    }

    if(sigismember(&newmask,SIGHUP))
    {
        printf("SIGHUP out\n");
    }
    else
    {
        printf("SIGHUP in!!!\n");
    }

    //取消对SIGQUIT的屏蔽
    //第一个参数用了SIGSETMASK表明设置新的信号屏蔽字为第二个参数指向的信号集，
    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        printf("sigprocmask(SIG_SETMASK) failed\n");
        exit(1);
    }

    printf("successed!\n");

    if(sigismember(&oldmask, SIGQUIT))
    {
        printf("SIGQUIT out\n");
    }
    else
    {
        printf("can sees signal SIGQUIT, but I need to sleep 10 seconds\n");
        int mysl = sleep(10);
        if(mysl > 0)
        {
            printf("Remain %d s\n", mysl);
        }
    }

    printf("goodbye!\n");

    return 0;
}
```



### FORK函数详解、范例

在一个进程中可以使用`fork`创建一个子进程，当该子进程创建时，**它从fork函数的下一条语句（或者说从fork的返回处）开始执行玉父进程相同的代码**。换句话说，fork函数产生一个和当前进程完全一样的新进程，并和当前进程一样从fork函数调用中返回。



fork函数，对于子进程来说，返回的值是0；对于父进程来说，返回的值是子进程的PID。



创建一个新进程之后，父进程和子进程的执行先后顺序是不确定的。



父进程和子进程的全局变量也不相同，每个进程都有不同的值。



在`linux`系统中，如果一个父进程创建的子进程死了，但是父进程还活着，在没有进行其他处理的情况下，这个子进程会变成一个僵尸进程。



我们可以通过编写一段信号捕捉机制来捕捉僵尸进程。

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>



void sig_usr(int signo)
{
    // printf("receive signal SIGUSR1, Processor ID = %d,", getpid());
    int status;
    switch (signo)
    {
    case SIGUSR1:
        printf("receive SIGUSR1, ProcessorID = %d",getpid());
        break;
    case SIGCHLD:
        printf("receive SIGCHLD, ProcessorID = %d", getpid());
        pid_t pid = waitpid(-1,&status, WNOHANG);
        //第一个参数，-1,表示等待任何进程
        //第二个参数，保存子进程的状态信息
        //第三个参数，表示不阻塞这个函数
        if(pid == 0 || pid == -1) return;
        break;
    default:
        printf("other----%d\n",signo);
        break;
    }

    return;
}


int main(int argc, char * const * argv)
{
    pid_t pid;

    printf("process on!\n");

    if(signal(SIGCHLD, sig_usr) ==SIG_ERR)
    {
        printf("can not catch the SIGUSR1\n");
    }

    pid = fork();

    if(pid < 0)
    {
        printf("failed to fork a subProcessor\n");
        exit(1);
    }


    for(;;)
    {
        sleep(1);
        printf("rest, processor ID = %d\n", getpid());
    }

    return 0;
}
```



```c
((fork() && fork()) || (fork() && fork()))
#创建7个进程
```



###### fork失败的情况

- 系统中进程太多。
- 创建的进程数超过了当前用户允许创建的最大进程数



### 守护进程

守护进程是一种长期运行的进程，这种进程在后台运行，不与任何控制终端关联。

1. 生存期长，一般操作系统启动的时候它就启动，操作系统关闭的时候就关闭。
2. 守护进程跟哪个终端都没有关联。TTY那一列显示的都是？
3. 守护进程是在后台运行的。
4. 大多数守护进程都是超级用户特权运行的。

###### 守护进程编写规则

- 调用`umask(0)`。
- fork一个子进程，然后父进程退出。
  - 可以把运行该进程的终端空闲出来。
  - 可以调用`setsid()`创建新会话。
  - 不关联终端。用了新的PID。

在守护进程中经常使用如下代码：

```c
int fd;
fd = open("/dev/null", O_RDWR);		//以读写方式打开黑洞设备

dup2(fd, STDIN_FILENO);			//先关闭STDIN_FILENO(这是规矩，已经代开的描述符，改动之前先关闭)，类似指针指向null，让/dev/null成为标准输入
dup2(fd, STDOUT_FILENO);		//先关闭STDOUT_FILENO，让/dev/null成为变成标准输出

if(fd > STDERR_FILENO) close(fd);

//dup2：复制文件描述符。
```



一段与守护进程有关的代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/stat.h>
#include <fcntl.h>

//创建守护进程
int ngx_daemon()
{
    int fd;

    switch(fork())
    {
        case -1:
            //failed to create subProcessor
            return -1;
        case 0:
            break;
        default:
            exit(0);        //父进程直接删除
    }



    //just subProcessor can flow here
    if(setsid() == -1)
    {
        //failed log
        return -1;
    }
    
    umask(0);           //不要让他限制文件权限

    fd = open("/dev/null",O_RDWR);

    if(-1 == fd)
    {
        //failed log
        return -1;
    }

    if(dup2(fd, STDIN_FILENO) == -1)
    {
        //failed log
        return -1;
    }

    if(dup2(fd, STDOUT_FILENO) == -1)
    {
        //failed log
        return -1;
    }

    if(fd > STDERR_FILENO)
    {
        if(close(fd) == -1)     //realeas source
        {
            return -1;
        }
    }  
}

int main(int argc, char * const * argv)
{
    if(-1 == ngx_daemon())
    {
        return -1;
    }
    else
    {
        for(;;)
        {
            sleep(1);
            printf("rest 1 second, Processor ID = %d \n", getpid());
        }
    }
    
    return 0;
}
```



###### 守护进程与后台进程的区别

- 守护进程和终端不挂钩，不向终端输出内容，但后台进程是能向终端输出内容的
- 守护进程在关闭终端的时候不会收到影响，而后台进程在关闭终端的时候会自动退出。