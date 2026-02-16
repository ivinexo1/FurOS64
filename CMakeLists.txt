cmake_minimum_required(VERSION 4.0)

project(kernel VERSION 0.1.0 LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kernel
              ${PROJECT_SOURCE_DIR}/src/main.c
              ${PROJECT_SOURCE_DIR}/src/font3.c
              ${PROJECT_SOURCE_DIR}/src/stdlib/string.c
              ${PROJECT_SOURCE_DIR}/src/MemoryManagment/physical.c
              ${PROJECT_SOURCE_DIR}/src/Drivers/vga.c
              ${PROJECT_SOURCE_DIR}/src/Terminal/stdio.c)

target_compile_options(kernel PRIVATE
                        -ffreestanding
                        -fno-stack-protector
                        -fno-stack-check
                        -m64
                        -fPIC)

target_include_directories(kernel PRIVATE
                          ${PROJECT_SOURCE_DIR}/include/)

target_link_options(kernel PRIVATE
                    -nostdlib
                    -Wl,-T ${PROJECT_SOURCE_DIR}/linkerscript.ld
                    -Wl,--oformat binary)

add_subdirectory(${PROJECT_SOURCE_DIR}/efi/)
