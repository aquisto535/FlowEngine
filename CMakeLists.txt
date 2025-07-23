cmake_minimum_required(VERSION 3.16)
project(NexusCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 윈도우 타겟 버전 설정
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_WIN32_WINNT=0x0601) # Windows 7 이상
    add_definitions(-DHAVE_REMOTE)
endif()

# 외부 라이브러리 찾기
find_package(Protobuf REQUIRED)

# npcap 라이브러리 설정
if(WIN32)
    # npcap 설치 경로 (환경에 따라 조정 필요)
    set(NPCAP_ROOT "C:/npcap" CACHE PATH "npcap installation directory")
    
    if(EXISTS ${NPCAP_ROOT})
        set(NPCAP_INCLUDE_DIR ${NPCAP_ROOT}/include)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(NPCAP_LIBRARY_DIR ${NPCAP_ROOT}/Lib/x64)
        else()
            set(NPCAP_LIBRARY_DIR ${NPCAP_ROOT}/Lib)
        endif()
        
        find_library(NPCAP_LIBRARY 
            NAMES wpcap
            PATHS ${NPCAP_LIBRARY_DIR}
            NO_DEFAULT_PATH
        )
        
        find_library(PACKET_LIBRARY
            NAMES Packet
            PATHS ${NPCAP_LIBRARY_DIR}
            NO_DEFAULT_PATH
        )
        
        if(NPCAP_LIBRARY AND PACKET_LIBRARY)
            set(NPCAP_FOUND TRUE)
            message(STATUS "npcap found: ${NPCAP_LIBRARY}")
        else()
            message(WARNING "npcap libraries not found in ${NPCAP_LIBRARY_DIR}")
        endif()
    else()
        message(WARNING "npcap not found at ${NPCAP_ROOT}")
    endif()
endif()

# 솔루션 폴더 구조 설정
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 공통 라이브러리
add_subdirectory(src/Common)

# 서버 코어 라이브러리  
add_subdirectory(src/Core)

# 네트워킹 라이브러리
add_subdirectory(src/Networking)

# 서버 실행 프로젝트
add_subdirectory(src/Server)

# 클라이언트 프로젝트
add_subdirectory(src/Client)

# 관리자 도구
add_subdirectory(src/Admin)

# 테스트 프로젝트
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# npcap 기반 네트워크 분석 도구
if(NPCAP_FOUND)
    add_subdirectory(tools/NetworkAnalyzer)
endif()

# 전역 컴파일 옵션
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_options(/MP) # 멀티프로세서 컴파일
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# npcap 전역 설정
if(NPCAP_FOUND)
    add_compile_definitions(HAVE_NPCAP)
endif()