# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aziz/esp/v5.1.2/esp-idf/components/bootloader/subproject"
  "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader"
  "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix"
  "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix/tmp"
  "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix/src/bootloader-stamp"
  "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix/src"
  "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aziz/Documents/Uni/ECE492_project/hardware/wifi_module/wifi_enterprise/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
