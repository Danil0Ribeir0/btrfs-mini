#include "fs/MapaDeBits.h"
#include <algorithm>

MapaDeBits::MapaDeBits(IDiscoVirtual& disco_ref, std::size_t inicio, std::size_t blocos, std::size_t total_rastreados)
    : disco(disco_ref), bloco_inicio(inicio), qtd_blocos(blocos), total_blocos_rastreados(total_rastreados) {
    buffer_mapa.resize(qtd_blocos * TAMANHO_BLOCO, std::byte{0});
}

void MapaDeBits::marcar_bit(std::size_t indice_bloco, bool ocupado) {
    if (indice_bloco >= total_blocos_rastreados) return;
    
    std::size_t indice_byte = indice_bloco / 8;
    std::size_t indice_bit = indice_bloco % 8;
    
    if (ocupado) {
        buffer_mapa[indice_byte] |= (std::byte{1} << indice_bit);
    } else {
        buffer_mapa[indice_byte] &= ~(std::byte{1} << indice_bit);
    }
}

bool MapaDeBits::checar_bit(std::size_t indice_bloco) const {
    if (indice_bloco >= total_blocos_rastreados) return true;
    
    std::size_t indice_byte = indice_bloco / 8;
    std::size_t indice_bit = indice_bloco % 8;
    
    return (buffer_mapa[indice_byte] & (std::byte{1} << indice_bit)) != std::byte{0};
}

std::expected<void, ErroDisco> MapaDeBits::carregar() {
    for (std::size_t i = 0; i < qtd_blocos; ++i) {
        auto span_bloco = std::span<std::byte, TAMANHO_BLOCO>(buffer_mapa.data() + (i * TAMANHO_BLOCO), TAMANHO_BLOCO);
        auto res = disco.ler_bloco(bloco_inicio + i, span_bloco);
        if (!res) return res;
    }
    return {};
}

std::expected<void, ErroDisco> MapaDeBits::sincronizar() {
    for (std::size_t i = 0; i < qtd_blocos; ++i) {
        auto span_bloco = std::span<const std::byte, TAMANHO_BLOCO>(buffer_mapa.data() + (i * TAMANHO_BLOCO), TAMANHO_BLOCO);
        auto res = disco.escrever_bloco(bloco_inicio + i, span_bloco);
        if (!res) return res;
    }
    return {};
}

std::optional<std::size_t> MapaDeBits::alocar_bloco_livre() {
    for (std::size_t i = 0; i < total_blocos_rastreados; ++i) {
        if (!checar_bit(i)) {
            marcar_bit(i, true);

            if (!sincronizar()) {}

            return i;
        }
    }
    return std::nullopt;
}

void MapaDeBits::liberar_bloco(std::size_t indice_bloco) {
    marcar_bit(indice_bloco, false);

    if (!sincronizar()) {}
}