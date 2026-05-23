#pragma once
#include "fs/DiscoVirtual.h"
#include "fs/GerenciadorDeInodes.h"
#include <expected>
#include <span>

class Arquivo {
private:
    GerenciadorDeInodes& gerenciador_inodes;
    IDiscoVirtual& disco;
    std::size_t indice_inode;
    std::size_t cursor_posicao;

public:
    Arquivo(GerenciadorDeInodes& gerenciador, IDiscoVirtual& disco_ref, std::size_t inode_id);

    std::expected<std::size_t, ErroDisco> ler(std::span<std::byte> buffer);
    std::expected<std::size_t, ErroDisco> escrever(std::span<const std::byte> buffer);
    
    std::expected<void, ErroDisco> buscar(std::size_t posicao);
    
    [[nodiscard]] std::size_t obter_posicao() const;
    [[nodiscard]] std::size_t obter_tamanho() const;
};