# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/patrykmartyniak/esp/esp-idf/components/bootloader/subproject"
  "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader"
  "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix"
  "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix/tmp"
  "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix/src/bootloader-stamp"
  "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix/src"
  "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/patrykmartyniak/active_balancer_transmitter/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
