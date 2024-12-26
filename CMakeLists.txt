cmake_minimum_required(VERSION 3.5.0)

# This file is used to provide IDE prompts using cmake-js.

project(regkey)

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

# Node addon API
execute_process(COMMAND node -p "require('node-addon-api').include"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_INC
)
string(REPLACE "\n" "" NODE_ADDON_API_INC ${NODE_ADDON_API_INC})
string(REPLACE "\"" "" NODE_ADDON_API_INC ${NODE_ADDON_API_INC})

set(REGKEY_INC ${CMAKE_JS_INC} ${NODE_ADDON_API_INC} ${CMAKE_SOURCE_DIR}/include)

set(REGKEY_LIB ${CMAKE_JS_LIB})

file(GLOB REGKEY_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(APPEND REGKEY_SRC ${CMAKE_JS_SRC})

set(REGKEY_TARGET ${PROJECT_NAME})
add_library(${REGKEY_TARGET} SHARED ${REGKEY_SRC})

set_target_properties(${REGKEY_TARGET} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(${REGKEY_TARGET} PRIVATE ${REGKEY_INC})
target_link_libraries(${REGKEY_TARGET} ${REGKEY_LIB})

# NAPI version
add_definitions(-DNAPI_VERSION=8)
