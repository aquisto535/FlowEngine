#pragma once

#include <pcap.h>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include "../Common/Protocol.h"

namespace NexusCore {
    namespace Core {

        class Session; // 전방 선언

        // 네트워크 헤더 구조체들
#pragma pack(push, 1)

// 이더넷 헤더 (14바이트)
        struct EthernetHeader {
            uint8_t dest_mac[6];      // 목적지 MAC 주소
            uint8_t src_mac[6];       // 출발지 MAC 주소
            uint16_t ether_type;      // 이더넷 타입 (0x0800: IPv4, 0x0806: ARP)
        };

        // IP 헤더 (최소 20바이트)
        struct IpHeader {
            uint8_t version_ihl;      // 버전(4비트) + IHL(4비트)
            uint8_t tos;              // Type of Service
            uint16_t total_length;    // 전체 길이
            uint16_t identification;  // 식별자
            uint16_t flags_fragment;  // 플래그(3비트) + 단편화 오프셋(13비트)
            uint8_t ttl;              // Time To Live
            uint8_t protocol;         // 프로토콜 (6: TCP, 17: UDP)
            uint16_t checksum;        // 헤더 체크섬
            uint32_t src_ip;          // 출발지 IP
            uint32_t dest_ip;         // 목적지 IP
        };

        // TCP 헤더 (최소 20바이트)
        struct TcpHeader {
            uint16_t src_port;        // 출발지 포트
            uint16_t dest_port;       // 목적지 포트
            uint32_t seq_num;         // 시퀀스 번호
            uint32_t ack_num;         // ACK 번호
            uint8_t data_offset_reserved; // 데이터 오프셋(4비트) + 예약(4비트)
            uint8_t flags;            // TCP 플래그
            uint16_t window_size;     // 윈도우 크기
            uint16_t checksum;        // 체크섬
            uint16_t urgent_ptr;      // 긴급 포인터
        };

        // UDP 헤더 (8바이트)
        struct UdpHeader {
            uint16_t src_port;        // 출발지 포트
            uint16_t dest_port;       // 목적지 포트
            uint16_t length;          // UDP 길이
            uint16_t checksum;        // 체크섬
        };

#pragma pack(pop)

        // TCP 플래그 비트
        namespace TcpFlags {
            constexpr uint8_t FIN = 0x01;
            constexpr uint8_t SYN = 0x02;
            constexpr uint8_t RST = 0x04;
            constexpr uint8_t PSH = 0x08;
            constexpr uint8_t ACK = 0x10;
            constexpr uint8_t URG = 0x20;
        }

        // 패킷 정보 구조체
        struct PacketInfo {
            // 시간 정보
            struct timeval timestamp;

            // 이더넷 정보
            std::string src_mac_str;
            std::string dest_mac_str;
            uint16_t ether_type;

            // IP 정보
            uint8_t ip_version;
            uint8_t ip_header_length;
            uint16_t total_length;
            uint8_t protocol;
            std::string src_ip_str;
            std::string dest_ip_str;
            bool is_fragmented;

            // TCP/UDP 정보
            uint16_t src_port;
            uint16_t dest_port;
            uint32_t tcp_seq;
            uint32_t tcp_ack;
            uint8_t tcp_flags;
            uint16_t window_size;

            // 페이로드 정보
            const uint8_t* payload_data;
            size_t payload_size;

            // 패킷 전체 정보
            const uint8_t* raw_packet;
            size_t packet_size;
        };

        // 패킷 분석 결과
        enum class PacketAnalysisResult {
            SUCCESS,
            INVALID_ETHERNET,
            INVALID_IP,
            INVALID_TCP_UDP,
            UNSUPPORTED_PROTOCOL,
            FRAGMENTED_PACKET
        };

        // 패킷 처리기 콜백 함수 타입
        using PacketCallback = std::function<void(const PacketInfo&)>;

        // npcap 기반 패킷 캡처 및 분석 클래스
        class NpcapPacketAnalyzer {
        public:
            NpcapPacketAnalyzer();
            ~NpcapPacketAnalyzer();

            // 초기화 및 해제
            bool Initialize();
            void Shutdown();

            // 네트워크 인터페이스 관리
            std::vector<std::string> GetNetworkInterfaces();
            bool SetCaptureInterface(const std::string& interface_name);

            // 패킷 캡처 시작/중지
            bool StartCapture();
            void StopCapture();
            bool IsCapturing() const { return is_capturing_; }

            // 필터 설정
            bool SetBPFFilter(const std::string& filter_expression);
            void SetPortFilter(uint16_t port);
            void SetProtocolFilter(const std::string& protocol); // "tcp", "udp", "icmp"

            // 콜백 등록
            void RegisterPacketCallback(PacketCallback callback);
            void RegisterTcpCallback(PacketCallback callback);
            void RegisterUdpCallback(PacketCallback callback);
            void RegisterServerTrafficCallback(PacketCallback callback, uint16_t server_port);

            // 패킷 전송 (테스트/디버깅용)
            bool SendRawPacket(const uint8_t* packet_data, size_t packet_size);
            bool SendTcpReset(const std::string& src_ip, const std::string& dest_ip,
                uint16_t src_port, uint16_t dest_port, uint32_t seq, uint32_t ack);
            bool SendUdpPacket(const std::string& src_ip, const std::string& dest_ip,
                uint16_t src_port, uint16_t dest_port,
                const uint8_t* data, size_t data_size);

            // 통계 정보
            struct CaptureStatistics {
                uint64_t total_packets_captured;
                uint64_t tcp_packets;
                uint64_t udp_packets;
                uint64_t other_packets;
                uint64_t bytes_captured;
                uint64_t dropped_packets;
            };

            CaptureStatistics GetStatistics() const;
            void ResetStatistics();

        private:
            // pcap 관련
            pcap_t* pcap_handle_;
            std::string selected_interface_;
            std::string bpf_filter_;

            // 캡처 스레드
            std::thread capture_thread_;
            std::atomic<bool> is_capturing_;
            std::atomic<bool> should_stop_;

            // 콜백 함수들
            std::vector<PacketCallback> packet_callbacks_;
            std::vector<PacketCallback> tcp_callbacks_;
            std::vector<PacketCallback> udp_callbacks_;
            std::vector<std::pair<PacketCallback, uint16_t>> server_traffic_callbacks_;

            // 통계
            mutable std::mutex stats_mutex_;
            CaptureStatistics statistics_;

            // 내부 함수들
            void CaptureThreadProc();
            static void PacketHandler(u_char* user_data, const struct pcap_pkthdr* packet_header,
                const u_char* packet_data);
            void ProcessPacket(const struct pcap_pkthdr* packet_header, const u_char* packet_data);

            PacketAnalysisResult AnalyzePacket(const uint8_t* packet_data, size_t packet_size,
                PacketInfo& packet_info);

            // 유틸리티 함수들
            std::string MacToString(const uint8_t* mac_addr);
            std::string IpToString(uint32_t ip_addr);
            uint16_t CalculateChecksum(const uint16_t* data, size_t length);
            uint16_t CalculateUdpChecksum(const IpHeader* ip_header, const UdpHeader* udp_header,
                const uint8_t* data, size_t data_length);
            uint16_t CalculateTcpChecksum(const IpHeader* ip_header, const TcpHeader* tcp_header,
                const uint8_t* data, size_t data_length);
        };

        // NexusCore 서버 트래픽 분석기
        class ServerTrafficAnalyzer {
        public:
            ServerTrafficAnalyzer(uint16_t server_port);
            ~ServerTrafficAnalyzer();

            // 분석 시작/중지
            bool StartAnalysis();
            void StopAnalysis();

            // 연결 추적
            struct ConnectionInfo {
                std::string client_ip;
                uint16_t client_port;
                uint32_t bytes_sent;
                uint32_t bytes_received;
                uint32_t packets_sent;
                uint32_t packets_received;
                std::chrono::system_clock::time_point first_seen;
                std::chrono::system_clock::time_point last_seen;
                bool is_active;
            };

            // 실시간 모니터링 정보
            struct TrafficStats {
                uint32_t active_connections;
                uint32_t total_connections;
                uint64_t total_bytes_in;
                uint64_t total_bytes_out;
                double avg_connection_duration;
                std::vector<ConnectionInfo> top_connections;
            };

            TrafficStats GetCurrentStats();
            std::vector<ConnectionInfo> GetActiveConnections();

            // 비정상 트래픽 감지
            void SetDDoSThreshold(uint32_t connections_per_second, uint64_t bytes_per_second);
            void SetSuspiciousPatternCallback(std::function<void(const std::string&)> callback);

            // 서버 성능 테스트 지원
            bool GenerateTestTraffic(const std::string& target_ip, uint16_t target_port,
                uint32_t packet_count, uint32_t packets_per_second);

        private:
            uint16_t server_port_;
            std::unique_ptr<NpcapPacketAnalyzer> analyzer_;

            // 연결 추적
            std::mutex connections_mutex_;
            std::unordered_map<std::string, ConnectionInfo> active_connections_; // "ip:port" -> ConnectionInfo

            // 통계
            TrafficStats current_stats_;

            // DDoS 감지
            uint32_t ddos_connection_threshold_;
            uint64_t ddos_bytes_threshold_;
            std::function<void(const std::string&)> suspicious_callback_;

            // 콜백 함수들
            void OnServerPacket(const PacketInfo& packet);
            void UpdateConnectionInfo(const PacketInfo& packet);
            void DetectSuspiciousActivity(const PacketInfo& packet);

            std::string GetConnectionKey(const std::string& ip, uint16_t port);
        };

        // 기존 패킷 처리기 인터페이스 (애플리케이션 레벨)
        class IPacketHandler {
        public:
            virtual ~IPacketHandler() = default;
            virtual bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) = 0;
            virtual uint16_t GetPacketId() const = 0;
        };

        // 패킷 처리기 등록 및 디스패치
        class PacketDispatcher {
        public:
            static PacketDispatcher* GetInstance();

            // 핸들러 등록
            void RegisterHandler(uint16_t packet_id, std::unique_ptr<IPacketHandler> handler);
            void UnregisterHandler(uint16_t packet_id);

            // 패킷 처리
            bool DispatchPacket(Session* session, Protocol::PacketHeader* header, char* payload);

            // npcap 분석기 통합
            void SetTrafficAnalyzer(std::shared_ptr<ServerTrafficAnalyzer> analyzer);
            std::shared_ptr<ServerTrafficAnalyzer> GetTrafficAnalyzer();

            // 등록된 핸들러 정보
            std::vector<uint16_t> GetRegisteredPacketIds() const;

        private:
            PacketDispatcher();
            ~PacketDispatcher();

            mutable std::shared_mutex handlers_mutex_;
            std::unordered_map<uint16_t, std::unique_ptr<IPacketHandler>> handlers_;

            // npcap 통합
            std::shared_ptr<ServerTrafficAnalyzer> traffic_analyzer_;

            static PacketDispatcher* instance_;
            static std::once_flag init_flag_;
        };

        // 구체적인 패킷 핸들러들 (기존과 동일)
        class LoginHandler : public IPacketHandler {
        public:
            bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) override;
            uint16_t GetPacketId() const override { return Protocol::PacketID::LOGIN_REQ; }
        };

        class LogoutHandler : public IPacketHandler {
        public:
            bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) override;
            uint16_t GetPacketId() const override { return Protocol::PacketID::LOGOUT_REQ; }
        };

        class EnterRoomHandler : public IPacketHandler {
        public:
            bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) override;
            uint16_t GetPacketId() const override { return Protocol::PacketID::ENTER_ROOM_REQ; }
        };

        class LeaveRoomHandler : public IPacketHandler {
        public:
            bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) override;
            uint16_t GetPacketId() const override { return Protocol::PacketID::LEAVE_ROOM_REQ; }
        };

        class ChatMessageHandler : public IPacketHandler {
        public:
            bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) override;
            uint16_t GetPacketId() const override { return Protocol::PacketID::ROOM_CHAT_REQ; }
        };

        class FileUploadHandler : public IPacketHandler {
        public:
            bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) override;
            uint16_t GetPacketId() const override { return Protocol::PacketID::FILE_UPLOAD_REQ; }
        };

    } // namespace Core
} // namespace NexusCore
