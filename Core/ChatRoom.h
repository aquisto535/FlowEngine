#pragma once

#include <unordered_set>
#include <string>
#include <memory>
#include "../Common/Protocol.h"

namespace NexusCore {
    namespace Core {

        class Session; // 전방 선언

        class ChatRoom {
        public:
            ChatRoom(uint32_t room_id, const std::string& title, size_t max_participants = 50);
            ~ChatRoom();

            // 방 입장/퇴장
            bool Enter(Session* session, const std::string& password = "");
            void Leave(Session* session);

            // 메시지 전송
            void BroadcastMessage(const char* data, size_t size, Session* exclude_session = nullptr);
            void SendToUser(const char* data, size_t size, const std::string& target_user_id);

            // 방 정보
            uint32_t GetRoomId() const { return room_id_; }
            const std::string& GetTitle() const { return title_; }
            size_t GetParticipantCount() const;
            bool IsFull() const;
            bool IsEmpty() const;

            // 참가자 관리
            std::vector<std::string> GetParticipantIds() const;
            bool IsUserInRoom(const std::string& user_id) const;

            // 방 설정
            void SetPassword(const std::string& password);
            void SetMaxParticipants(size_t max_count);

        private:
            uint32_t room_id_;
            std::string title_;
            std::string password_;
            size_t max_participants_;

            mutable SRWLOCK participants_lock_;
            std::unordered_set<Session*> participants_;

            // 통계 정보
            std::atomic<uint64_t> total_messages_sent_{ 0 };
            std::atomic<uint64_t> total_users_entered_{ 0 };
        };

    } // namespace Core
} // namespace NexusCore
