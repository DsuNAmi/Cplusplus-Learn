# 域名及网络地址

除非商业需要，一般不会不会轻易改变域名，而是IP地址。所以在程序中一般使用域名。



##### 利用域名获取IP地址。

```c++
struct hostent * gethostbyname(const char * name);
//成功时返回hostent地址，失败时返回nullptr;

//结构体表示如下
struct hostent
{
    char * h_name;			//offical name
    char ** h_aliases;		//alias list
    int h_addrtype;			//host address type
    int h_length;			//addres length
    char ** h_addr_list;	//addres list
};
```



###### h_name

官方域名

###### h_aliases

可以通过多个域名访问同一主页，也就是相当于别名。

###### h_addrtype

IPV4和IPV6

###### h_length

IP地址长度

###### h_addr_list

通过此变量以整数形式保存域名对应的IP地址。一个域名可能有多个地址。



##### 利用IP地址获取域名

```c++
struct hostent * gethostbyaddr(const char * addr, int len, int type);
//成功时返回hostent地址，失败时返回nullptr。
```



