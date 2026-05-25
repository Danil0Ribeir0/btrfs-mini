#pragma once
#include "fs/GerenciadorArvoreB.h"
#include "fs/Arquivo.h"
#include <string>
#include <vector>
#include <expected>
#include <functional>

enum class TipoInode : uint8_t;

struct InfoEntrada {
    std::string nome;
    uint64_t id_inode;
    bool eh_diretorio;
};

class Diretorio {
private:
    GerenciadorArvoreB& arvore;
    uint64_t id_inode;
    std::function<uint64_t()> gerador_id;
    Diretorio(GerenciadorArvoreB &arvore_ref, uint64_t id);

    std::expected<uint64_t, ErroDisco> criar_entrada_interna(const std::string& nome, TipoInode tipo, bool eh_diretorio) const;

public:
    Diretorio(GerenciadorArvoreB& arvore_ref, uint64_t id, std::function<uint64_t()> gerador);

    std::expected<Arquivo, ErroDisco> criar_arquivo(const std::string& nome) const;
    std::expected<Diretorio, ErroDisco> criar_diretorio(const std::string& nome) const;
    std::expected<Arquivo, ErroDisco> abrir_arquivo(const std::string& nome) const;
    std::expected<std::vector<InfoEntrada>, ErroDisco> listar() const;

    [[nodiscard]] uint64_t obter_id() const { return id_inode; }
};