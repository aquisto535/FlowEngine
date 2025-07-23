#pragma once

#include <unordered_map>
#include <memory>
#include <atomic>
#include "Session.h"
#include "ChatRoom.h"

namespace NexusCore {
    namespace Core {

        // 싱글턴 세션 매니저
        class SessionManager {
        public:
            static SessionManager* GetInstance();

            // 세션 관리
            Session* CreateSession(SOCKET socket);
            void RemoveSession(uint64_t session_id);
            Session* FindSession(uint64_t session_id);
            Session* FindSessionByUserId(const std::string& user_id);

            // 통계 정보
            size_t GetSessionCount() const;
            std::vector<std::string> GetConnectedUserIds() const;

            // 전체 브로드캐스트
            void BroadcastToAll(const char* data, size_t size);
            void BroadcastToLoggedInUsers(const char* data, size_t size);

        private:
            SessionManager();
            ~SessionManager();

            mutable SRWLOCK sessions_lock_;
            std::unordered_map<uint64_t, std::unique_ptr<Session>> sessions_;
            std::unordered_map<std::string, Session*> user_session_map_; // user_id -> Session 빠른 검색용

            std::atomic<uint64_t> next_session_id_{ 1 };

            static SessionManager* instance_;
            static std::once_flag init_flag_;
        };

        // 싱글턴 룸 매니저
        class RoomManager {
        public:
            static RoomManager* GetInstance();

            // 방 관리
            ChatRoom* CreateRoom(const std::string& title, const std::string& password = "");
            void RemoveRoom(uint32_t room_id);
            ChatRoom* FindRoom(uint32_t room_id);

            // 방 목록
            std::vector<uint32_t> GetRoomIds() const;
            std::vector<ChatRoom*> GetAllRooms() const;

            // 통계
            size_t GetRoomCount() const;
            size_t GetTotalActiveUsers() const;

        private:
            RoomManager();
            ~RoomManager();

            mutable SRWLOCK rooms_lock_;
            std::unordered_map<uint32_t, std::unique_ptr<ChatRoom>> rooms_;

            std::atomic<uint32_t> next_room_id_{ 1 };

            static RoomManager* instance_;
            static std::once_flag init_flag_;
        };

        // 파일 전송 매니저
        class FileTransferManager {
        public:
            static FileTransferManager* GetInstance();

            struct FileTransferInfo {
                uint64_t upload_id;
                std::string file_name;
                uint64_t file_size;
                std::string file_hash;
                std::string sender_id;
                std::string receiver_id;
                size_t received_chunks;
                size_t total_chunks;
                std::string temp_file_path;
                bool is_complete;
            };

            // 파일 전송 관리
            uint64_t StartFileUpload(const std::string& file_name, uint64_t file_size,
                const std::string& file_hash, const std::string& sender_id,
                const std::string& receiver_id = "");
            bool ProcessFileChunk(uint64_t upload_id, uint32_t chunk_index,
                const char* chunk_data, size_t chunk_size);
            FileTransferInfo* GetTransferInfo(uint64_t upload_id);
            void CompleteTransfer(uint64_t upload_id);
            void CancelTransfer(uint64_t upload_id);

            // 통계 및 정리
            size_t GetActiveTransferCount() const;
            void CleanupExpiredTransfers();

        private:
            FileTransferManager();
            ~FileTransferManager();

            mutable SRWLOCK transfers_lock_;
            std::unordered_map<uint64_t, std::unique_ptr<FileTransferInfo>> active_transfers_;

            std::atomic<uint64_t> next_upload_id_{ 1 };

            static FileTransferManager* instance_;
            static std::once_flag init_flag_;
        };

    } // namespace Core
} // namespace NexusCore
