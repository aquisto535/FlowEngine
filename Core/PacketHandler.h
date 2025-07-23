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

        class Session; // ���� ����

        // ��Ʈ��ũ ��� ����ü��
#pragma pack(push, 1)

// �̴��� ��� (14����Ʈ)
        struct EthernetHeader {
            uint8_t dest_mac[6];      // ������ MAC �ּ�
            uint8_t src_mac[6];       // ����� MAC �ּ�
            uint16_t ether_type;      // �̴��� Ÿ�� (0x0800: IPv4, 0x0806: ARP)
        };

        // IP ��� (�ּ� 20����Ʈ)
        struct IpHeader {
            uint8_t version_ihl;      // ����(4��Ʈ) + IHL(4��Ʈ)
            uint8_t tos;              // Type of Service
            uint16_t total_length;    // ��ü ����
            uint16_t identification;  // �ĺ���
            uint16_t flags_fragment;  // �÷���(3��Ʈ) + ����ȭ ������(13��Ʈ)
            uint8_t ttl;              // Time To Live
            uint8_t protocol;         // �������� (6: TCP, 17: UDP)
            uint16_t checksum;        // ��� üũ��
            uint32_t src_ip;          // ����� IP
            uint32_t dest_ip;         // ������ IP
        };

        // TCP ��� (�ּ� 20����Ʈ)
        struct TcpHeader {
            uint16_t src_port;        // ����� ��Ʈ
            uint16_t dest_port;       // ������ ��Ʈ
            uint32_t seq_num;         // ������ ��ȣ
            uint32_t ack_num;         // ACK ��ȣ
            uint8_t data_offset_reserved; // ������ ������(4��Ʈ) + ����(4��Ʈ)
            uint8_t flags;            // TCP �÷���
            uint16_t window_size;     // ������ ũ��
            uint16_t checksum;        // üũ��
            uint16_t urgent_ptr;      // ��� ������
        };

        // UDP ��� (8����Ʈ)
        struct UdpHeader {
            uint16_t src_port;        // ����� ��Ʈ
            uint16_t dest_port;       // ������ ��Ʈ
            uint16_t length;          // UDP ����
            uint16_t checksum;        // üũ��
        };

#pragma pack(pop)

        // TCP �÷��� ��Ʈ
        namespace TcpFlags {
            constexpr uint8_t FIN = 0x01;
            constexpr uint8_t SYN = 0x02;
            constexpr uint8_t RST = 0x04;
            constexpr uint8_t PSH = 0x08;
            constexpr uint8_t ACK = 0x10;
            constexpr uint8_t URG = 0x20;
        }

        // ��Ŷ ���� ����ü
        struct PacketInfo {
            // �ð� ����
            struct timeval timestamp;

            // �̴��� ����
            std::string src_mac_str;
            std::string dest_mac_str;
            uint16_t ether_type;

            // IP ����
            uint8_t ip_version;
            uint8_t ip_header_length;
            uint16_t total_length;
            uint8_t protocol;
            std::string src_ip_str;
            std::string dest_ip_str;
            bool is_fragmented;

            // TCP/UDP ����
            uint16_t src_port;
            uint16_t dest_port;
            uint32_t tcp_seq;
            uint32_t tcp_ack;
            uint8_t tcp_flags;
            uint16_t window_size;

            // ���̷ε� ����
            const uint8_t* payload_data;
            size_t payload_size;

            // ��Ŷ ��ü ����
            const uint8_t* raw_packet;
            size_t packet_size;
        };

        // ��Ŷ �м� ���
        enum class PacketAnalysisResult {
            SUCCESS,
            INVALID_ETHERNET,
            INVALID_IP,
            INVALID_TCP_UDP,
            UNSUPPORTED_PROTOCOL,
            FRAGMENTED_PACKET
        };

        // ��Ŷ ó���� �ݹ� �Լ� Ÿ��
        using PacketCallback = std::function<void(const PacketInfo&)>;

        // npcap ��� ��Ŷ ĸó �� �м� Ŭ����
        class NpcapPacketAnalyzer {
        public:
            NpcapPacketAnalyzer();
            ~NpcapPacketAnalyzer();

            // �ʱ�ȭ �� ����
            bool Initialize();
            void Shutdown();

            // ��Ʈ��ũ �������̽� ����
            std::vector<std::string> GetNetworkInterfaces();
            bool SetCaptureInterface(const std::string& interface_name);

            // ��Ŷ ĸó ����/����
            bool StartCapture();
            void StopCapture();
            bool IsCapturing() const { return is_capturing_; }

            // ���� ����
            bool SetBPFFilter(const std::string& filter_expression);
            void SetPortFilter(uint16_t port);
            void SetProtocolFilter(const std::string& protocol); // "tcp", "udp", "icmp"

            // �ݹ� ���
            void RegisterPacketCallback(PacketCallback callback);
            void RegisterTcpCallback(PacketCallback callback);
            void RegisterUdpCallback(PacketCallback callback);
            void RegisterServerTrafficCallback(PacketCallback callback, uint16_t server_port);

            // ��Ŷ ���� (�׽�Ʈ/������)
            bool SendRawPacket(const uint8_t* packet_data, size_t packet_size);
            bool SendTcpReset(const std::string& src_ip, const std::string& dest_ip,
                uint16_t src_port, uint16_t dest_port, uint32_t seq, uint32_t ack);
            bool SendUdpPacket(const std::string& src_ip, const std::string& dest_ip,
                uint16_t src_port, uint16_t dest_port,
                const uint8_t* data, size_t data_size);

            // ��� ����
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
            // pcap ����
            pcap_t* pcap_handle_;
            std::string selected_interface_;
            std::string bpf_filter_;

            // ĸó ������
            std::thread capture_thread_;
            std::atomic<bool> is_capturing_;
            std::atomic<bool> should_stop_;

            // �ݹ� �Լ���
            std::vector<PacketCallback> packet_callbacks_;
            std::vector<PacketCallback> tcp_callbacks_;
            std::vector<PacketCallback> udp_callbacks_;
            std::vector<std::pair<PacketCallback, uint16_t>> server_traffic_callbacks_;

            // ���
            mutable std::mutex stats_mutex_;
            CaptureStatistics statistics_;

            // ���� �Լ���
            void CaptureThreadProc();
            static void PacketHandler(u_char* user_data, const struct pcap_pkthdr* packet_header,
                const u_char* packet_data);
            void ProcessPacket(const struct pcap_pkthdr* packet_header, const u_char* packet_data);

            PacketAnalysisResult AnalyzePacket(const uint8_t* packet_data, size_t packet_size,
                PacketInfo& packet_info);

            // ��ƿ��Ƽ �Լ���
            std::string MacToString(const uint8_t* mac_addr);
            std::string IpToString(uint32_t ip_addr);
            uint16_t CalculateChecksum(const uint16_t* data, size_t length);
            uint16_t CalculateUdpChecksum(const IpHeader* ip_header, const UdpHeader* udp_header,
                const uint8_t* data, size_t data_length);
            uint16_t CalculateTcpChecksum(const IpHeader* ip_header, const TcpHeader* tcp_header,
                const uint8_t* data, size_t data_length);
        };

        // NexusCore ���� Ʈ���� �м���
        class ServerTrafficAnalyzer {
        public:
            ServerTrafficAnalyzer(uint16_t server_port);
            ~ServerTrafficAnalyzer();

            // �м� ����/����
            bool StartAnalysis();
            void StopAnalysis();

            // ���� ����
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

            // �ǽð� ����͸� ����
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

            // ������ Ʈ���� ����
            void SetDDoSThreshold(uint32_t connections_per_second, uint64_t bytes_per_second);
            void SetSuspiciousPatternCallback(std::function<void(const std::string&)> callback);

            // ���� ���� �׽�Ʈ ����
            bool GenerateTestTraffic(const std::string& target_ip, uint16_t target_port,
                uint32_t packet_count, uint32_t packets_per_second);

        private:
            uint16_t server_port_;
            std::unique_ptr<NpcapPacketAnalyzer> analyzer_;

            // ���� ����
            std::mutex connections_mutex_;
            std::unordered_map<std::string, ConnectionInfo> active_connections_; // "ip:port" -> ConnectionInfo

            // ���
            TrafficStats current_stats_;

            // DDoS ����
            uint32_t ddos_connection_threshold_;
            uint64_t ddos_bytes_threshold_;
            std::function<void(const std::string&)> suspicious_callback_;

            // �ݹ� �Լ���
            void OnServerPacket(const PacketInfo& packet);
            void UpdateConnectionInfo(const PacketInfo& packet);
            void DetectSuspiciousActivity(const PacketInfo& packet);

            std::string GetConnectionKey(const std::string& ip, uint16_t port);
        };

        // ���� ��Ŷ ó���� �������̽� (���ø����̼� ����)
        class IPacketHandler {
        public:
            virtual ~IPacketHandler() = default;
            virtual bool HandlePacket(Session* session, Protocol::PacketHeader* header, char* payload) = 0;
            virtual uint16_t GetPacketId() const = 0;
        };

        // ��Ŷ ó���� ��� �� ����ġ
        class PacketDispatcher {
        public:
            static PacketDispatcher* GetInstance();

            // �ڵ鷯 ���
            void RegisterHandler(uint16_t packet_id, std::unique_ptr<IPacketHandler> handler);
            void UnregisterHandler(uint16_t packet_id);

            // ��Ŷ ó��
            bool DispatchPacket(Session* session, Protocol::PacketHeader* header, char* payload);

            // npcap �м��� ����
            void SetTrafficAnalyzer(std::shared_ptr<ServerTrafficAnalyzer> analyzer);
            std::shared_ptr<ServerTrafficAnalyzer> GetTrafficAnalyzer();

            // ��ϵ� �ڵ鷯 ����
            std::vector<uint16_t> GetRegisteredPacketIds() const;

        private:
            PacketDispatcher();
            ~PacketDispatcher();

            mutable std::shared_mutex handlers_mutex_;
            std::unordered_map<uint16_t, std::unique_ptr<IPacketHandler>> handlers_;

            // npcap ����
            std::shared_ptr<ServerTrafficAnalyzer> traffic_analyzer_;

            static PacketDispatcher* instance_;
            static std::once_flag init_flag_;
        };

        // ��ü���� ��Ŷ �ڵ鷯�� (������ ����)
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
