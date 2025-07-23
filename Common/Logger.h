#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace NexusCore {
    namespace Common {

        enum class LogLevel {
            DEBUG = 0,
            INFO = 1,
            WARNING = 2,
            ERROR = 3,
            CRITICAL = 4
        };

        class Logger {
        public:
            static Logger* GetInstance();

            // 초기화
            bool Initialize(const std::string& log_file_path, LogLevel min_level = LogLevel::INFO);
            void Shutdown();

            // 로깅 함수들
            void Log(LogLevel level, const std::string& message);
            void Debug(const std::string& message);
            void Info(const std::string& message);
            void Warning(const std::string& message);
            void Error(const std::string& message);
            void Critical(const std::string& message);

            // 포맷 로깅
            template<typename... Args>
            void LogF(LogLevel level, const std::string& format, Args... args);

            // 설정
            void SetLogLevel(LogLevel level);
            void SetConsoleOutput(bool enable);

        private:
            Logger();
            ~Logger();

            std::string GetCurrentTimeString();
            std::string LogLevelToString(LogLevel level);
            void WriteLog(LogLevel level, const std::string& message);

            std::mutex log_mutex_;
            std::ofstream log_file_;
            LogLevel min_log_level_;
            bool console_output_enabled_;
            bool is_initialized_;

            static Logger* instance_;
            static std::once_flag init_flag_;
        };

        // 편의용 매크로
#define LOG_DEBUG(msg) NexusCore::Common::Logger::GetInstance()->Debug(msg)
#define LOG_INFO(msg) NexusCore::Common::Logger::GetInstance()->Info(msg)
#define LOG_WARNING(msg) NexusCore::Common::Logger::GetInstance()->Warning(msg)
#define LOG_ERROR(msg) NexusCore::Common::Logger::GetInstance()->Error(msg)
#define LOG_CRITICAL(msg) NexusCore::Common::Logger::GetInstance()->Critical(msg)

    } // namespace Common
} // namespace NexusCore
