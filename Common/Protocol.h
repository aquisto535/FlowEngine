#pragma once

#include <cstdint>
#include <string>

namespace NexusCore {
    namespace Protocol {

        // 패킷 헤더 구조체 (8바이트)
#pragma pack(push, 1)
        struct PacketHeader {
            uint16_t packet_id;      // 패킷 ID (2바이트)
            uint16_t payload_length; // 페이로드 길이 (2바이트)
            uint32_t crc32;          // 체크섬 (4바이트)

            PacketHeader() : packet_id(0), payload_length(0), crc32(0) {}
            PacketHeader(uint16_t id, uint16_t len, uint32_t checksum = 0)
                : packet_id(id), payload_length(len), crc32(checksum) {
            }
        };
#pragma pack(pop)

        // 패킷 ID 상수 정의
        namespace PacketID {
            // 인증 및 기본 (1000~)
            constexpr uint16_t LOGIN_REQ = 1001;
            constexpr uint16_t LOGIN_RES = 1002;
            constexpr uint16_t LOGOUT_REQ = 1003;
            constexpr uint16_t LOGOUT_RES = 1004;
            constexpr uint16_t HEARTBEAT_REQ = 1005;
            constexpr uint16_t HEARTBEAT_RES = 1006;

            // 채팅 (2000~)
            constexpr uint16_t ENTER_ROOM_REQ = 2001;
            constexpr uint16_t ENTER_ROOM_RES = 2002;
            constexpr uint16_t NEW_USER_IN_ROOM_NTF = 2003;
            constexpr uint16_t LEAVE_ROOM_REQ = 2004;
            constexpr uint16_t USER_LEFT_ROOM_NTF = 2005;
            constexpr uint16_t ROOM_CHAT_REQ = 2006;
            constexpr uint16_t ROOM_CHAT_NTF = 2007;

            // 파일 전송 (3000~)
            constexpr uint16_t FILE_UPLOAD_REQ = 3001;
            constexpr uint16_t FILE_UPLOAD_RES = 3002;
            constexpr uint16_t FILE_CHUNK_SEND = 3003;
            constexpr uint16_t FILE_UPLOAD_COMPLETE_NTF = 3004;

            // 관리자 (9000~)
            constexpr uint16_t ADMIN_USER_LIST_REQ = 9001;
            constexpr uint16_t ADMIN_USER_LIST_RES = 9002;
            constexpr uint16_t KICK_USER_REQ = 9003;
            constexpr uint16_t KICK_USER_RES = 9004;
        }

        // 에러 코드
        namespace ErrorCode {
            constexpr int32_t SUCCESS = 0;
            constexpr int32_t INVALID_USER_ID = 1001;
            constexpr int32_t INVALID_PASSWORD = 1002;
            constexpr int32_t USER_ALREADY_LOGGED_IN = 1003;
            constexpr int32_t ROOM_NOT_FOUND = 2001;
            constexpr int32_t ROOM_FULL = 2002;
            constexpr int32_t INVALID_ROOM_PASSWORD = 2003;
            constexpr int32_t FILE_TOO_LARGE = 3001;
            constexpr int32_t INVALID_FILE_FORMAT = 3002;
            constexpr int32_t INSUFFICIENT_PERMISSION = 9001;
        }

        // 서버 설정 상수
        namespace Config {
            constexpr uint16_t SERVER_PORT = 9000;
            constexpr uint16_t ADMIN_PORT = 9001;
            constexpr size_t MAX_PACKET_SIZE = 65536; // 64KB
            constexpr size_t RECV_BUFFER_SIZE = 4096;
            constexpr size_t SEND_BUFFER_SIZE = 4096;
            constexpr int32_t MAX_CLIENTS = 1000;
            constexpr int32_t MAX_ROOMS = 100;
            constexpr uint64_t MAX_FILE_SIZE = 100 * 1024 * 1024; // 100MB
        }

    } // namespace Protocol
} // namespace NexusCore
