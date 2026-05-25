#pragma once
#include "fs/DiscoVirtual.h"
#include "fs/MapaDeBits.h"
#include "fs/BtrfsBloco.h"
#include <expected>
#include <vector>
#include <span>
#include <cstring>

class GerenciadorArvoreB {
private:
    IDiscoVirtual& disco;
    MapaDeBits& mapa_blocos;
    uint64_t bloco_raiz_atual;
    uint64_t geracao_atual;

public:
    GerenciadorArvoreB(IDiscoVirtual& disco_ref, MapaDeBits& mapa, uint64_t raiz, uint64_t geracao);

    [[nodiscard]] std::expected<uint64_t, ErroDisco> clonar_bloco_cow(uint64_t bloco_origem) const;

    template <typename TPayload>
    std::expected<TPayload, ErroDisco> buscar_item_tipado(const ChaveBtrfs& chave_busca) {
        std::array<std::byte, TAMANHO_BLOCO> buffer{};
        auto res = disco.ler_bloco(bloco_raiz_atual, buffer);
        if (!res) return std::unexpected(res.error());

        auto* bloco = reinterpret_cast<BlocoArvoreB*>(buffer.data());

        for (uint16_t i = 0; i < bloco->cabecalho.qtd_itens; ++i) {
            if (bloco->itens[i].chave == chave_busca) {
                TPayload payload_recuperado;
                std::memcpy(&payload_recuperado, bloco->itens[i].dados.data(), sizeof(TPayload));
                return payload_recuperado;
            }
        }
        return std::unexpected(ErroDisco::ForaDosLimites);
    }

    template <typename TPayload>
    std::expected<std::vector<TPayload>, ErroDisco> listar_itens_tipado(uint64_t id_objeto, BtrfsTipoItem tipo) {
        std::array<std::byte, TAMANHO_BLOCO> buffer{};
        auto res = disco.ler_bloco(bloco_raiz_atual, buffer);
        if (!res) return std::unexpected(res.error());

        auto* bloco = reinterpret_cast<BlocoArvoreB*>(buffer.data());
        std::vector<TPayload> resultados;

        for (uint16_t i = 0; i < bloco->cabecalho.qtd_itens; ++i) {
            if (bloco->itens[i].chave.id_objeto == id_objeto && bloco->itens[i].chave.tipo == tipo) {
                TPayload payload_recuperado;
                std::memcpy(&payload_recuperado, bloco->itens[i].dados.data(), sizeof(TPayload));
                resultados.push_back(payload_recuperado);
            }
        }
        return resultados;
    }

    template <typename TPayload>
    std::expected<std::vector<std::pair<ChaveBtrfs, TPayload>>, ErroDisco> listar_itens_com_chave(uint64_t id_objeto, BtrfsTipoItem tipo) {
        std::array<std::byte, TAMANHO_BLOCO> buffer{};
        auto res = disco.ler_bloco(bloco_raiz_atual, buffer);
        if (!res) return std::unexpected(res.error());

        auto* bloco = reinterpret_cast<BlocoArvoreB*>(buffer.data());
        std::vector<std::pair<ChaveBtrfs, TPayload>> resultados;

        for (uint16_t i = 0; i < bloco->cabecalho.qtd_itens; ++i) {
            if (bloco->itens[i].chave.id_objeto == id_objeto && bloco->itens[i].chave.tipo == tipo) {
                TPayload payload_recuperado;
                std::memcpy(&payload_recuperado, bloco->itens[i].dados.data(), sizeof(TPayload));
                resultados.emplace_back(bloco->itens[i].chave, payload_recuperado);
            }
        }
        return resultados;
    }

    template <typename TPayload>
    std::expected<void, ErroDisco> inserir_item_tipado(const ChaveBtrfs& chave, const TPayload& payload) {
        static_assert(sizeof(TPayload) <= TAMANHO_PAYLOAD_MAX, "Erro: Payload muito grande para o Slot!");

        auto res_cow = clonar_bloco_cow(bloco_raiz_atual);
        if (!res_cow) return std::unexpected(res_cow.error());

        uint64_t novo_bloco = *res_cow;
        this->bloco_raiz_atual = novo_bloco;

        std::array<std::byte, TAMANHO_BLOCO> buffer{};
        if (auto res_leitura = disco.ler_bloco(novo_bloco, buffer); !res_leitura)
            return std::unexpected(res_leitura.error());
        auto* bloco = reinterpret_cast<BlocoArvoreB*>(buffer.data());

        for (uint16_t i = 0; i < bloco->cabecalho.qtd_itens; ++i) {
            if (bloco->itens[i].chave == chave) {
                std::memcpy(bloco->itens[i].dados.data(), &payload, sizeof(TPayload));
                bloco->itens[i].tamanho_payload = sizeof(TPayload);
                return disco.escrever_bloco(novo_bloco, buffer);
            }
        }

        if (bloco->cabecalho.qtd_itens >= MAX_ITENS_BLOCO) {
            return std::unexpected(ErroDisco::ForaDosLimites);
        }

        uint16_t indice = bloco->cabecalho.qtd_itens;
        bloco->itens[indice].chave = chave;
        bloco->itens[indice].tamanho_payload = sizeof(TPayload);
        std::memcpy(bloco->itens[indice].dados.data(), &payload, sizeof(TPayload));
        bloco->cabecalho.qtd_itens++;

        return disco.escrever_bloco(novo_bloco, buffer);
    }
    
    [[nodiscard]] uint64_t obter_raiz_atual() const;
    [[nodiscard]] uint64_t obter_geracao_atual() const;
};