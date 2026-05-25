#include "fs/GerenciadorArvoreB.h"

GerenciadorArvoreB::GerenciadorArvoreB(IDiscoVirtual& disco_ref, MapaDeBits& mapa, uint64_t raiz, uint64_t geracao)
    : disco(disco_ref), mapa_blocos(mapa), bloco_raiz_atual(raiz), geracao_atual(geracao) {}

std::expected<uint64_t, ErroDisco> GerenciadorArvoreB::clonar_bloco_cow(const uint64_t bloco_origem) const {
    auto novo_bloco_opt = mapa_blocos.alocar_bloco_livre();
    if (!novo_bloco_opt) {
        return std::unexpected(ErroDisco::ForaDosLimites);
    }
    uint64_t novo_bloco = *novo_bloco_opt;

    std::array<std::byte, TAMANHO_BLOCO> buffer{};
    auto res_leitura = disco.ler_bloco(bloco_origem, buffer);
    if (!res_leitura) return std::unexpected(res_leitura.error());

    auto* cabecalho = reinterpret_cast<CabecalhoNoBtrfs*>(buffer.data());
    cabecalho->id_bloco_fisico = novo_bloco;
    cabecalho->id_geracao = this->geracao_atual;

    auto res_escrita = disco.escrever_bloco(novo_bloco, buffer);
    if (!res_escrita) return std::unexpected(res_escrita.error());

    return novo_bloco;
}

uint64_t GerenciadorArvoreB::obter_raiz_atual() const { return bloco_raiz_atual; }
uint64_t GerenciadorArvoreB::obter_geracao_atual() const { return geracao_atual; }