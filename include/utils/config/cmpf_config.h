#ifndef CMPF_UTILS_CONFIG_H_
#define CMPF_UTILS_CONFIG_H_

#include <string>
#include <unordered_map>

namespace cmpf {

class CmpfConfig {
public:
    CmpfConfig();
    ~CmpfConfig();
    
    // 初始化配置
    bool init(const std::string& config_path);
    
    // 获取整数配置项，默认值为0
    int get_int(const std::string& key, int default_value = 0) const;
    
    // 获取字符串配置项，默认值为空字符串
    std::string get_string(const std::string& key, const std::string& default_value = "") const;
    
    // 获取布尔配置项，默认值为false
    bool get_bool(const std::string& key, bool default_value = false) const;
    
private:
    std::unordered_map<std::string, std::string> config_;
    
    // 解析配置文件
    bool parse_config(const std::string& config_path);
};

// 获取配置对象
CmpfConfig& GetConfig();

} // namespace cmpf

#endif // CMPF_UTILS_CONFIG_H_