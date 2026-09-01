cmake_minimum_required(VERSION 3.10)
project(GraphicsA3)

set(CMAKE_CXX_STANDARD 17)

# --- CHANGED: VENDORING GLFW ---
# Instead of looking for it on the system, we build it from the folder we provided.
# This prevents the "Could not find package" error for students.
add_subdirectory(libs/glfw) 

# --- YOUR PROJECT ---
add_executable(A3 src/Shader.cpp src/Lab3_completed.cpp src/glad.c src/Camera.cpp src/Model.cpp)

# --- INCLUDE DIRECTORIES ---
target_include_directories(A3 PRIVATE include libs/glfw/include libs/glm)

# Tell the compiler where to find your headers (include/) AND 
# where to find GLFW's headers (libs/glfw/include)
target_include_directories(A3 PRIVATE include libs/glfw/include)

# --- CHANGED: LINKING ---
# We link against the 'glfw' target that we just built in the line above.
target_link_libraries(A3 PRIVATE glfw)

# Platform specifics (Windows/Mac/Linux)
if(WIN32)
    target_link_libraries(A3 PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(A3 PRIVATE "-framework OpenGL" "-framework IOKit" "-framework Cocoa" "-framework CoreVideo")
else()
    target_link_libraries(A3 PRIVATE GL dl)
endif()
