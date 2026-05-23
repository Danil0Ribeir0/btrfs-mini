#pragma once
#include "fs/BtrfsKey.h"
#include <cstdint>
#include <array>

constexpr std::size_t TAMANHO_BLOCO = 4096;

struct CabecalhoNoBtrfs {
    uint64_t id_bloco_fisico;
    uint64_t id_geracao;
    uint16_t nivel_arvore;
    uint16_t qtd_itens;
    
    std::array<uint8_t, 12> padding;
};

static_assert(sizeof(CabecalhoNoBtrfs) == 32, "Erro crítico: CabecalhoNoBtrfs deve ter exatamente 32 bytes.");