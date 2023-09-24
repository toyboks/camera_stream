cmake_minimum_required(VERSION 2.8)

project( Transmitter )

find_package( OpenCV REQUIRED )

add_executable( Transmitter transmitter.cpp )
target_link_libraries( Transmitter ${OpenCV_LIBS} )