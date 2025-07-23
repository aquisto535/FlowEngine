#pragma once

#include "PacketHandler.h"
#include <string>
#include <vector>

namespace NexusCore {
    namespace Core {
        namespace NpcapUtils {

            // ��Ʈ��ũ �������̽� ����
            struct NetworkInterface {
                std::string name;
                std::string description;
                std::string ip_address;
                std::string subnet_mask;
                bool is_loopback;
                bool is_up;
            };

            // ��Ŷ ���� ���� Ŭ����
            class PacketBuilder {
            public:
                PacketBuilder();
                ~PacketBuilder();

                // �̴��� ��� ����
                PacketBuilder& SetEthernet(const std::string& src_mac, const std::string& dest_mac,
                    uint16_t ether_type = 0x0800);

                // IP ��� ����
                PacketBuilder& SetIP(const std::string& src_ip, const std::string& dest_ip,
                    uint8_t protocol, uint16_t id = 0);

                // TCP ��� ����
                PacketBuilder& SetTCP(uint16_t src_port, uint16_t dest_port,
                    uint32_t seq, uint32_t ack, uint8_t flags,
                    uint16_t window_size = 8192);

                // UDP ��� ����
                PacketBuilder& SetUDP(uint16_t src_port, uint16_t dest_port);

                // ���̷ε� �߰�
                PacketBuilder& SetPayload(const uint8_t* data, size_t size);
                PacketBuilder& SetPayload(const std::string& text_data);

                // üũ�� �ڵ� ���
                PacketBuilder& CalculateChecksums();

                // ��Ŷ ����
                std::vector<uint8_t> Build();

                // ����
                void Reset();

            private:
                std::vector<uint8_t> packet_data_;
                size_t ethernet_offset_;
                size_t ip_offset_;
                size_t transport_offset_;
                size_t payload_offset_;

                uint8_t ip_protocol_;
                bool has_ethernet_;
                bool has_ip_;
                bool has_transport_;

                void UpdateOffsets();
                uint16_t CalculateIPChecksum(const IpHeader* ip_header);
            };

            // ��Ʈ��ũ ��ƿ��Ƽ �Լ���
            class NetworkUtils {
            public:
                // �������̽� ����
                static std::vector<NetworkInterface> GetNetworkInterfaces();
                static std::string GetDefaultInterface();
                static std::string GetInterfaceIP(const std::string& interface_name);

                // �ּ� ��ȯ
                static std::string MacBytesToString(const uint8_t* mac_bytes);
                static std::vector<uint8_t> MacStringToBytes(const std::string& mac_string);
                static uint32_t IpStringToInt(const std::string& ip_string);
                static std::string IpIntToString(uint32_t ip_int);

                // üũ�� ���
                static uint16_t CalculateChecksum(const uint16_t* data, size_t word_count);
                static uint16_t CalculateIPChecksum(const IpHeader* ip_header);
                static uint16_t CalculateUDPChecksum(const IpHeader* ip_header, const UdpHeader* udp_header,
                    const uint8_t* payload, size_t payload_size);
                static uint16_t CalculateTCPChecksum(const IpHeader* ip_header, const TcpHeader* tcp_header,
                    const uint8_t* payload, size_t payload_size);

                // ��Ŷ �м�
                static bool ParseEthernetHeader(const uint8_t* packet, size_t packet_size,
                    EthernetHeader* eth_header);
                static bool ParseIPHeader(const uint8_t* packet, size_t packet_size, size_t offset,
                    IpHeader* ip_header);
                static bool ParseTCPHeader(const uint8_t* packet, size_t packet_size, size_t offset,
                    TcpHeader* tcp_header);
                static bool ParseUDPHeader(const uint8_t* packet, size_t packet_size, size_t offset,
                    UdpHeader* udp_header);

                // BPF ���� ����
                static std::string CreatePortFilter(uint16_t port);
                static std::string CreateHostFilter(const std::string& ip_address);
                static std::string CreateTCPFilter();
                static std::string CreateUDPFilter();
                static std::string CreateServerFilter(uint16_t server_port);

                // ��Ŷ ���� (������)
                static std::string PacketToHexString(const uint8_t* packet, size_t size);
                static void PrintPacketInfo(const PacketInfo& packet_info);
            };

            // ���� ���� �׽�Ʈ ����
            class ServerLoadTester {
            public:
                ServerLoadTester(const std::string& server_ip, uint16_t server_port);
                ~ServerLoadTester();

                // �׽�Ʈ ����
                void SetTestDuration(uint32_t seconds);
                void SetConnectionCount(uint32_t count);
                void SetRequestsPerSecond(uint32_t rps);
                void SetPayloadSize(size_t size);

                // �׽�Ʈ ����
                bool StartLoadTest();
                void StopLoadTest();

                // ��� ����
                struct TestResults {
                    uint32_t total_connections_attempted;
                    uint32_t successful_connections;
                    uint32_t failed_connections;
                    uint32_t total_requests_sent;
                    uint32_t total_responses_received;
                    double average_response_time_ms;
                    double requests_per_second;
                    uint64_t total_bytes_sent;
                    uint64_t total_bytes_received;
                };

                TestResults GetTestResults();

            private:
                std::string server_ip_;
                uint16_t server_port_;

                // �׽�Ʈ ����
                uint32_t test_duration_seconds_;
                uint32_t connection_count_;
                uint32_t requests_per_second_;
                size_t payload_size_;

                // �׽�Ʈ ����
                std::atomic<bool> is_running_;
                std::thread test_thread_;

                // ���
                TestResults results_;
                std::mutex results_mutex_;

                void TestThreadProc();
                bool SendTestPacket();
            };

        } // namespace NpcapUtils
    } // namespace Core
} // namespace NexusCore

