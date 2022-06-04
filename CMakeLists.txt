cmake_minimum_required(VERSION 3.16)
project(VulkanTutorial)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

set(GLFW_DIR "submodules/glfw")
add_subdirectory(${GLFW_DIR} EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME}
    main.cpp
    HelloTriangleApplication.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLFW_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    ${GLFW_LIBRARIES})