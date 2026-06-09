cmake_minimum_required(VERSION 3.10)

project(Minesweeper)

# 设置可执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(EGE_LIB_PATH E:/Code/Cpp/lib/ege-25.11.0)   # <--------设置EGE库的路径

# 查找目录中的所有源文件（将所有的源码保存到 SRC_LIST 变量中）
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

# 指定头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 定义ege库
add_library(ege INTERFACE)
target_include_directories(ege INTERFACE ${EGE_LIB_PATH}/include)
target_link_directories(ege INTERFACE ${EGE_LIB_PATH}/lib/mingw64)
target_link_libraries(ege INTERFACE graphics gdiplus gdi32 imm32 msimg32 ole32 oleaut32 winmm uuid)

# 使用指定的源文件向项目添加可执行文件
add_executable(Minesweeper ${SRC_LIST})

# 将源添加到目标
target_link_libraries(Minesweeper PRIVATE ege ws2_32)