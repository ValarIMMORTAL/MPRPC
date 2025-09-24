#include "MPRpcApplication.h"
#include "MPRpcConfig.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

MPRpcConfig MPRpcApplication::m_config;

void ShowArgsHelp() {
  std::cout << "format : command -i <configfile>" << std::endl;
}

void MPRpcApplication::Init(int argc, char **argv) {
  if (argc < 2) {
    ShowArgsHelp();
    exit(EXIT_FAILURE);
  }
  int opt;
  const char *optstring = "i:"; // 支持 -i
  std::string config_file;

  // 循环解析命令行选项
  while ((opt = getopt(argc, argv, optstring)) != -1) {
    switch (opt) {
    case 'i':
      config_file = optarg;
      break;
    case '?':
      // 非法选项或缺少参数
      std::cerr << "Unknown option or missing argument: -" << char(optopt)
                << "\n";
      exit(EXIT_FAILURE);
    case ':':
      std::cerr << "need <configfile>\n";
      ShowArgsHelp();
      exit(EXIT_FAILURE);
    default:
      break;
    }
  }
  m_config.LoadConfigFile(config_file.c_str());

  // std::cout << "rpcserverip: " << m_config.Load("rpcserverip") <<
  // std::endl; std::cout << "rpcserverport: " <<
  // m_config.Load("rpcserverport")
  // 		  << std::endl;
  // std::cout << "zookeeperip: " << m_config.Load("zookeeperip") <<
  // std::endl; std::cout << "zookeeperport: " <<
  // m_config.Load("zookeeperport")
  // 		  << std::endl;
}

MPRpcApplication &MPRpcApplication::GetInstance() {
  static MPRpcApplication app;
  return app;
}

MPRpcConfig &MPRpcApplication::GetConfig() { return m_config; }