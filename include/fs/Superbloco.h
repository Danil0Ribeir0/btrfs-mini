#pragma once
#include <cstdint>

struct Superbloco {
    uint32_t numero_magico;
    uint32_t tamanho_bloco;
    uint32_t total_blocos;
    uint32_t blocos_livres;
    
    uint32_t inicio_mapa_bits;
    uint32_t blocos_mapa_bits;

    uint32_t inicio_mapa_inodes;
    uint32_t blocos_mapa_inodes;

    uint32_t inicio_inodes;
    uint32_t total_inodes;
    uint32_t bloco_raiz;
};