#pragma once

#include <memory>
#include <queue>
#include <mutex>

namespace NexusCore {
    namespace Core {

        template<typename T>
        class MemoryPool {
        public:
            explicit MemoryPool(size_t initial_size = 100, size_t max_size = 1000)
                : max_size_(max_size) {
                for (size_t i = 0; i < initial_size; ++i) {
                    pool_.push(std::make_unique<T>());
                }
            }

            ~MemoryPool() = default;

            std::unique_ptr<T> Acquire() {
                std::lock_guard<std::mutex> lock(mutex_);
                if (pool_.empty()) {
                    return std::make_unique<T>();
                }

                auto obj = std::move(pool_.front());
                pool_.pop();
                return obj;
            }

            void Release(std::unique_ptr<T> obj) {
                if (!obj) return;

                std::lock_guard<std::mutex> lock(mutex_);
                if (pool_.size() < max_size_) {
                    pool_.push(std::move(obj));
                }
                // max_size를 초과하면 객체를 폐기 (자동으로 소멸됨)
            }

            size_t GetPoolSize() const {
                std::lock_guard<std::mutex> lock(mutex_);
                return pool_.size();
            }

        private:
            mutable std::mutex mutex_;
            std::queue<std::unique_ptr<T>> pool_;
            size_t max_size_;
        };

        // 특화된 메모리 풀들
        using SessionPool = MemoryPool<class Session>;
        using PacketBufferPool = MemoryPool<std::vector<char>>;

    } // namespace Core
} // namespace NexusCore
