#include "MPRpcApplication.h"
#include "MPRpcChannel.h"
#include "user.pb.h"
#include <iostream>

int main(int argc, char **argv) {
	// 整个程序启动以后，想使用mprpc来获取rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
	MPRpcApplication::Init(argc, argv);

	// 演示调用远程发布的rpc方法Login
	User::UserServiceRpc_Stub stub(new MPRpcChannel);

	// rpc方法的请求参数
	User::LoginRequest request;
	request.set_name("Tim");
	request.set_pwd("123");

  //rpc方法的响应
	User::LoginResponse response;
  //发起rpc方法的调用，同步的rpc调用过程，MprpcChannel::callmethod
	stub.login(nullptr, &request, &response, nullptr);

	// 一次rpc调用完成，读取调用的结果
	if (0 == response.result().errcode()) {
		std::cout << "rpc login response:" << response.success() << std::endl;
	} else {
		std::cout << "rpc login response error:" << response.result().errmesg()
				  << std::endl;
	}

	return 0;
}