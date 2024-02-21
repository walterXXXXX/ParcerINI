cmake_minimum_required(VERSION 3.22.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(Parcer_of_INI_files)

add_library(ParcerLib "parcer.h" "parcer.cpp"  )

add_executable(parcerINI "main.cpp" "parcer.h" "parcer.cpp"  )

target_link_libraries(parcerINI ParcerLib)