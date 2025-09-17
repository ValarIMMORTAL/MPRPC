#pragma once
#include"google/protobuf/service.h"
//muduo库的头文件
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<google/protobuf/descriptor.h>
#include<functional>
#include<unordered_map>
#include<string>

// 发布RPC服务的网络对象类
class MPRpcProvider {
  public:
	// 发布RPC方法的接口
	void NotifyService(google::protobuf::Service *service);

	// 启动RPC服务节点，开始提供远程RPC服务调用
	void Run();

  private:
		muduo::net::EventLoop m_eventLoop;
		//service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;//保存服务对象
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;//保存服务方法

    };

    //存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string,ServiceInfo> m_serviceMap;
    //新的socket链接回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    //已建立连接用户的读写事件回调
    void OnMessge(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
};