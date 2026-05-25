#include "fs/SistemaDeArquivos.h"
#include "fs/Superbloco.h"
#include <cstring>
#include <array>

constexpr uint32_t BTRFS_MAGIC = 0x4D494E49;
constexpr uint64_t ID_DIRETORIO_RAIZ = 256;

SistemaDeArquivos::SistemaDeArquivos(IDiscoVirtual& disco_ref) : disco(disco_ref) {}

uint64_t SistemaDeArquivos::gerar_novo_id() {
    return proximo_id_objeto++;
}

std::expected<void, ErroDisco> SistemaDeArquivos::formatar() {
    std::size_t total_blocos = disco.total_blocos();
    std::size_t bytes_mapa = total_blocos / 8;
    std::size_t blocos_mapa = (bytes_mapa / TAMANHO_BLOCO) + (bytes_mapa % TAMANHO_BLOCO != 0 ? 1 : 0);

    uint32_t inicio_mapa = 1;
    uint64_t bloco_raiz_arvore = inicio_mapa + blocos_mapa;

    mapa = std::make_unique<MapaDeBits>(disco, inicio_mapa, blocos_mapa, total_blocos);
    for (std::size_t i = 0; i <= bloco_raiz_arvore; ++i) {
        (void)mapa->alocar_bloco_livre();
    }

    arvore = std::make_unique<GerenciadorArvoreB>(disco, *mapa, bloco_raiz_arvore, 1);

    Superbloco sb{};
    sb.numero_magico = BTRFS_MAGIC;
    sb.tamanho_bloco = TAMANHO_BLOCO;
    sb.total_blocos = total_blocos;
    sb.inicio_mapa_bits = inicio_mapa;
    sb.blocos_mapa_bits = blocos_mapa;
    sb.raiz_arvore_fs = bloco_raiz_arvore;
    sb.geracao_atual = 1;

    std::array<std::byte, TAMANHO_BLOCO> buffer{};
    std::memcpy(buffer.data(), &sb, sizeof(Superbloco));
    
    auto res_escrita = disco.escrever_bloco(0, buffer);
    if (!res_escrita) return std::unexpected(res_escrita.error());

    return mapa->sincronizar();
}

std::expected<void, ErroDisco> SistemaDeArquivos::montar() {
    std::array<std::byte, TAMANHO_BLOCO> buffer{};
    auto res_leitura = disco.ler_bloco(0, buffer);
    if (!res_leitura) return std::unexpected(res_leitura.error());

    auto* sb = reinterpret_cast<Superbloco*>(buffer.data());
    if (sb->numero_magico != BTRFS_MAGIC) {
        return std::unexpected(ErroDisco::ErroIO);
    }

    mapa = std::make_unique<MapaDeBits>(disco, sb->inicio_mapa_bits, sb->blocos_mapa_bits, sb->total_blocos);
    auto res_mapa = mapa->carregar();
    if (!res_mapa) return std::unexpected(res_mapa.error());

    arvore = std::make_unique<GerenciadorArvoreB>(disco, *mapa, sb->raiz_arvore_fs, sb->geracao_atual);

    return {};
}

std::expected<Diretorio, ErroDisco> SistemaDeArquivos::obter_raiz() {
    if (!arvore) return std::unexpected(ErroDisco::NaoMontado);
    return Diretorio(*arvore, ID_DIRETORIO_RAIZ);
}