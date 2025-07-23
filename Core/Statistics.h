#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <string>
#include <mutex>

namespace NexusCore {
    namespace Core {

        class Statistics {
        public:
            static Statistics* GetInstance();

            // 카운터 관련
            void IncrementCounter(const std::string& name, uint64_t value = 1);
            void DecrementCounter(const std::string& name, uint64_t value = 1);
            void SetCounter(const std::string& name, uint64_t value);
            uint64_t GetCounter(const std::string& name) const;

            // 게이지 관련 (현재 상태값)
            void SetGauge(const std::string& name, double value);
            double GetGauge(const std::string& name) const;

            // 히스토그램 관련 (응답시간, 크기 등의 분포)
            void RecordValue(const std::string& name, double value);
            double GetAverage(const std::string& name) const;
            double GetMin(const std::string& name) const;
            double GetMax(const std::string& name) const;

            // 통계 리포트
            std::string GenerateReport() const;
            void ResetAllStats();

            // 서버 시작 시간
            void MarkServerStart();
            std::chrono::system_clock::time_point GetServerStartTime() const;

        private:
            Statistics();
            ~Statistics();

            struct HistogramData {
                double sum = 0.0;
                double min_value = std::numeric_limits<double>::max();
                double max_value = std::numeric_limits<double>::lowest();
                uint64_t count = 0;
            };

            mutable std::mutex stats_mutex_;
            std::map<std::string, std::atomic<uint64_t>> counters_;
            std::map<std::string, std::atomic<double>> gauges_;
            std::map<std::string, HistogramData> histograms_;

            std::chrono::system_clock::time_point server_start_time_;

            static Statistics* instance_;
            static std::once_flag init_flag_;
        };

        // 편의용 매크로
#define STATS_INCREMENT(name) NexusCore::Core::Statistics::GetInstance()->IncrementCounter(name)
#define STATS_DECREMENT(name) NexusCore::Core::Statistics::GetInstance()->DecrementCounter(name)
#define STATS_SET_GAUGE(name, value) NexusCore::Core::Statistics::GetInstance()->SetGauge(name, value)
#define STATS_RECORD(name, value) NexusCore::Core::Statistics::GetInstance()->RecordValue(name, value)

    } // namespace Core
} // namespace NexusCore

