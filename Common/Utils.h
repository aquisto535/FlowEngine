#pragma once

#include <string>
#include <vector>
#include <chrono>
#include "Winsock2.h"

namespace NexusCore {
    namespace Common {
        namespace Utils {

            // 문자열 유틸리티
            class StringUtils {
            public:
                static std::vector<std::string> Split(const std::string& str, char delimiter);
                static std::string Trim(const std::string& str);
                static std::string ToLower(const std::string& str);
                static std::string ToUpper(const std::string& str);
                static bool StartsWith(const std::string& str, const std::string& prefix);
                static bool EndsWith(const std::string& str, const std::string& suffix);
                static std::string Replace(const std::string& str, const std::string& from, const std::string& to);
            };

            // 시간 유틸리티
            class TimeUtils {
            public:
                static int64_t GetCurrentTimestamp();
                static std::string GetCurrentTimeString(const std::string& format = "%Y-%m-%d %H:%M:%S");
                static std::string TimestampToString(int64_t timestamp, const std::string& format = "%Y-%m-%d %H:%M:%S");
                static int64_t StringToTimestamp(const std::string& time_str, const std::string& format = "%Y-%m-%d %H:%M:%S");
            };

            // 파일 유틸리티
            class FileUtils {
            public:
                static bool FileExists(const std::string& file_path);
                static bool DirectoryExists(const std::string& dir_path);
                static bool CreateDirectory(const std::string& dir_path);
                static bool DeleteFile(const std::string& file_path);
                static uint64_t GetFileSize(const std::string& file_path);
                static std::string GetFileExtension(const std::string& file_path);
                static std::string GetFileName(const std::string& file_path);
                static std::string GetDirectoryPath(const std::string& file_path);
                static std::vector<std::string> ListFiles(const std::string& dir_path, const std::string& extension = "");
            };

            // 암호화 유틸리티
            class CryptoUtils {
            public:
                static std::string CalculateMD5(const char* data, size_t size);
                static std::string CalculateSHA256(const char* data, size_t size);
                static uint32_t CalculateCRC32(const char* data, size_t size);
                static std::string Base64Encode(const char* data, size_t size);
                static std::vector<char> Base64Decode(const std::string& encoded);
                static std::string GenerateRandomString(size_t length);
                static std::vector<char> GenerateRandomBytes(size_t length);
            };

            // 네트워크 유틸리티
            class NetworkUtils {
            public:
                static std::string GetLocalIPAddress();
                static bool IsValidIPAddress(const std::string& ip);
                static bool IsValidPort(int port);
                static std::string SocketToString(SOCKET socket);
                static std::string GetPeerAddress(SOCKET socket);
            };

        } // namespace Utils
    } // namespace Common
} // namespace NexusCore