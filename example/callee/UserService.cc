#include "../user.pb.h"
#include "MPRpcApplication.h"
#include "MPRpcProvider.h"
#include <iostream>
#include <string>

class UserService : public User::UserServiceRpc {
public:
  bool login(std::string name, std::string pwd) {
    std::cout << "Local Service: login" << std::endl;
    std::cout << "name: " << name << ", pwd: " << pwd << std::endl;
    return false;
  }
  bool Register(uint32_t id, std::string name, std::string pwd) {
    std::cout << "doing Register service:Login" << std::endl;
    std::cout << "id:" << id << " name:" << name << " pwd:" << pwd << std::endl;
    return true;
  }
  // 重写基类UserServiceRpc的虚函数
  void login(::google::protobuf::RpcController *controller,
             const ::User::LoginRequest *request,
             ::User::LoginResponse *response,
             ::google::protobuf::Closure *done) {
    // 从请求得到参数
    std::string name = request->name();
    std::string pwd = request->pwd();

    // 本地实现业务
    bool res = login(name, pwd);

    // 响应消息
    User::ResultCode *resCode = response->mutable_result();
    resCode->set_errcode(0);
    resCode->set_errmesg("");
    response->set_success(res);

    // 执行回调，响应数据对象的序列化和网络发送
    done->Run();
  }
  void Register(::google::protobuf::RpcController *controller,
                const ::User::RegisterRequest *request,
                ::User::RegisterResponse *response,
                ::google::protobuf::Closure *done) {
    uint32_t id = request->id();
    std::string name = request->name();
    std::string pwd = request->pwd();

    // 开始做本地业务
    bool ret = Register(id, name, pwd);

    // 填充回调结果
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmesg("");
    response->set_success(ret);

    done->Run();
  }
};

int main(int argc, char **argv) {
  // 框架初始化
  MPRpcApplication::Init(argc, argv);

  // 创建网络服务对象，发布到RPC节点
  MPRpcProvider provider;
  provider.NotifyService(new UserService());

  provider.Run();
}