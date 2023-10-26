cmake_minimum_required(VERSION 3.5)

project(QHttpServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUSE_IMPORT_EXPORT)
add_definitions(-DUSE_WINDOWS_DLL_SEMANTICS)
add_definitions(-D_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)

#指定exe输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#指定lib输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


##############################

# 设置源文件夹路径和目标文件夹路径
set(SOURCE_FOLDER ${PROJECT_SOURCE_DIR}/file)
set(TARGET_FOLDER ${CMAKE_BINARY_DIR}/bin)

# 获取源文件夹中的所有文件
file(GLOB FILES_TO_COPY ${SOURCE_FOLDER}/*)

# 遍历所有文件，并将其复制到目标文件夹
foreach(FILE ${FILES_TO_COPY})
  file(COPY ${FILE} DESTINATION ${TARGET_FOLDER})
endforeach()

##############################

# 设置头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/inc/pthread
    ${PROJECT_SOURCE_DIR}/inc/libevent/windows
    ${PROJECT_SOURCE_DIR}/inc/mysql
    ${PROJECT_SOURCE_DIR}/inc/librabbitmq
    ${PROJECT_SOURCE_DIR}/inc/boost
    ${PROJECT_SOURCE_DIR}/inc/opencv
    ${PROJECT_SOURCE_DIR}/inc/aws
)

# 设置库文件路径
link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/lib/curl/windows/Release
    ${PROJECT_SOURCE_DIR}/lib/libevent/windows/Release
    ${PROJECT_SOURCE_DIR}/lib/mysql/windows/Release
    ${PROJECT_SOURCE_DIR}/lib/librabbitmq/windows/Release
    ${PROJECT_SOURCE_DIR}/lib/boost/windows/Release
    ${PROJECT_SOURCE_DIR}/lib/opencv/windows/Release
    ${PROJECT_SOURCE_DIR}/lib/aws/windows/Release
)

# 添加源文件
add_executable(QHttpServer
    main.cpp
    awsUpload.cpp
    digitalEntityJson.cpp
    digitalmysql.cpp
    httpConcurrencyServer.cpp
    httpkit.cpp
    json.cpp
    logger.cpp
    mmRabbitmq.cpp
    public.cpp
    shareMemory.cpp
    ssoDataFrom.cpp
    videoOperate.cpp
)

# 链接库文件
target_link_libraries(QHttpServer
)

##############################
