# FlowEngine
데이터가 물 흐르듯(Flow) 막힘없이 처리되는 고성능 엔진을 추구합니다.

# NexusCore - 고성능 멀티스레드 서버 및 클라이언트

## 프로젝트 개요

NexusCore는 Windows IOCP(I/O Completion Port)를 기반으로 한 고성능 채팅 및 파일 전송 서버입니다.

### 주요 특징

- **고성능**: Windows IOCP를 활용한 비동기 I/O 처리
- **확장성**: 수천 개의 동시 연결 지원
- **안정성**: 스레드 안전한 설계와 예외 처리
- **보안**: 암호화 통신 및 인증 시스템
- **모듈화**: 독립적인 컴포넌트 구조

### 기술 스택

- **언어**: C++17
- **네트워킹**: Windows IOCP, Winsock2
- **직렬화**: Google Protocol Buffers
- **빌드 시스템**: CMake
- **테스팅**: Google Test
- **로깅**: 자체 구현 로깅 시스템

## 빌드 방법

### 필요 사항

- Visual Studio 2019 이상 (Windows SDK 포함)
- CMake 3.16 이상
- Google Protocol Buffers
- Google Test (테스트용)

### 빌드 단계

```bash
# 1. 저장소 클론
git clone https://github.com/your-repo/NexusCore.git
cd NexusCore

# 2. 빌드 디렉토리 생성
mkdir build
cd build

# 3. CMake 설정
cmake ..

# 4. 빌드
cmake --build . --config Release
```

## 실행 방법

### 서버 실행

```bash
# 설정 파일을 지정하여 실행
./NexusCore.Server.exe --config config/server.ini

# 기본 설정으로 실행
./NexusCore.Server.exe
```

### 클라이언트 실행

```bash
./NexusCore.Client.Mfc.exe
```

## 프로젝트 구조

NexusCore/
├── src/
│ ├── Common/ # 공용 라이브러리
│ ├── Core/ # 서버 핵심 로직
│ ├── Networking/ # 네트워킹 모듈
│ ├── Server/ # 서버 실행 프로젝트
│ ├── Client/ # 클라이언트 프로젝트
│ └── Admin/ # 관리자 도구
├── tests/ # 단위 테스트
├── config/ # 설정 파일
├── docs/ # 문서
└── scripts/ # 빌드 스크립트

## 라이선스

MIT License

