#ifndef __NGX_COMM_H__
#define __NGX_COMM_H__

#define _PKG_MAX_LENGTH         30000 //每个包的最大长度

//通信，收包状态定义
#define _PKG_HD_INIT            0   //初始状态
#define _PKG_HD_RECVING         1   //接受包头中，包头不完整，继续接受
#define _PKG_BD_INIT            2   //包头刚好收完，准备接受包体
#define _PKG_BD_RECVING         3   //接受包体，不完整就继续接受，回到0状态



#define _DATA_BUFSIZE_      20      //因为我要先收包头，我希望定义一个固定大小的数组专门用来收包头，这个数字大小一定要 >sizeof(COMM_PKG_HEADER) ,所以我这里定义为20绰绰有余


//结构定义
#pragma pack(1)     //对齐方式，1字节对齐

typedef struct _COMM_PKG_HEADER
{
    unsigned short pkgLen;      //包头+包体总长度，--2字节
    unsigned short msgCode;     //消息类型2字节，用于区别每个不同的命令
    int            crc32;       //crc校验，4字节，
} COMM_PKG_HEADER, *LPCOMM_PKG_HEADER;

#pragma pack()      //取消指定对齐

#endif