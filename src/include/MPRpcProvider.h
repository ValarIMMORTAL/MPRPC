#pragma once
#include "google/protobuf/service.h"

// 发布RPC服务的网络对象类
class MPRpcProvider {
  public:
	// 发布RPC方法的接口
	void NotifyCation(google::protobuf::Service *service);

	// 启动RPC服务节点，开始提供远程RPC服务调用
	void Run();
};