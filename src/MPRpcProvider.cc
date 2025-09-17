#include "MPRpcProvider.h"
#include <cstdint>
#include <string>
#include "MPRpcApplication.h"

/*
service_name=>对service进行描述
            1.service* 记录服务对象
            2.method_name 记录服务方法对象
*/
//这里是框架提供给外部使用的，可以发布rpc方法的函数接口
//此处应该使用Service类，而不是指定某个方法
void MPRpcProvider::NotifyService(google::protobuf::Service *service)
{
    
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void MPRpcProvider::Run()
{
    std::string ip= MPRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(MPRpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);

    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop,address,"RpcProvider");
    //绑定链接回调和消息读写回调方法,很好的分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&MPRpcProvider::OnConnection,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&MPRpcProvider::OnMessge,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

    //设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout<<"RpcProvider start service at ip:"<<ip<<" port:"<<port<<std::endl;
    //启动网络服务
    server.start();
    m_eventLoop.loop();//启动epollwait
}

//新的socket连接回调
void MPRpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        //和rpcclient的链接断开了
        conn->shutdown();
    }
}

/*
在框架内部需要提前协商好通信使用的protobuf数据类型：比如发送过来的数据类型为：service_name,method_name,args
需要定义proto的message类型，进行数据头的序列化和反序列化，为防止TCP的粘包，需要对各个参数进行参数的长度明确

定义header_size（4字节） + header_str + args_str

已建立连接的用户的读写事件回调，网络上如果有一个远程的rpc服务请求，则onmessge方法就会响应
*/
void MPRpcProvider::OnMessge(const muduo::net::TcpConnectionPtr &conn,
                                    muduo::net::Buffer *buffer,
                                    muduo::Timestamp)
{
    
}
