#pragma once
#include "fs/GerenciadorArvoreB.h"
#include <span>
#include <vector>
#include <expected>

class Arquivo {
private:
    GerenciadorArvoreB& arvore;
    uint64_t id_inode;

public:
    Arquivo(GerenciadorArvoreB& arvore_ref, uint64_t id);

    std::expected<void, ErroDisco> escrever(std::span<const std::byte> dados);
    
    std::expected<std::vector<std::byte>, ErroDisco> ler();

    [[nodiscard]] uint64_t obter_id() const { return id_inode; }
};