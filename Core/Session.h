#pragma once

#include <winsock2.h>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "../Common/Protocol.h"

namespace NexusCore {
    namespace Core {

        class ChatRoom; // 전방 선언

        // I/O 작업 타입
        enum class IoOperationType {
            RECV,
            SEND,
            ACCEPT
        };

        // I/O 작업별 컨텍스트
        struct PerIoContext {
            OVERLAPPED overlapped;
            WSABUF wsa_buffer;
            char buffer[Protocol::Config::RECV_BUFFER_SIZE];
            IoOperationType operation_type;

            PerIoContext(IoOperationType type) : operation_type(type) {
                ZeroMemory(&overlapped, sizeof(overlapped));
                ZeroMemory(buffer, sizeof(buffer));
                wsa_buffer.len = sizeof(buffer);
                wsa_buffer.buf = buffer;
            }
        };

        // 송신 데이터 구조체
        struct SendData {
            char* data;
            size_t size;

            SendData(const char* src, size_t len) : size(len) {
                data = new char[len];
                memcpy(data, src, len);
            }

            ~SendData() {
                delete[] data;
            }
        };

        // 클라이언트 세션 클래스
        class Session {
        public:
            Session(SOCKET socket, uint64_t session_id);
            ~Session();

            // 네트워크 I/O 관련
            bool PostRecv();
            bool PostSend(const char* data, size_t size);
            void ProcessPacket(Protocol::PacketHeader* header, char* payload);
            void Disconnect();

            // 상태 관리
            bool IsLoggedIn() const;
            void SetLoggedIn(const std::string& user_id);
            void SetLoggedOut();

            // 채팅방 관리
            void EnterRoom(ChatRoom* room);
            void LeaveRoom();
            ChatRoom* GetCurrentRoom() const;

            // Getter/Setter
            SOCKET GetSocket() const { return socket_; }
            uint64_t GetSessionId() const { return session_id_; }
            const std::string& GetUserId() const { return user_id_; }

            // 내부 데이터 보호를 위한 락
            mutable SRWLOCK data_lock_;

        private:
            // 네트워크 관련
            SOCKET socket_;
            uint64_t session_id_;
            PerIoContext recv_context_;

            // 송신 큐 관리
            std::queue<std::unique_ptr<SendData>> send_queue_;
            std::mutex send_queue_mutex_;
            bool is_sending_;

            // 세션 상태
            std::string user_id_;
            bool is_logged_in_;
            ChatRoom* current_room_;

            // 수신 버퍼 관리
            char packet_buffer_[Protocol::Config::MAX_PACKET_SIZE];
            size_t packet_buffer_pos_;

            // 내부 헬퍼 함수들
            void ProcessSendQueue();
            bool ParsePackets(char* data, size_t size);
            uint32_t CalculateCRC32(const char* data, size_t size);
        };

    } // namespace Core
} // namespace NexusCore
