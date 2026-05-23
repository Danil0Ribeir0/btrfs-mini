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

    template <typename TPayload, std::size_t MaxItens>
    std::expected<TPayload, ErroDisco> buscar_item_tipado(const ChaveBtrfs& chave_busca) {
        std::array<std::byte, TAMANHO_BLOCO> buffer;
        auto res = disco.ler_bloco(bloco_raiz_atual, buffer);
        if (!res) return std::unexpected(res.error());

        auto* bloco = reinterpret_cast<BlocoArvoreB<TPayload, MaxItens>*>(buffer.data());

        for (uint16_t i = 0; i < bloco->cabecalho.qtd_itens; ++i) {
            if (bloco->itens[i].chave == chave_busca) {
                return bloco->itens[i].dados;
            }
        }
        return std::unexpected(ErroDisco::ForaDosLimites);
    }

    template <typename TPayload, std::size_t MaxItens>
    std::expected<void, ErroDisco> inserir_item_tipado(const ChaveBtrfs& chave, const TPayload& payload) {
        auto res_cow = clonar_bloco_cow(bloco_raiz_atual);
        if (!res_cow) return std::unexpected(res_cow.error());

        uint64_t novo_bloco = *res_cow;
        this->bloco_raiz_atual = novo_bloco;

        std::array<std::byte, TAMANHO_BLOCO> buffer;
        disco.ler_bloco(novo_bloco, buffer);
        auto* bloco = reinterpret_cast<BlocoArvoreB<TPayload, MaxItens>*>(buffer.data());

        for (uint16_t i = 0; i < bloco->cabecalho.qtd_itens; ++i) {
            if (bloco->itens[i].chave == chave) {
                bloco->itens[i].dados = payload;
                return disco.escrever_bloco(novo_bloco, buffer);
            }
        }

        if (bloco->cabecalho.qtd_itens >= MaxItens) {
            return std::unexpected(ErroDisco::ForaDosLimites);
        }

        uint16_t indice = bloco->cabecalho.qtd_itens;
        bloco->itens[indice].chave = chave;
        bloco->itens[indice].tamanho_payload = sizeof(TPayload);
        bloco->itens[indice].dados = payload;
        bloco->cabecalho.qtd_itens++;

        return disco.escrever_bloco(novo_bloco, buffer);
    }
    
    [[nodiscard]] uint64_t obter_raiz_atual() const;
    [[nodiscard]] uint64_t obter_geracao_atual() const;
};