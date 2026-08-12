cmake_minimum_required(VERSION 4.0)

project(kernel VERSION 0.1.0 LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kernel
              ${PROJECT_SOURCE_DIR}/src/main.c
              ${PROJECT_SOURCE_DIR}/src/font_mononoki.c
              ${PROJECT_SOURCE_DIR}/src/stdlib/string.c
              ${PROJECT_SOURCE_DIR}/src/MemoryManagment/physical.c
              ${PROJECT_SOURCE_DIR}/src/Drivers/vga.c
              ${PROJECT_SOURCE_DIR}/src/Terminal/stdio.c
              ${PROJECT_SOURCE_DIR}/src/Arch/Interrupts/idt.c
              ${PROJECT_SOURCE_DIR}/src/Arch/Interrupts/isr.c
              ${PROJECT_SOURCE_DIR}/src/decoder_qoi.c)

file(GLOB ASSET_FILES ${PROJECT_SOURCE_DIR}/assets/*.qoi)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
foreach(ASSET ${ASSET_FILES})
    get_filename_component(ASSET_NAME ${ASSET} NAME_WE)
    set(ASSET_OBJ ${CMAKE_BINARY_DIR}/assets/${ASSET_NAME}.o)

    add_custom_command(
        OUTPUT ${ASSET_OBJ}
        COMMAND objcopy -I binary -O elf64-x86-64 -B i386:x86-64
                ${ASSET_NAME}.qoi ${ASSET_OBJ}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/assets
        DEPENDS ${ASSET}
    )
    list(APPEND ASSET_OBJS ${ASSET_OBJ})
endforeach()

target_sources(kernel PRIVATE ${ASSET_OBJS})

target_compile_options(kernel PRIVATE
                        -ffreestanding
                        -fno-stack-protector
                        -fno-stack-check
                        -m64)

target_include_directories(kernel PRIVATE
                          ${PROJECT_SOURCE_DIR}/include/)

target_link_options(kernel PRIVATE
                    -nostdlib
                    -Wl,-T ${PROJECT_SOURCE_DIR}/linkerscript.ld
                    -Wl,--oformat binary)

add_subdirectory(${PROJECT_SOURCE_DIR}/efi/)
