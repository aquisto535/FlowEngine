#pragma once

#include <string>
#include <map>
#include <mutex>

namespace NexusCore {
    namespace Common {

        class Config {
        public:
            static Config* GetInstance();

            // 설정 파일 로드
            bool LoadFromFile(const std::string& config_file_path);
            bool SaveToFile(const std::string& config_file_path);

            // 설정 값 접근
            std::string GetString(const std::string& key, const std::string& default_value = "");
            int GetInt(const std::string& key, int default_value = 0);
            bool GetBool(const std::string& key, bool default_value = false);
            double GetDouble(const std::string& key, double default_value = 0.0);

            // 설정 값 설정
            void SetString(const std::string& key, const std::string& value);
            void SetInt(const std::string& key, int value);
            void SetBool(const std::string& key, bool value);
            void SetDouble(const std::string& key, double value);

            // 기본 설정 초기화
            void InitializeDefaults();

        private:
            Config();
            ~Config();

            std::mutex config_mutex_;
            std::map<std::string, std::string> config_data_;

            static Config* instance_;
            static std::once_flag init_flag_;
        };

    } // namespace Common
} // namespace NexusCore
