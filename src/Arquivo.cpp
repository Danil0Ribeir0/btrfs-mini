#include "fs/Arquivo.h"
#include "fs/Inode.h"
#include <algorithm>

constexpr std::size_t TAMANHO_CHUNK = 256;

struct ChunkDados {
    std::array<std::byte, TAMANHO_CHUNK> bytes;
    uint32_t tamanho_util;
};

Arquivo::Arquivo(GerenciadorArvoreB& arvore_ref, uint64_t id) : arvore(arvore_ref), id_inode(id) {}

std::expected<void, ErroDisco> Arquivo::escrever(std::span<const std::byte> dados) {
    std::size_t offset_leitura = 0;
    uint64_t offset_chave = 0;

    while (offset_leitura < dados.size()) {
        std::size_t tamanho_copia = std::min<std::size_t>(dados.size() - offset_leitura, TAMANHO_CHUNK);

        ChunkDados chunk{};
        chunk.tamanho_util = tamanho_copia;
        std::copy_n(dados.begin() + offset_leitura, tamanho_copia, chunk.bytes.begin());

        ChaveBtrfs chave_chunk{id_inode, BtrfsTipoItem::ExtentDados, offset_chave};

        auto res = arvore.inserir_item_tipado(chave_chunk, chunk);
        if (!res) return std::unexpected(res.error());

        offset_leitura += tamanho_copia;
        offset_chave++;
    }

    ChaveBtrfs chave_inode{id_inode, BtrfsTipoItem::InodeItem, 0};
    auto res_inode = arvore.buscar_item_tipado<Inode>(chave_inode);

    if (res_inode) {
        Inode inode_atual = *res_inode;
        inode_atual.tamanho_bytes = dados.size();
        arvore.inserir_item_tipado(chave_inode, inode_atual);
    }

    return {};
}

std::expected<std::vector<std::byte>, ErroDisco> Arquivo::ler() {
    auto res_chunks = arvore.listar_itens_tipado<ChunkDados>(id_inode, BtrfsTipoItem::ExtentDados);
    if (!res_chunks) return std::unexpected(res_chunks.error());

    std::vector<std::byte> resultado;
    
    for (const auto& chunk : *res_chunks) {
        resultado.insert(resultado.end(), chunk.bytes.begin(), chunk.bytes.begin() + chunk.tamanho_util);
    }
    
    return resultado;
}