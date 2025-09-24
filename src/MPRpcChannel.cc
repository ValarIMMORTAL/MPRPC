#include "MPRpcChannel.h"
#include"rpcheader.pb.h"
#include"MPRpcApplication.h"

#include<string>
#include<errno.h>
#include<unistd.h>

//网络编程部分
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

void MPRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
  const google::protobuf::ServiceDescriptor* sd=method->service();
    std::string service_name=sd->name();//service的name
    std::string method_name=method->name();//method的name

    //获取参数的序列化字符串长度 args_size
    uint32_t args_size=0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        //序列化成功
        args_size=args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");//保存错误信息
        return;
    }
    
    //定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size=0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size=rpc_header_str.size();
    }
    else
    {
        std::cout<<"serialize rpc header error!"<<std::endl;
        return;
    }

    //组织待发送的rpc请求的字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char *)&header_size,4));//header_size
    send_rpc_str+=rpc_header_str;//rpcheader
    send_rpc_str+=args_str;//args

    std::cout<<"======================================"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str"<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"======================================"<<std::endl;
    
    //使用TCP编程，完成rpc方法的远程调用
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==clientfd)
    {
        std::cout<<"create socket error! errno: "<<errno<<std::endl;//改用controller记录错误信息
        exit(EXIT_FAILURE);
    }

    //读取配置文件rpcserver的信息
    std::string ip=MPRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(MPRpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str());

    //链接rpc服务节点
    if(-1==connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {

        std::cout<<"connect error!errno: "<<errno<<std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    //发送rpc请求
    if(-1==send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0))
    {
        std::cout<<"send error!errno: "<<errno<<std::endl;
        close(clientfd);
        return;//结束本次rpc调用
    }

    //接受rpc请求的响应值
    char recv_buf[1024]={0};
    int recv_size=0;
    if(-1==(recv_size=recv(clientfd,recv_buf,1024,0)))
    {
        std::cout<<"recv error!errno: "<<errno<<std::endl;
        close(clientfd);
        return;
    }
    // std::cout<<recv_buf<<std::endl;

    //反序列化rpc调用的响应数据
    //bug点：recv_buf遇到\0后的数据不再读取，导致反序列化失败
    //解决方案：使用string转换时会遇到\0，由于字符串特性导致不再读取，因为protobuf支持从数组转换，所以换方法直接从Array反序列化
    std::string response_str(recv_buf,0,recv_size);
    //if(!response->ParseFromString(response_str))
    if(!response->ParsePartialFromArray(recv_buf,recv_size))
    {
        std::cout<<"parse error! response_str:"<<response_str<<std::endl;
        close(clientfd);
        return;
    }
    close(clientfd);
}