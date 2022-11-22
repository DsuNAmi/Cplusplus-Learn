//主要是与打印格式相关的函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>



#include "ngx_global.h"
#include "ngx_macro.h"
#include "ngx_func.h"


static u_char * NgxSprintNum(u_char * buf, u_char * last, uint64_t ui64,
                            u_char zero, uintptr_t hexadecimal, uintptr_t width);


u_char * NgxSLPrintf(u_char * buf, u_char * last, const char * fmt, ...)
{
    va_list args;
    u_char *p;

    va_start(args, fmt);
    p = NgxVSLPrintf(buf, last, fmt, args);
    va_end(args);
    return p;
}
u_char * NgxSNPrintf(u_char * buf, size_t max, const char * fmt, ...)
{
    u_char *p;
    va_list args;
    va_start(args, fmt);
    p = NgxSLPrintf(buf,buf + max, fmt, args);
    va_end(args);
    return p;
}


//对于nginx自定义的数据结构进行标准化输出
/****
 * buf 数据放在这里
 * last 放置的数据不要超过这里
 * fmt 以这个为首的可变参数
 * NgxLogStderr(0,"invalid option: \"%s\", %d", "testinfo", 123);
 * fmt = "invalid option: \"%s\", %d"
 * args = "testinfo", 123
*/
u_char * NgxVSLPrintf(u_char * buf, u_char * last, const char * fmt, va_list args)
{
    u_char zero;

    //一些临时变量
    uintptr_t width;
    uintptr_t sign;
    uintptr_t hex;
    uintptr_t frac_width;
    uintptr_t scale;
    uintptr_t n;

    int64_t     i64;    //保存%d对应的可变参数
    uint64_t    ui64;   //保存%ud对应的可变参数，临时作为%f的可变参数也可以
    u_char      *p;     //保存%s对应的可变参数
    double      f;      //保存%f对应的可变参数
    uint64_t    frac;   //%f可变参数，根据%.2f，取得小数点后面的位

    while(*fmt && buf < last)   //每次处理一个字符，是变量fmt的字符
    {
        if(*fmt == '%')
        {
            zero = (u_char) ((*++fmt == '0') ? '0' : ' ');      //这个地方用来填充字符

            width = 0;
            sign = 1;
            hex = 0;
            frac_width = 0;
            i64 = 0;
            ui64 = 0;

            //变量初始化结束

            //取出%后面数字，如果是有前导零第一个已经处理
            while(*fmt >= '0' && *fmt <= '9')
            {
                width = width * 10 + (*fmt++ - '0');
            }
            //把%后面的数字读完，忽略第一个前导零后面的所有前导零
            //之后我们根据不同的格式来作相关的处理

            for(;;)
            {
                switch(*fmt)
                {
                    case 'u':       //这个是无符号的标记，后面按理说应该还有
                        sign = 0;
                        fmt++;
                        continue;
                    case 'X':       //十六进制，也不是单独使用，继续往后走
                        hex = 2;
                        sign = 0;
                        fmt++;
                        continue;   
                    case 'x':       //同理，十六进制，不过这种写法是输出小写的十六进制
                        hex = 1;
                        sign = 0;
                        fmt++;
                        continue;

                    case '.':   //这里后面必须跟数字，而且后面必须是.2f类似的表达形式
                        fmt++;
                        while(*fmt >= '0' && *fmt <= '9')
                        {
                            frac_width = frac_width * 10 + (*fmt++ - '0');
                        }
                        //读完小数位
                        break;

                    default:
                        //这里其他错误情况不做判断吗？
                        //直接退出这个无限循环
                        break;
                }
                break;  ///这里忘记退出死循环了
            }


            //这里应该是把对应的格式读完了
            switch(*fmt)
            {
                case '%':       //唯一一种情况，%%，打印一个%
                    *buf++ = '%';
                    fmt++;
                    continue;       //这里是直接走大循环了，也就是这个%的内容已经读完了
                
                case 'd':
                    //这里就要结合之前的来判断了，看是有符号还是无符号
                    if(sign)
                    {
                        i64 = (int64_t) va_arg(args, int);  //按当前位置取出那个参数，并指向下一个参数
                    }
                    else
                    {
                        ui64 = (uint64_t) va_arg(args, u_int);
                    }
                    break;

                case 'i':
                    if(sign)
                    {
                        i64 = (int64_t)va_arg(args, intptr_t);
                    }
                    else
                    {
                        ui64 = (uint64_t)va_arg(args, u_int);
                    }
                
                case 's':   //字符串的情况
                    p = va_arg(args, u_char*);

                    while (*p && buf < last)
                    {
                        *buf++ = *p++;          //这里是直接给了这个地方的地址阿，没有复制
                    }
                    fmt++;
                    continue;       //字符串情况读完了

                case 'P':           //转换一个pid_t类型
                    i64 = (int64_t) va_arg(args, pid_t);
                    sign = 1;
                    break;      //和数字读法一样

                case 'f':   //浮点数的情况
                    f = va_arg(args, double);
                    if(f < 0)
                    {
                        *buf++ = '-';
                        f = -f;
                    }
                    //按照正数处理
                    ui64 = (int64_t) f;//取正数部分
                    frac = 0;

                    if(frac_width)  //按照要求的小数位读取
                    {
                        scale = 1;
                        for(n = frac_width; n; --n)
                        {
                            scale *= 10;        //这里肯定会溢出吧。。。
                        }
                        //把小数部分取出来
                        frac =(uint64_t) ((f - (double)ui64) * scale + 0.5);        //这还来个四舍五入

                        if(frac == scale)
                        {
                            //进位
                            ui64++;
                            frac = 0;
                        }
                    }

                    buf = NgxSprintNum(buf,last,ui64,zero,0,width);
                    //先把正数往里面写

                    if(frac_width)
                    {
                        if(buf < last)
                        {
                            *buf++ = '.'; //先整个小数点
                        }
                        buf = NgxSprintNum(buf, last, frac, '0', 0, frac_width);    //这后面的小数点补零
                        //这里是不是有点问题阿，如果是5位小数，末尾小数是2为，这0不是补前面了吗？
                        //果然是有问题的，但是用错误掩盖了错误，这里参用了一种可能溢出的情况
                        //其实作者没有解决这个问题，如果是小数的话，应该要反过来写，但是作者
                        //这里直接把小数位补零了，就是当成了正数，这里‘0’改成' 会影响后面是零的是输出，但是如果小数不是0就不会影响

                    }
                    fmt++;
                    continue;   //小数读完、

                //还可以继续补别的数据类型
                default:
                    *buf++ = *fmt++;    //没写的默认当普通字符
                    continue;       //这里是不要影响其他流程
            }

            //同意把显示的数字保存到ui64中
            if(sign)
            {
                if(i64 < 0)
                {
                    *buf++ = '-';
                    ui64 = (int64_t) -i64;      //这里负数转正数，其实会有溢出问题
                }
                else
                {
                    ui64 = (uint64_t) i64;
                }
            }

            buf = NgxSprintNum(buf,last,ui64,zero,hex,width);//把数字往里面写
            fmt++;
            //继续往后面读
        }
        else    //当成正常字符,直接保存到内容里
        {
            *buf++ = *fmt++; //*和++优先级相同，但是buf++是先用的，所以就是先用后加
        }
    }

    return buf;
}


//这个函数用来格式化显示数字
static u_char * NgxSprintNum(u_char * buf, u_char * last, uint64_t ui64,
                            u_char zero, uintptr_t hexadecimal, uintptr_t width)
{
    u_char      *p;
    u_char      temp[NGX_INT64_LEN + 1];
    size_t      len;
    uint32_t    ui32;

    static u_char   hex[] = "0123456789abcdef";  //跟把一个10进制数显示成16进制有关，换句话说和  %xd格式符有关，显示的16进制数中a-f小写
    static u_char   HEX[] = "0123456789ABCDEF";  //跟把一个10进制数显示成16进制有关，换句话说和  %Xd格式符有关，显示的16进制数中A-F大写

    p = temp + NGX_INT64_LEN;           //直到最后一个位置

    if(hexadecimal == 0)
    {
        if(ui64 <= (uint64_t) NGX_MAX_UINT32_VALUE)
        {
            ui32 = (int32_t) ui64;
            do
            {
                *--p = (u_char)(ui32 % 10 + '0');       //这里是取模倒着装
            }
            while(ui32 /= 10);      //把数字缩小
        }
        else
        {
            //不然就用默认的ui64放了，感觉有点多此一举阿
            do
            {
                *--p = (u_char)(ui64 % 10 + '0');
            } while (ui64 /= 10);
            
        }
    }
    else if(hexadecimal == 1)
    {
        //输出小写的16进制
        do
        {
            *--p = hex[(uint32_t)(ui64 & 0xf)];
        } while (ui64 >>= 4);
    }
    else if(hexadecimal == 2)
    {
        //输出大写的16进制情况
        do
        {
            *--p = HEX[(uint32_t)(ui64 & 0xf)];
        } while (ui64 >>= 4);
    }

    len = (temp + NGX_INT64_LEN) - p;       //得到这个数字的位数

    //下面处理填充
    while(len++ < width && buf < last)
    {
        *buf++ = zero;      //如果是有前导零的话前面是补0不是补空格
        //这里应该是保存了整个宽度的长度，但是如果数字长度比想要表达的宽度的话，
        //应该就不会进这个循环了，如果是只有小于的情况才会补0
    }

    len = (temp + NGX_INT64_LEN) - p; //之前的指针动了，其实用两个也是一样

    if((buf + len) >= last)
    {
        //不够了，能够放多少我就放多少
        len = last - buf;
    }

    return ngx_memcpy(buf, p, len);     //把读出来的数字字符串往buf中写

}