# 服务器程序框架初步

#### TIME_WAIT状态

如果是服务器先断开连接，当服务器接受都客户端发送的FIN标志数据包之后，会在回复ACK之后进入TIME_WAIT状态。

（1）第一个目的就是说，TIME_WAIT要保证客户端接收到ACK，因为如果客户端没有接收到ACK，会重发一个FIN，这时候TIME_WAIT的存在就可以接收到这个重发的FIN，不然提前断开的话，就会出现如果客户端没有收到服务器发送的ACK，那么客户端会一直在LAST_ACK状态，这样并不友好。TIME_WAIT会在一定时间之后自动关闭。

（2）TIME_WAIT存在的另一个意义就是防止在立刻建立新连接后，新连接的通道会收到旧连接所重发的ACK，为了防止收到这样冗余的包，所以设计了TIME_WAIT。
