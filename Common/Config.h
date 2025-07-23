#pragma once

#include <string>
#include <map>
#include <mutex>

namespace NexusCore {
    namespace Common {

        class Config {
        public:
            static Config* GetInstance();

            // ���� ���� �ε�
            bool LoadFromFile(const std::string& config_file_path);
            bool SaveToFile(const std::string& config_file_path);

            // ���� �� ����
            std::string GetString(const std::string& key, const std::string& default_value = "");
            int GetInt(const std::string& key, int default_value = 0);
            bool GetBool(const std::string& key, bool default_value = false);
            double GetDouble(const std::string& key, double default_value = 0.0);

            // ���� �� ����
            void SetString(const std::string& key, const std::string& value);
            void SetInt(const std::string& key, int value);
            void SetBool(const std::string& key, bool value);
            void SetDouble(const std::string& key, double value);

            // �⺻ ���� �ʱ�ȭ
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
