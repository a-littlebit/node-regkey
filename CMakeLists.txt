# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.5.0)

# 项目信息
project(regkey)

# 输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIRR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# 搜索头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

# 在未使用 cmake-js 构建时，搜索 cmake-js 包含目录以支持 IDE 提示
if (NOT DEFINED CMAKE_JS_INC)
  if (WIN32)
    find_file(CMAKE_JS_PATH cmake-js.cmd)
  else()
    find_file(CMAKE_JS_PATH cmake-js)
  endif()
  execute_process(COMMAND ${CMAKE_JS_PATH} print-cmakejs-include
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE CMAKE_JS_INC
  )
endif ()
include_directories(${CMAKE_JS_INC})

# 搜索源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_FILES)

# 添加库文件
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

# Node addon API
execute_process(COMMAND node -p "require('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_INC
)
string(REPLACE "\n" "" NODE_ADDON_API_INC ${NODE_ADDON_API_INC})
string(REPLACE "\"" "" NODE_ADDON_API_INC ${NODE_ADDON_API_INC})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_INC})
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})

add_definitions(-DNAPI_VERSION=8)
