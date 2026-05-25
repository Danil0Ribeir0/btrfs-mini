#include "fs/Diretorio.h"
#include "fs/Inode.h"
#include <cstring>
#include <chrono>

struct RegistroDiretorio {
    uint64_t id_inode;
    char nome[28];
    bool eh_diretorio;
};

static uint64_t gerador_id_global = 1000;

Diretorio::Diretorio(GerenciadorArvoreB& arvore_ref, uint64_t id) : arvore(arvore_ref), id_inode(id) {}

std::expected<Arquivo, ErroDisco> Diretorio::criar_arquivo(const std::string& nome) {
    uint64_t novo_id = ++gerador_id_global;

    Inode inode_arquivo{};
    inode_arquivo.tipo = TipoInode::Arquivo;
    inode_arquivo.permissoes = 255;
    inode_arquivo.tamanho_bytes = 0;
    inode_arquivo.data_criacao = std::chrono::system_clock::now().time_since_epoch().count();
    inode_arquivo.data_modificacao = inode_arquivo.data_criacao;

    ChaveBtrfs chave_inode{novo_id, BtrfsTipoItem::InodeItem, 0};
    auto res_inode = arvore.inserir_item_tipado(chave_inode, inode_arquivo);
    if (!res_inode) return std::unexpected(res_inode.error());

    RegistroDiretorio registro{};
    registro.id_inode = novo_id;
    registro.eh_diretorio = false;
    std::strncpy(registro.nome, nome.c_str(), 27);
    registro.nome[27] = '\0'; // Garante o terminador nulo

    ChaveBtrfs chave_dir{this->id_inode, BtrfsTipoItem::ItemDiretorio, novo_id};
    auto res_dir = arvore.inserir_item_tipado(chave_dir, registro);;
    if (!res_dir) return std::unexpected(res_dir.error());

    return Arquivo(arvore, novo_id);
}

std::expected<Diretorio, ErroDisco> Diretorio::criar_diretorio(const std::string& nome) {
    uint64_t novo_id = ++gerador_id_global;

    Inode inode_dir{};
    inode_dir.tipo = TipoInode::Diretorio;
    inode_dir.permissoes = 255;
    inode_dir.tamanho_bytes = 0;
    inode_dir.data_criacao = std::chrono::system_clock::now().time_since_epoch().count();
    inode_dir.data_modificacao = inode_dir.data_criacao;

    ChaveBtrfs chave_inode{novo_id, BtrfsTipoItem::InodeItem, 0};
    // CORRIGIDO: Passando inode_dir em vez de inode_arquivo
    auto res_inode = arvore.inserir_item_tipado(chave_inode, inode_dir);
    if (!res_inode) return std::unexpected(res_inode.error());

    RegistroDiretorio registro{};
    registro.id_inode = novo_id;
    registro.eh_diretorio = true;
    std::strncpy(registro.nome, nome.c_str(), 27);
    registro.nome[27] = '\0';

    ChaveBtrfs chave_dir{this->id_inode, BtrfsTipoItem::ItemDiretorio, novo_id};
    auto res_dir = arvore.inserir_item_tipado(chave_dir, registro);
    if (!res_dir) return std::unexpected(res_dir.error());

    return Diretorio(arvore, novo_id);
}

std::expected<std::vector<InfoEntrada>, ErroDisco> Diretorio::listar() {
    auto res_itens = arvore.listar_itens_tipado<RegistroDiretorio>(this->id_inode, BtrfsTipoItem::ItemDiretorio);;
    if (!res_itens) return std::unexpected(res_itens.error());

    std::vector<InfoEntrada> resultado;
    for (const auto& reg : *res_itens) {
        resultado.push_back(InfoEntrada{
            std::string(reg.nome),
            reg.id_inode,
            reg.eh_diretorio
        });
    }
    return resultado;
}