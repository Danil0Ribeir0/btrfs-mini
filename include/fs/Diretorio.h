#pragma once
#include "fs/GerenciadorArvoreB.h"
#include "fs/Arquivo.h"
#include <string>
#include <vector>
#include <expected>

struct InfoEntrada {
    std::string nome;
    uint64_t id_inode;
    bool eh_diretorio;
};

class Diretorio {
private:
    GerenciadorArvoreB& arvore;
    uint64_t id_inode;

public:
    Diretorio(GerenciadorArvoreB& arvore_ref, uint64_t id);

    std::expected<Arquivo, ErroDisco> criar_arquivo(const std::string& nome);
    std::expected<Diretorio, ErroDisco> criar_diretorio(const std::string& nome);
    
    std::expected<std::vector<InfoEntrada>, ErroDisco> listar();

    [[nodiscard]] uint64_t obter_id() const { return id_inode; }
};