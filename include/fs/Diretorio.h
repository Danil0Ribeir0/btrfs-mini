#pragma once
#include "fs/GerenciadorArvoreB.h"
#include "fs/Arquivo.h"
#include "fs/Inode.h"
#include <string>
#include <vector>
#include <expected>
#include <functional>

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

    std::expected<uint64_t, ErroDisco> criar_entrada_interna(const std::string& nome, TipoInode tipo, bool eh_diretorio);

public:
    Diretorio(GerenciadorArvoreB& arvore_ref, uint64_t id, std::function<uint64_t()> gerador);

    std::expected<Arquivo, ErroDisco> criar_arquivo(const std::string& nome);
    std::expected<Diretorio, ErroDisco> criar_diretorio(const std::string& nome);
    std::expected<Arquivo, ErroDisco> abrir_arquivo(const std::string& nome);
    std::expected<std::vector<InfoEntrada>, ErroDisco> listar();
    
    std::expected<Diretorio, ErroDisco> navegar_para(const std::string& caminho);
    std::expected<Arquivo, ErroDisco> criar_arquivo_em_caminho(const std::string& caminho);
    std::expected<Arquivo, ErroDisco> abrir_arquivo_em_caminho(const std::string& caminho);

    std::expected<Diretorio, ErroDisco> criar_diretorio_em_caminho(const std::string& caminho);

    [[nodiscard]] uint64_t obter_id() const { return id_inode; }
};