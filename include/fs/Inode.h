#pragma once
#include <cstdint>

enum class TipoInode : uint8_t {
    Livre = 0,
    Arquivo = 1,
    Diretorio = 2
};

struct Inode {
    TipoInode tipo;
    uint8_t permissoes;
    uint16_t contador_links;
    uint32_t tamanho_bytes;
    uint64_t data_criacao;
    uint64_t data_modificacao;
};