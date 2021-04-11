target_link_libraries(DistanceTransformOnGPU glad glfw3dll)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
   target_link_libraries(DistanceTransformOnGPU FreeImaged)
else()
   target_link_libraries(DistanceTransformOnGPU FreeImage)
endif()