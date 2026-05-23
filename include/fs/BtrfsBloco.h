#pragma once
#include "fs/BtrfsNo.h"
#include <array>
#include <cstdint>

template <typename TPayload>
struct ItemBtrfs {
    ChaveBtrfs chave;
    uint32_t tamanho_payload;
    TPayload dados;
};

template <typename TPayload, std::size_t MaxItens>
struct BlocoArvoreB {
    CabecalhoNoBtrfs cabecalho;
    std::array<ItemBtrfs<TPayload>, MaxItens> itens; 
    
    BlocoArvoreB() {
        cabecalho = CabecalhoNoBtrfs{};
        itens.fill(ItemBtrfs<TPayload>{});
    }

    static constexpr std::size_t tamanho_total() {
        return sizeof(CabecalhoNoBtrfs) + (sizeof(ItemBtrfs<TPayload>) * MaxItens);
    }
};

static_assert(TAMANHO_BLOCO == 4096, "Erro critico: O motor BTRFS requer blocos estritos de 4KB.");