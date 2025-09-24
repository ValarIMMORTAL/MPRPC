#include "MPRpcConfig.h"
#include "json.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// 负责解析加载配置文件
void MPRpcConfig::LoadConfigFile(const char *config_file_name) {
  // 读取JSON文件
  std::ifstream config_file(config_file_name);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open config.json" << std::endl;
    exit(EXIT_FAILURE);
  }
  // 解析JSON
  json config;
  config_file >> config;
  config_file.close();
  // 填充m_configMap
  for (auto &[key, value] : config.items()) {
    m_configMap[key] = value.is_string() ? value.get<std::string>()
                                         : std::to_string(value.get<int>());
  }
}
// 查询配置项信息
std::string MPRpcConfig::Load(const std::string &key) {
  auto it = m_configMap.find(key);
  if (it != m_configMap.end()) {
    return it->second;
  }
  return "";
}