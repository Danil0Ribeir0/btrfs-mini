#pragma once
#include <cstdint>

struct Superbloco {
    uint32_t numero_magico; // Assinatura do FS
    uint32_t tamanho_bloco;
    uint32_t total_blocos;
    uint32_t blocos_livres;
    
    uint32_t inicio_mapa_bits; // Mapa de blocos de dados
    uint32_t blocos_mapa_bits; // Quantos blocos o mapa de bits ocupa

    uint64_t raiz_arvore_fs; // Bloco físico onde fica a raiz da nossa Árvore B
    uint64_t geracao_atual;  // ID da transação atual
};