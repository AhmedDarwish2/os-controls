set(osinterfacetest_SRCS
  osinterfacetest.cpp
  AudioVolumeControllerTest.cpp
  MediaControllerTest.cpp
)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" osinterfacetest_SRCS)
add_executable(osinterfacetest ${osinterfacetest_SRCS})
target_link_libraries(osinterfacetest osinterface AutoTesting)
target_include_directories(osinterfacetest PUBLIC ..)
target_include_directories(osinterfacetest PUBLIC ${CMAKE_SOURCE_DIR}/contrib/autowiring)

# This is a unit test, let CMake know this
add_test(NAME osinterfacetest COMMAND $<TARGET_FILE:osinterfacetest>)