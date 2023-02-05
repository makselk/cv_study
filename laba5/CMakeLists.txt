cmake_minimum_required(VERSION 3.16)
project(cv_laba_5)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_laba_5
	main.cpp
	src/cube_utility.cpp
	src/custom_utility.cpp
	#calibration_utilities/calibrate_camera.cpp
	#calibration_utilities/create_board.cpp
	#calibration_utilities/create_tag.cpp
)
	
target_link_libraries(cv_laba_5 ${OpenCV_LIBS})
