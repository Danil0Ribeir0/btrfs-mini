cmake_minimum_required(VERSION 4.0)
project(BTRFS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

include_directories(include)

add_executable(BTRFS app/main.cpp
        src/DiscoVirtualEmMemoria.cpp
        src/DiscoVirtualEmMemoria.h
        include/fs/DiscoVirtual.h
        include/fs/Superbloco.h
        include/fs/MapaDeBits.h
        include/fs/Inode.h
        include/fs/BtrfsKey.h
        include/fs/BtrfsNo.h
        include/fs/BtrfsBloco.h
        include/fs/GerenciadorArvoreB.h
        src/GerenciadorArvoreB.cpp
        include/fs/Arquivo.h
        include/fs/Diretorio.h
        include/fs/SistemaDeArquivos.h
)
