#pragma once
#include <cstdint>
#include <array>

constexpr std::size_t TAMANHO_INODE = 128;
constexpr std::size_t NUM_BLOCOS_DIRETOS = 12;

enum class TipoInode : uint8_t {
    Livre = 0,
    Arquivo = 1,
    Diretorio = 2
};

struct Inode {
    // === Cabeçalho do Metadado (24 bytes) ===
    TipoInode tipo;
    uint8_t permissoes;
    uint16_t contador_links;
    uint32_t tamanho_bytes;
    uint64_t data_criacao;
    uint64_t data_modificacao;

    std::array<uint32_t, NUM_BLOCOS_DIRETOS> blocos_diretos;
    uint32_t bloco_indireto_simples;
    std::array<uint8_t, 52> padding;
};

static_assert(sizeof(Inode) == TAMANHO_INODE, "Erro crítico: O tamanho do Inode deve ser exatamente 128 bytes para o correto alinhamento no disco.");