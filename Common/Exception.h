#pragma once

#include <exception>
#include <string>

namespace NexusCore {
    namespace Common {

        // 기본 예외 클래스
        class NexusException : public std::exception {
        public:
            explicit NexusException(const std::string& message) : message_(message) {}
            virtual ~NexusException() noexcept = default;

            const char* what() const noexcept override {
                return message_.c_str();
            }

            const std::string& GetMessage() const { return message_; }

        protected:
            std::string message_;
        };

        // 네트워크 관련 예외
        class NetworkException : public NexusException {
        public:
            explicit NetworkException(const std::string& message)
                : NexusException("Network Error: " + message) {
            }
        };

        // 프로토콜 관련 예외
        class ProtocolException : public NexusException {
        public:
            explicit ProtocolException(const std::string& message)
                : NexusException("Protocol Error: " + message) {
            }
        };

        // 설정 관련 예외
        class ConfigException : public NexusException {
        public:
            explicit ConfigException(const std::string& message)
                : NexusException("Config Error: " + message) {
            }
        };

        // 파일 I/O 관련 예외
        class FileException : public NexusException {
        public:
            explicit FileException(const std::string& message)
                : NexusException("File Error: " + message) {
            }
        };

        // 암호화 관련 예외
        class CryptoException : public NexusException {
        public:
            explicit CryptoException(const std::string& message)
                : NexusException("Crypto Error: " + message) {
            }
        };

    } // namespace Common
} // namespace NexusCore

class Exception
{
};
