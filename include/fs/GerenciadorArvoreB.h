#pragma once
#include "fs/DiscoVirtual.h"
#include "fs/MapaDeBits.h"
#include "fs/BtrfsBloco.h"
#include <expected>
#include <vector>
#include <span>

class GerenciadorArvoreB {
private:
    IDiscoVirtual& disco;
    MapaDeBits& mapa_blocos;
    uint64_t bloco_raiz_atual;
    uint64_t geracao_atual;

public:
    GerenciadorArvoreB(IDiscoVirtual& disco_ref, MapaDeBits& mapa, uint64_t raiz, uint64_t geracao);

    std::expected<uint64_t, ErroDisco> clonar_bloco_cow(uint64_t bloco_origem);

    std::expected<std::vector<std::byte>, ErroDisco> buscar_item(const ChaveBtrfs& chave_busca);
    std::expected<void, ErroDisco> inserir_item(const ChaveBtrfs& chave, std::span<const std::byte> payload);
    
    [[nodiscard]] uint64_t obter_raiz_atual() const;
    [[nodiscard]] uint64_t obter_geracao_atual() const;
};