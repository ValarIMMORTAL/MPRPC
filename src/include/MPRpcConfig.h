#pragma once

#include <string>
#include <unordered_map>

// 框架读取配置类
class MPRpcConfig {
public:
  // 负责解析加载配置文件
  void LoadConfigFile(const char *config_file_name);
  // 查询配置项信息
  std::string Load(const std::string &key);

private:
  std::unordered_map<std::string, std::string> m_configMap;
};