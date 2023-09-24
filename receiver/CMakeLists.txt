cmake_minimum_required(VERSION 2.8)

project( Receiver )

find_package( OpenCV REQUIRED )

add_executable( Receiver receiver.cpp )
target_link_libraries( Receiver ${OpenCV_LIBS} )