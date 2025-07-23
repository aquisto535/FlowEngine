syntax = "proto3";
package NexusCore.Protocol;

// ===== 인증 및 기본 프로토콜 =====
message LoginRequest {
    string user_id = 1;
    string password = 2;
    string client_version = 3;
}

message LoginResponse {
    bool success = 1;
    uint64 session_id = 2;
    string message = 3;
    int32 error_code = 4;
}

message LogoutRequest {
    uint64 session_id = 1;
}

message LogoutResponse {
    bool success = 1;
    string message = 2;
}

// ===== 채팅 프로토콜 =====
message EnterRoomRequest {
    uint32 room_id = 1;
    string room_password = 2; // 선택적
}

message EnterRoomResponse {
    bool success = 1;
    uint32 room_id = 2;
    string room_title = 3;
    repeated UserInfo users_in_room = 4;
    string message = 5;
}

message LeaveRoomRequest {
    uint32 room_id = 1;
}

message UserInfo {
    string user_id = 1;
    int64 join_time = 2;
    int32 status = 3; // 0: 일반, 1: 관리자
}

message NewUserInRoomNotify {
    UserInfo user_info = 1;
    uint32 room_id = 2;
}

message UserLeftRoomNotify {
    string user_id = 1;
    uint32 room_id = 2;
    int64 leave_time = 3;
}

message RoomChatRequest {
    string message = 1;
    int32 message_type = 2; // 0: 일반, 1: 귓속말, 2: 공지
    string target_user = 3; // 귓속말 대상 (선택적)
}

message RoomChatNotify {
    string sender_id = 1;
    string message = 2;
    int64 timestamp = 3;
    int32 message_type = 4;
}

// ===== 파일 전송 프로토콜 =====
message FileUploadRequest {
    string file_name = 1;
    uint64 file_size = 2;
    string file_hash = 3; // MD5 또는 SHA256
    string target_user = 4; // 받을 사용자 (선택적)
}

message FileUploadResponse {
    bool success = 1;
    uint64 upload_id = 2;
    uint32 chunk_size = 3;
    string message = 4;
}

message FileChunk {
    uint64 upload_id = 1;
    uint32 chunk_index = 2;
    uint32 chunk_size = 3;
    bytes chunk_data = 4;
}

message FileUploadCompleteNotify {
    uint64 upload_id = 1;
    bool success = 2;
    string file_path = 3; // 서버상의 파일 경로
    string message = 4;
}

// ===== 관리자 프로토콜 =====
message AdminUserListRequest {
    // 빈 메시지
}

message AdminUserListResponse {
    repeated UserInfo connected_users = 1;
    int32 total_count = 2;
}

message KickUserRequest {
    string target_user_id = 1;
    string reason = 2;
}

message KickUserResponse {
    bool success = 1;
    string message = 2;
}