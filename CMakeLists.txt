# CMakeList.txt: lay 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("nluna")




# set设置变量
# list可以添加文件、依赖库名到变量
# include_directories引用目录
# link_directories库目录
# add_executable、add_library
# add_definitions
# target_link_libraries连接依赖到目录
# INSTALL






set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# -gdwarf-2
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_SHOW_INCLUDES OFF)
set(CMAKE_DEPENDS_IN_PROJECT_ONLY ON)
#list(APPEND CMAKE_CXX_FLAGS "/showIncludes:off")
message("CX +-+-  ${CMAKE_CXX_FLAGS}")
aux_source_directory(laytest all_source)
#add_definitions(-w)
include_directories(./ tools lib3 lib2 lib3/libuv)
include_directories(
    tools/include
    external/SPIRV-Cross/
)

IF(CMAKE_CL_64)
    set(platform_i)
ELSE()
    set(platform_i 32)
ENDIF()
if(WIN32)
include_directories("C:\\sdk\\vcpkg\\installed\\${VCPKG_TARGET_TRIPLET}\\include")
include_directories("$ENV{VULKAN_SDK}\\Include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external")
# vulkan lib
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
#link_directories("$ENV{VULKAN_SDK}\\Lib${platform_i}\\Debug")
else()
include_directories("experimental")
endif()
link_directories("$ENV{VULKAN_SDK}\\Lib${platform_i}")
message("vulkan目录：$ENV{VULKAN_SDK}\\Include${platform_i}")
message("vulkan目录1：$ENV{VULKAN_SDK}\\Include")


else()
include_directories("/mnt/c/sdk/vcpkg/installed/x64-linux/include")
endif()
message("vcpkg目录：${VCPKG_ROOT}\\${VCPKG_TARGET_TRIPLET}\\include")
message("vcpkg lib目录：${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib")
# 将源代码添加到此项目的可执行文件。					AUX_SOURCE_DIRECTORY(. SRC_LIST) #把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
#file(GLOB all_source  *.cpp *.c )

  message("out dir")
  message("${CMAKE_CURRENT_SOURCE_DIR}/lib2/uv")
  message("CMAKE_LIBRARY_PATH： ${CMAKE_LIBRARY_PATH}")
  message("${CONFIG}")
  message("${REQUIRED}")
  message("${SOLUTION_DIR}/lib/${CMAKE_SYSTEM_NAME}/${TARGET_ARCH}")

if(WIN32)
    message("${CMAKE_CURRENT_SOURCE_DIR}/lib2")
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib2)
    set(VCPKG_LIB_PATH "${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib")
else()
  #set(VCPKG_LIB_PATH "/mnt/c/sdk/vcpkg/x64-linux/lib")
  #link_directories(/mnt/c/sdk/vcpkg/x64-linux/lib)
  #$LD_LIBRARY_PATH:./lib
    set(CMAKE_INSTALL_RPATH "../lib")
    set(VCPKG_LIB_PATH "${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib")
    set(CMAKE_TOOLCHAIN_FILE "/mnt/c/sdk/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
    message("lib->> ${_VCPKG_ROOT_DIR}/installed/${VCPKG_TARGET_TRIPLET}/lib")

  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
#C:\sdk\vcpkg\installed
    
  message("CMAKE_INSTALL_RPATH：${CMAKE_INSTALL_RPATH}")
message("+-+")
message("${SOLUTION_DIR}...${CMAKE_SOURCE_DIR}")
message("${VCPKG_TARGET_TRIPLET}")
message(" -》${CMAKE_TOOLCHAIN_FILE}")
message("${CMAKE_TOOLCHAIN_FILE1}")
message("${CMAKE_VCPKG_ROOT}")
message("${VCPKG_LIB_PATH}")

message("+-+")
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
message("wd->${CMAKE_RUNTIME_OUTPUT_PATH}")

#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#include(UseBackportedModules)
#if(MSVC)
#    include(CreateLaunchers)
#endif()
#create_target_launcher(tdx002 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/out/bin")

#设置库文件输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/bin)

#设置执行文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/bin)
link_directories(${PROJECT_SOURCE_DIR}/out/lib)
link_directories(${VCPKG_LIB_PATH})
# 查找包
find_package(OpenSSL REQUIRED)
find_package(unofficial-libuv CONFIG REQUIRED)


# 添加子目录
#https://github.com/KhronosGroup/SPIRV-Cross.git
#https://github.com/KhronosGroup/glslang.git
#add_subdirectory(external/SPIRV-Cross)

#add_subdirectory(src/tools)


#add_library( lay SHARED ${all_source} )
#add_executable( laytest ${all_source} )
#add_executable( tptest "thptest.cpp" )

    if (MSVC)
    set(CompilerFlags
        # CMAKE_CXX_FLAGS
        #CMAKE_CXX_FLAGS_DEBUG
        # CMAKE_CXX_FLAGS_RELEASE
        # CMAKE_C_FLAGS
        #CMAKE_C_FLAGS_DEBUG
        # CMAKE_C_FLAGS_RELEASE
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
    endif(MSVC)

    message("cx ${CMAKE_CXX_FLAGS}
      cxd ${CMAKE_CXX_FLAGS_DEBUG}
      cxr  ${CMAKE_CXX_FLAGS_RELEASE}
      c  ${CMAKE_C_FLAGS}
      cd  ${CMAKE_C_FLAGS_DEBUG}
      cr  ${CMAKE_C_FLAGS_RELEASE}")
