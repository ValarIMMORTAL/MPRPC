#include "MPRpcApplication.h"
#include "MPRpcProvider.h"
#include "friend.pb.h"
#include <iostream>
#include <string>
#include <vector>

class FriendService : public Friend::FriendServiceRpc {
public:
  std::vector<std::string> GetFriendList(uint32_t userid) {
    std::cout << " do GetFriendList service!" << std::endl;
    std::vector<std::string> vec;
    vec.push_back("Jerry");
    vec.push_back("Zero");
    vec.push_back("Starve");
    return vec;
  }

  // 重写基类方法
  void GetFriendList(::google::protobuf::RpcController *controller,
                     const ::Friend::GetFriendListRequest *request,
                     ::Friend::GetFriendListResponse *response,
                     ::google::protobuf::Closure *done) {
    uint32_t userid = request->userid();

    std::vector<std::string> friendList = GetFriendList(userid);
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
    for (std::string &name : friendList) {
      std::string *p = response->add_friends();
      *p = name;
    }
    done->Run();
  }
};

int main(int argc, char **argv) {
  // 先调用框架的初始化操作 provider -i
  // config.conf，从init方法读取配置服务，比如IP地址和端口号
  MPRpcApplication::Init(argc, argv);

  // 项目提供者，发布该服务
  MPRpcProvider provider;
  // 把FriendService对象发布到rpc节点上
  provider.NotifyService(new FriendService());

  // 启动一个rpc服务发布节点，run以后，进程进入阻塞状态，等待远程的rpc请求
  provider.Run();

  return 0;
}