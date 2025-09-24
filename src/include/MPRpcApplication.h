#pragma once

// mprpc框架基础类
#include "MPRpcConfig.h"
class MPRpcApplication {
public:
  static void Init(int argc, char **argv);
  static MPRpcApplication &GetInstance();
  static MPRpcConfig &GetConfig();

private:
  static MPRpcConfig m_config;

  MPRpcApplication() {};
  MPRpcApplication(const MPRpcApplication &) = delete;
  MPRpcApplication(MPRpcApplication &&) = delete;
};