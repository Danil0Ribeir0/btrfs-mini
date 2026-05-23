#pragma once
#include "fs/DiscoVirtual.h"
#include <vector>
#include <expected>
#include <optional>

class MapaDeBits {
private:
    IDiscoVirtual& disco;
    std::vector<std::byte> buffer_mapa;
    std::size_t bloco_inicio;
    std::size_t qtd_blocos;
    std::size_t total_blocos_rastreados;

    void marcar_bit(std::size_t indice_bloco, bool ocupado);
    [[nodiscard]] bool checar_bit(std::size_t indice_bloco) const;

public:
    MapaDeBits(IDiscoVirtual& disco_ref, std::size_t inicio, std::size_t blocos, std::size_t total_rastreados);

    std::expected<void, ErroDisco> carregar();
    
    std::expected<void, ErroDisco> sincronizar();

    [[nodiscard]] std::optional<std::size_t> alocar_bloco_livre();
    void liberar_bloco(std::size_t indice_bloco);
};