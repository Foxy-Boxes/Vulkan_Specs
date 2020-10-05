cmake_minimum_required(VERSION 3.16)
project(Vulkan_Specs)

set(CMAKE_CXX_STANDARD 17)

include_directories("Vulkan_git/gen/include" "/usr/local/share/vulkan/explicit_layer.d")
add_definitions(-DBUILD_TYPE=${BUILD_TYPE} -DGLFW_VULKAN_STATIC=ON)
add_executable(Vulkan_Specs main.cpp  global_inline.h Initializer.cpp Initializer.h build_configs.h BackgroundCheck.cpp BackgroundCheck.h PipelineResourceSetup.cpp PipelineResourceSetup.h Renderer.cpp Renderer.h Initialization/Filler.cpp Initialization/Filler.h Initialization/LayerManager.cpp Initialization/LayerManager.h Initialization/ExtensionManager.cpp Initialization/ExtensionManager.h Initialization/Creater.cpp Initialization/Creater.h Initialization/Types.h)
find_library(Vulkan_Specs vulkan glfw "~/Downloads/1.2.141.2/x86_64/lib" "/usr/lib")
target_link_libraries(Vulkan_Specs glfw vulkan xcb pthread)

