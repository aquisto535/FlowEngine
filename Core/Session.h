#pragma once

#include <winsock2.h>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "../Common/Protocol.h"

namespace NexusCore {
    namespace Core {

        class ChatRoom; // ���� ����

        // I/O �۾� Ÿ��
        enum class IoOperationType {
            RECV,
            SEND,
            ACCEPT
        };

        // I/O �۾��� ���ؽ�Ʈ
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

        // �۽� ������ ����ü
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

        // Ŭ���̾�Ʈ ���� Ŭ����
        class Session {
        public:
            Session(SOCKET socket, uint64_t session_id);
            ~Session();

            // ��Ʈ��ũ I/O ����
            bool PostRecv();
            bool PostSend(const char* data, size_t size);
            void ProcessPacket(Protocol::PacketHeader* header, char* payload);
            void Disconnect();

            // ���� ����
            bool IsLoggedIn() const;
            void SetLoggedIn(const std::string& user_id);
            void SetLoggedOut();

            // ä�ù� ����
            void EnterRoom(ChatRoom* room);
            void LeaveRoom();
            ChatRoom* GetCurrentRoom() const;

            // Getter/Setter
            SOCKET GetSocket() const { return socket_; }
            uint64_t GetSessionId() const { return session_id_; }
            const std::string& GetUserId() const { return user_id_; }

            // ���� ������ ��ȣ�� ���� ��
            mutable SRWLOCK data_lock_;

        private:
            // ��Ʈ��ũ ����
            SOCKET socket_;
            uint64_t session_id_;
            PerIoContext recv_context_;

            // �۽� ť ����
            std::queue<std::unique_ptr<SendData>> send_queue_;
            std::mutex send_queue_mutex_;
            bool is_sending_;

            // ���� ����
            std::string user_id_;
            bool is_logged_in_;
            ChatRoom* current_room_;

            // ���� ���� ����
            char packet_buffer_[Protocol::Config::MAX_PACKET_SIZE];
            size_t packet_buffer_pos_;

            // ���� ���� �Լ���
            void ProcessSendQueue();
            bool ParsePackets(char* data, size_t size);
            uint32_t CalculateCRC32(const char* data, size_t size);
        };

    } // namespace Core
} // namespace NexusCore
