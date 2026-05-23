#pragma once
#include "fs/DiscoVirtual.h"
#include "fs/Inode.h"
#include "fs/MapaDeBits.h"
#include <expected>

class GerenciadorDeInodes {
private:
    IDiscoVirtual& disco;
    MapaDeBits mapa_inodes;
    std::size_t bloco_inicio_inodes;
    std::size_t capacidade_total_inodes;

    [[nodiscard]] std::size_t calcular_bloco_fisico(std::size_t indice_inode) const;
    [[nodiscard]] std::size_t calcular_offset(std::size_t indice_inode) const;

public:
    GerenciadorDeInodes(IDiscoVirtual& disco_ref, 
                        std::size_t inicio_mapa, std::size_t blocos_mapa,
                        std::size_t inicio_inodes, std::size_t total_inodes);

    std::expected<void, ErroDisco> inicializar();

    std::expected<Inode, ErroDisco> ler_inode(std::size_t indice_inode);
    std::expected<void, ErroDisco> escrever_inode(std::size_t indice_inode, const Inode& inode);

    std::expected<std::size_t, ErroDisco> alocar_inode(TipoInode tipo);
    std::expected<void, ErroDisco> liberar_inode(std::size_t indice_inode);
};