#include "MPRpcApplication.h"
#include "MPRpcChannel.h"
#include "MPRpcController.h"
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

	// rpc方法的响应
	User::LoginResponse response;
	MprpcController controller;
	// 发起rpc方法的调用，同步的rpc调用过程，MprpcChannel::callmethod
	stub.login(&controller, &request, &response, nullptr);

	if (controller.Failed()) {
		std::cout << controller.ErrorText() << std::endl;
	} else {
		// 一次rpc调用完成，读取调用的结果
		if (0 == response.result().errcode()) {
			std::cout << "rpc login response:" << response.success()
					  << std::endl;
		} else {
			std::cout << "rpc login response error:"
					  << response.result().errmesg() << std::endl;
		}
	}

	User::RegisterRequest req;
	req.set_id(1);
	req.set_name("Tom");
	req.set_pwd("123");
	User::RegisterResponse rsp;

	// 以同步的方式发起RPC请求，等待返回结果
	stub.Register(&controller, &req, &rsp, nullptr);
	if (controller.Failed()) {
		std::cout << controller.ErrorText() << std::endl;
	} else {
		// 一次rpc调用完成，读取调用的结果
		if (0 == rsp.result().errcode()) {
			std::cout << "rpc register response:" << rsp.success() << std::endl;
		} else {
			std::cout << "rpc register response error:"
					  << rsp.result().errmesg() << std::endl;
		}
	}
	
	return 0;
}