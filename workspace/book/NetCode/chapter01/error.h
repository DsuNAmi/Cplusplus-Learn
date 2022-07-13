#ifndef __ERROR__
#define __ERROR__

const char * StartUpError = "WSAStartup() error!";
const char * SocketError = "socket() error!";
const char * BindError = "bind() error!";
const char * ListenError = "listen() error!";
const char * AcceptError = "accept() error!";
const char * ConnectError = "connect() error!";
const char * SendError = "send() error!";
const char * RecvError = "recv() error!";

const int SERVER = 1;
const int CLIENT = 2;

void ParamsFormat(char * fileName, int argc)
{
    if(argc == SERVER)
    {
        printf("Usage : %s <port>",fileName);
    }
    else if(argc == CLIENT)
    {
        printf("Usage : %s <IP> <Port>",fileName);
    }
    exit(1);
}

#endif