cmake_minimum_required(VERSION 4.0)

project(FurOS64 VERSION 0.1.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(BOOTX64.efi
              ${PROJECT_SOURCE_DIR}/lib/gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o
              ${PROJECT_SOURCE_DIR}/src/main.c)

target_link_directories(BOOTX64.efi
                        PRIVATE ${PROJECT_SOURCE_DIR}/lib/gnu-efi/x86_64/lib
                        PRIVATE ${PROJECT_SOURCE_DIR}/lib/gnu-efi/x86_64/gnuefi)

target_link_libraries(BOOTX64.efi PRIVATE
                      gnuefi
                      efi
)

target_compile_options(BOOTX64.efi PRIVATE -ffreestanding
                    -fpic
                    -fno-stack-protector
                    -fno-stack-check
                    -fshort-wchar
                    -mno-red-zone
                    -m64
)

target_link_options(BOOTX64.efi PRIVATE  -Wl,-shared -Wl,-Bsymbolic -nostdlib -nostartfiles
                    -Wl,-T${PROJECT_SOURCE_DIR}/gnu-efi/gnuefi/elf_x86_64_efi.lds)

target_include_directories(BOOTX64.efi PRIVATE
                          ${PROJECT_SOURCE_DIR}/include/gnu-efi/)

set_target_properties(BOOTX64.efi PROPERTIES
    BUILD_WITH_INSTALL_RPATH OFF
    INSTALL_RPATH ""
    SKIP_BUILD_RPATH YES
    LINKER_LANGUAGE C
)

add_custom_command(TARGET BOOTX64.efi POST_BUILD
                  COMMAND objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 ${CMAKE_BINARY_DIR}/efi/BOOTX64.efi ${CMAKE_BINARY_DIR}/efi/BOOTX64.efi
)
