#pragma once

#include <winsock2.h>
#include <vector>
#include <thread>
#include <atomic>
#include "../Core/Session.h"

namespace NexusCore {
    namespace Server {

        class NexusServer {
        public:
            NexusServer();
            ~NexusServer();

            // 서버 시작/중지
            bool Initialize(uint16_t port = Protocol::Config::SERVER_PORT,
                uint16_t admin_port = Protocol::Config::ADMIN_PORT);
            bool Start();
            void Stop();

            // 상태 확인
            bool IsRunning() const { return is_running_; }
            size_t GetWorkerThreadCount() const { return worker_threads_.size(); }

        private:
            // 초기화 관련
            bool InitializeWinsock();
            bool CreateIocpHandle();
            bool CreateListenSocket(uint16_t port);
            bool CreateWorkerThreads();

            // 스레드 함수들
            static unsigned int __stdcall AcceptThreadProc(void* param);
            static unsigned int __stdcall WorkerThreadProc(void* param);
            static unsigned int __stdcall AdminThreadProc(void* param);

            // I/O 처리
            void ProcessIoCompletion(DWORD bytes_transferred, Core::Session* session,
                Core::PerIoContext* io_context);
            void ProcessRecvCompletion(Core::Session* session, DWORD bytes_transferred);
            void ProcessSendCompletion(Core::Session* session);

            // 관리자 인터페이스
            void ProcessAdminConnection(SOCKET admin_socket);

            // 서버 상태
            std::atomic<bool> is_running_{ false };
            std::atomic<bool> should_stop_{ false };

            // 네트워크 관련
            SOCKET listen_socket_;
            SOCKET admin_listen_socket_;
            HANDLE iocp_handle_;

            // 스레드 관리
            std::vector<std::thread> worker_threads_;
            std::thread accept_thread_;
            std::thread admin_thread_;

            // 설정
            uint16_t server_port_;
            uint16_t admin_port_;
            size_t worker_thread_count_;

            // 통계
            std::atomic<uint64_t> total_connections_{ 0 };
            std::atomic<uint64_t> total_packets_processed_{ 0 };
        };

    } // namespace Server
} // namespace NexusCore
