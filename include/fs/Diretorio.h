#pragma once
#include <cstdint>
#include <array>
#include <string_view>
#include <algorithm>

constexpr std::size_t TAMANHO_MAX_NOME = 28;

struct EntradaDiretorio {
    uint32_t indice_inode;                           
    std::array<char, TAMANHO_MAX_NOME> nome_arquivo; 

    EntradaDiretorio() : indice_inode(0) {
        nome_arquivo.fill('\0');
    }

    EntradaDiretorio(uint32_t inode, std::string_view nome) : indice_inode(inode) {
        nome_arquivo.fill('\0');
        std::size_t tamanho_copia = std::min(nome.size(), TAMANHO_MAX_NOME - 1);
        std::copy_n(nome.begin(), tamanho_copia, nome_arquivo.begin());
    }

    [[nodiscard]] bool esta_vazia() const {
        return indice_inode == 0;
    }
};

static_assert(sizeof(EntradaDiretorio) == 32, "Erro crítico: EntradaDiretorio deve ter 32 bytes.");