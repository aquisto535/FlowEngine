#pragma once

#include <exception>
#include <string>

namespace NexusCore {
    namespace Common {

        // �⺻ ���� Ŭ����
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

        // ��Ʈ��ũ ���� ����
        class NetworkException : public NexusException {
        public:
            explicit NetworkException(const std::string& message)
                : NexusException("Network Error: " + message) {
            }
        };

        // �������� ���� ����
        class ProtocolException : public NexusException {
        public:
            explicit ProtocolException(const std::string& message)
                : NexusException("Protocol Error: " + message) {
            }
        };

        // ���� ���� ����
        class ConfigException : public NexusException {
        public:
            explicit ConfigException(const std::string& message)
                : NexusException("Config Error: " + message) {
            }
        };

        // ���� I/O ���� ����
        class FileException : public NexusException {
        public:
            explicit FileException(const std::string& message)
                : NexusException("File Error: " + message) {
            }
        };

        // ��ȣȭ ���� ����
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
