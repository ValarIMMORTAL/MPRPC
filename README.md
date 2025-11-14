# MPRPC - 基于 Protobuf + ZooKeeper 的 C++ 高性能 RPC 框架

## 项目简介

MPRPC 是一个轻量、高性能的跨进程通信（RPC）框架，基于 C++11 开发，采用 Protobuf 进行数据序列化 / 反序列化，结合 ZooKeeper 实现服务注册与发现，依托 muduo 网络库提供高并发网络通信能力。适用于微服务架构中的跨进程调用、分布式系统协作等场景，支持异步日志、服务动态上下线、多线程并发处理等核心特性。

### 核心特性

- 🚀 **高性能**：基于 muduo 多线程 Reactor 模型，I/O 与业务逻辑分离，支持万级并发连接

- 📦 **协议标准化**：采用 Protobuf 定义服务接口，自动生成序列化代码，兼容多语言通信

- 🔍 **服务发现**：集成 ZooKeeper 实现服务注册、负载均衡与动态发现，支持服务下线自动剔除

- 📝 **异步日志**：基于生产者 - 消费者模型的异步日志系统，避免业务线程阻塞

- 🎯 **接口易用**：简洁的服务注册与调用 API，降低 RPC 开发门槛

- 🔧 **灵活配置**：JSON 格式配置文件，支持服务端口、ZooKeeper 地址自定义

- 🧪 **完善示例**：提供服务端 / 客户端完整示例，快速上手二次开发

## 快速开始

### 前置依赖

确保系统已安装以下依赖（推荐版本）：

- 编译器：VSCode

- 构建工具：CMake 3.10+、Git

- 第三方库：

- - Protobuf 3.21+（数据序列化）

- - ZooKeeper 3.8+（服务注册发现）

- - muduo 2.0+（网络通信）

- - json.hpp（配置解析，已内置在 thirdparty/）

### 依赖安装（Ubuntu 示例）

```
# 安装基础依赖
sudo apt-get update && sudo apt-get install -y build-essential cmake git

# 安装 Protobuf
sudo apt-get install -y libprotobuf-dev protobuf-compiler

# 安装 ZooKeeper
sudo apt-get install -y zookeeper zookeeper-dev

# 安装 muduo 库（手动编译）
git clone https://github.com/chenshuo/muduo.git
cd muduo
./build.sh
sudo cp -r build/release/include/* /usr/local/include/
sudo cp -r build/release/lib/* /usr/local/lib/
cd .. && rm -rf muduo
```

### 编译项目

```
# 克隆仓库
git clone https://github.com/ValarIMMORTAL/MPRPC.git
cd MPRPC

# 自动化编译（生成 build 目录）
chmod +x autobuild.sh
./autobuild.sh
```

### 运行示例

以 friend_service 示例为例，演示服务端启动、客户端调用流程：

#### 1. 启动 ZooKeeper 服务

```
# 本地测试启动 ZooKeeper
sudo service zookeeper start
# 验证 ZooKeeper 状态（默认端口 2181）
zkServer.sh status
```

#### 2. 启动 RPC 服务端（服务提供者）

```
cd MPRPC/bin
./provider -i conf.json # 启动好友服务，默认监听 127.0.0.1:8000
```

#### 3. 启动 RPC 客户端（服务调用者）

```
# 新建终端，进入客户端目录
cd MPRPC/bin
./consumer -i conf.json  # 调用好友服务的 GetFriendList 方法
```

#### 预期输出

客户端终端会打印：

```
pc GetFriendList response success!
index: 1 name:Jerry
index: 2 name:Zero
index: 3 name:Starve
```

## 核心功能详解

### 1. 服务注册与发现

- 服务端启动时，自动将服务信息（IP、端口、服务名）注册到 ZooKeeper 的 /mprpc/services/[服务名] 节点（临时节点）

- 客户端通过 ZooKeeper 查询目标服务的可用节点，实现负载均衡（默认轮询策略）

- 服务下线时，ZooKeeper 临时节点自动删除，客户端感知后不再调用该节点

### 2. 异步日志系统

- 采用生产者 - 消费者模型，业务线程通过 Logger::Log() 异步写入日志，不阻塞业务逻辑

- 支持日志分级（DEBUG/INFO/WARN/ERROR），可通过配置文件调整输出级别

- 日志缓冲区基于 LockQueue 实现，通过互斥锁 + 条件变量保证线程安全

### 3. 协议通信流程

1. 客户端通过 Protobuf 定义的 Stub 类封装请求参数，序列化后发送给服务端

1. 服务端接收数据后，通过 Protobuf 反序列化得到请求对象

1. 服务端调用对应的业务实现（重写的虚函数），处理完成后构造响应对象

1. 响应对象序列化后返回给客户端，客户端反序列化得到结果

## 二次开发指南

基于 MPRPC 开发自己的 RPC 服务，只需 5 步：定义协议 → 生成代码 → 实现服务 → 注册服务 → 客户端调用。

### 步骤 1：定义 Protobuf 协议

在 proto/ 目录下创建自定义协议文件（如 user.proto），定义请求 / 响应消息和服务接口：

```
syntax = "proto3";
package User;  // 命名空间，避免冲突

// 登录请求消息
message LoginRequest {
  string name = 1;  // 用户名
  string pwd = 2;   // 密码
}

// 登录响应消息
message LoginResponse {
  bool success = 1; // 登录结果（true/false）
  string msg = 2;   // 响应消息（如登录成功/失败原因）
  uint32 user_id = 3; // 用户 ID（登录成功返回）
}

// 服务接口定义
service UserServiceRpc {
  rpc login(LoginRequest) returns (LoginResponse); // 登录方法
}
```

### 步骤 2：生成 Protobuf 代码

执行以下命令，生成 C++ 版本的协议代码（.[pb.cc](http://pb.cc) 和 .pb.h）：

```
# 生成代码到 src/ 目录（与框架核心代码同路径）
protoc --cpp_out=./src ./proto/user.proto
```

### 步骤 3：实现服务端业务逻辑

创建服务实现类，继承 Protobuf 生成的服务接口类，重写虚函数（核心业务逻辑）：

```
// src/user_service.cc
#include "user.pb.h"
#include "MPRpcProvider.h"
#include <string>

// 继承 Protobuf 生成的服务接口类
class UserService : public User::UserServiceRpc {
public:
  // 重写登录方法的虚函数
  void login(google::protobuf::RpcController* controller,
             const User::LoginRequest* request,
             User::LoginResponse* response,
             google::protobuf::Closure* done) override {
    // 1. 解析请求参数
    std::string name = request->name();
    std::string pwd = request->pwd();

    // 2. 执行本地业务逻辑（示例：模拟用户名密码验证）
    bool login_success = false;
    uint32_t user_id = 0;
    if (name == "admin" && pwd == "123456") {
      login_success = true;
      user_id = 10001; // 模拟用户 ID
    }

    // 3. 构造响应结果
    response->set_success(login_success);
    response->set_msg(login_success ? "登录成功" : "账号密码错误");
    response->set_user_id(user_id);

    // 4. 触发响应发送（序列化+网络传输）
    done->Run();
  }
};

// 服务端主函数
int main() {
  // 1. 初始化 RPC 框架（读取配置文件）
  MPRPCApplication::Init();

  // 2. 创建服务提供者对象
  MPRpcProvider provider;

  // 3. 注册服务（将 UserService 实例注册到框架）
  provider.NotifyService(new UserService());

  // 4. 启动 RPC 服务（阻塞监听客户端连接）
  provider.Run();

  return 0;
}
```

### 步骤 4：配置服务端

在项目根目录创建 rpc.conf 配置文件（JSON 格式）：

```
{
  "rpcserverip": "0.0.0.0",    // 服务端监听 IP（0.0.0.0 表示所有网卡）
  "rpcserverport": 8000,       // 服务端监听端口
  "zookeeperip": "127.0.0.1",  // ZooKeeper 服务器 IP
  "zookeeperport": 2181,       // ZooKeeper 端口
  "loglevel": "INFO"           // 日志级别（DEBUG/INFO/WARN/ERROR）
}
```

### 步骤 5：客户端调用服务

创建客户端代码，通过 Protobuf 生成的 Stub 类调用远程服务：

```
// src/user_client.cc
#include "user.pb.h"
#include "MPRpcChannel.h"
#include <iostream>
#include <string>

int main() {
  // 1. 初始化 RPC 框架
  MPRPCApplication::Init();

  // 2. 创建 Stub 对象（绑定通信通道）
  User::UserServiceRpc_Stub stub(new MprpcChannel());

  // 3. 构造请求对象
  User::LoginRequest req;
  req.set_name("admin");
  req.set_pwd("123456");

  // 4. 构造响应对象（用于接收结果）
  User::LoginResponse resp;

  // 5. 同步调用远程服务（阻塞直到收到响应）
  stub.login(nullptr, &req, &resp, nullptr);

  // 6. 解析响应结果
  if (resp.success()) {
    std::cout << "登录成功！用户 ID：" << resp.user_id() << std::endl;
  } else {
    std::cout << "登录失败：" << resp.msg() << std::endl;
  }

  return 0;
}
```

### 步骤 6：编译运行

修改 CMakeLists.txt，添加新服务的编译规则（参考 example/ 目录下的配置），重新执行 ./[autobuild.sh](http://autobuild.sh)，然后启动服务端和客户端即可。

## 项目结构说明

```
MPRPC/
├── src/                  # 框架核心代码
│   ├── include/          # 核心头文件（MPRpcProvider.h、Logger.h 等）
│   ├── MPRpcApplication.cc  # 框架初始化（单例）
│   ├── MPRpcProvider.cc     # 服务提供者（注册服务、启动网络）
│   ├── MprpcChannel.cc      # 客户端通信通道
│   ├── logger.cc            # 日志系统（异步+线程安全）
│   ├── zookeeperutil.cc     # ZooKeeper 客户端封装
│   └── lockqueue.h          # 线程安全队列（日志缓冲）
├── example/              # 示例代码
│   ├── callee/           # 服务端示例（friend_service、user_service）
│   ├── caller/           # 客户端示例（friend_client、user_client）
│   └── proto/            # 协议文件（friend.proto、user.proto）
├── thirdparty/           # 第三方依赖（json.hpp）
├── CMakeLists.txt        # 项目构建配置
├── autobuild.sh          # 自动化编译脚本
└── README.md             # 项目文档（本文档）
```

## 配置文件说明

配置文件默认名为 rpc.conf，放置在项目根目录，支持以下参数：

| 参数名        | 说明                | 可选值                 | 默认值    |
| ------------- | ------------------- | ---------------------- | --------- |
| rpcserverip   | 服务端监听 IP       | 合法 IP 地址 / 0.0.0.0 | 127.0.0.1 |
| rpcserverport | 服务端监听端口      | 1-65535                | 8000      |
| zookeeperip   | ZooKeeper 服务器 IP | 合法 IP 地址           | 127.0.0.1 |
| zookeeperport | ZooKeeper 服务端口  | 1-65535                | 2181      |

## 常见问题（FAQ）

### Q1：编译时提示「muduo 库未找到」？

A1：确保 muduo 库已正确安装到 /usr/local/include/ 和 /usr/local/lib/，或在 CMakeLists.txt 中添加 muduo 路径：

```
include_directories(/path/to/muduo/include)
link_directories(/path/to/muduo/lib)
```

### Q2：客户端调用失败，提示「服务未找到」？

A2：

1. 检查 ZooKeeper 服务是否启动（[zkServer.sh](http://zkServer.sh) status）

1. 检查服务端是否成功注册服务（查看服务端日志）

1. 确认客户端配置的 ZooKeeper 地址与服务端一致

### Q3：日志未输出或输出异常？

A3：

1. 检查配置文件的 loglevel 是否正确（如设置为 ERROR 则不输出 INFO 日志）

1. 确认日志目录有写入权限（默认输出到控制台，可修改 [logger.cc](http://logger.cc) 改为文件输出）

### Q4：高并发场景下出现性能瓶颈？

A4：

1. 调整 muduo 线程池大小（在 MPRpcProvider::Run() 中修改 server.setThreadNum(8)）

1. 优化业务逻辑，避免在 RPC 方法中执行耗时操作

1. 增加服务节点，通过 ZooKeeper 实现负载均衡

## 贡献指南

欢迎参与 MPRPC 项目的开发与优化，贡献流程如下：

1. Fork 本仓库到自己的 GitHub 账号

1. 基于 dev 分支创建 Feature 分支（命名格式：feat/功能名称 或 fix/BUG描述）

1. 提交代码时，遵循 [Google C++ 编码规范](https://google.github.io/styleguide/cppguide.html)

1. Commit 信息格式：类型：描述（如 feat：新增配置热加载功能、fix：修复 ZooKeeper 重连bug）

1. 提交 PR 到本仓库的 dev 分支，PR 描述需说明功能或修复内容

1. 等待维护者审核，通过后合并到主分支

## 许可证

本项目基于 [MIT 许可证](LICENSE) 开源，允许个人和商业项目自由使用、修改和分发，无需支付授权费用。

## 维护者

- 作者：ValarIMMORTAL

- 反馈：通过 GitHub Issues 提交问题或建议