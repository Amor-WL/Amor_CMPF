#include "config/cmpf_config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace cmpf {

// 获取配置对象
CmpfConfig& GetConfig() {
    static CmpfConfig* config = new CmpfConfig();
    return *config;
}

CmpfConfig::CmpfConfig() {
}

CmpfConfig::~CmpfConfig() {
}

bool CmpfConfig::init(const std::string& config_path) {
    return parse_config(config_path);
}

int CmpfConfig::get_int(const std::string& key, int default_value) const {
    auto it = config_.find(key);
    if (it != config_.end()) {
        // 避免使用异常，使用更安全的转换方式
        char* end_ptr = nullptr;
        long value = std::strtol(it->second.c_str(), &end_ptr, 10);
        if (*end_ptr == '\0') {
            return static_cast<int>(value);
        }
        return default_value;
    }
    return default_value;
}

std::string CmpfConfig::get_string(const std::string& key, const std::string& default_value) const {
    auto it = config_.find(key);
    if (it != config_.end()) {
        return it->second;
    }
    return default_value;
}

bool CmpfConfig::get_bool(const std::string& key, bool default_value) const {
    auto it = config_.find(key);
    if (it != config_.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "true" || value == "1" || value == "yes") {
            return true;
        } else if (value == "false" || value == "0" || value == "no") {
            return false;
        }
    }
    return default_value;
}

bool CmpfConfig::parse_config(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    int line_num = 0;
    
    while (std::getline(file, line)) {
        line_num++;
        
        // 去除首尾空格
        size_t first_non_space = line.find_first_not_of(" \t\n\r");
        if (first_non_space == std::string::npos) continue;
        line = line.substr(first_non_space);
        if (line.empty()) continue;
        
        // 跳过注释行
        if (line[0] == '#') continue;
        
        // 寻找等号
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        
        // 分割key和value
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        
        // 去除key的首尾空格
        first_non_space = key.find_first_not_of(" \t\n\r");
        if (first_non_space == std::string::npos) continue;
        key = key.substr(first_non_space);
        
        size_t last_non_space = key.find_last_not_of(" \t\n\r");
        if (last_non_space == std::string::npos) continue;
        key = key.substr(0, last_non_space + 1);
        
        // 去除value的首尾空格
        first_non_space = value.find_first_not_of(" \t\n\r");
        if (first_non_space == std::string::npos) {
            config_[key] = "";
            continue;
        }
        value = value.substr(first_non_space);
        
        last_non_space = value.find_last_not_of(" \t\n\r");
        if (last_non_space != std::string::npos) {
            value = value.substr(0, last_non_space + 1);
        }
        
        // 存储配置
        config_[key] = value;
    }
    
    file.close();
    return true;
}

} // namespace cmpf
