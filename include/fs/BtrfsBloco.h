#pragma once
#include "fs/BtrfsNo.h"
#include <array>
#include <cstdint>

constexpr std::size_t TAMANHO_PAYLOAD_MAX = 264;
constexpr std::size_t MAX_ITENS_BLOCO = 13;

struct ItemBtrfs {
    ChaveBtrfs chave;
    uint32_t tamanho_payload;
    std::array<std::byte, TAMANHO_PAYLOAD_MAX> dados;
};

struct BlocoArvoreB {
    CabecalhoNoBtrfs cabecalho;
    std::array<ItemBtrfs, MAX_ITENS_BLOCO> itens;

    BlocoArvoreB() {
        cabecalho = CabecalhoNoBtrfs{};
        itens.fill(ItemBtrfs{});
    }
};