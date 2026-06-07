#include "fs/Diretorio.h"
#include "fs/Inode.h"
#include <cstring>
#include <chrono>

struct RegistroDiretorio {
    uint64_t id_inode;
    char nome[28];
    bool eh_diretorio;
};

Diretorio::Diretorio(GerenciadorArvoreB& arvore_ref, uint64_t id, std::function<uint64_t()> gerador)
    : arvore(arvore_ref), id_inode(id), gerador_id(std::move(gerador)) {}

std::expected<uint64_t, ErroDisco> Diretorio::criar_entrada_interna(const std::string& nome, TipoInode tipo, bool eh_diretorio) {
    uint64_t novo_id = gerador_id();

    Inode inode_novo{};
    inode_novo.tipo = tipo;
    inode_novo.permissoes = 255;
    inode_novo.tamanho_bytes = 0;
    inode_novo.data_criacao = std::chrono::system_clock::now().time_since_epoch().count();
    inode_novo.data_modificacao = inode_novo.data_criacao;

    ChaveBtrfs chave_inode{novo_id, BtrfsTipoItem::InodeItem, 0};
    auto res_inode = arvore.inserir_item_tipado(chave_inode, inode_novo);
    if (!res_inode) return std::unexpected(res_inode.error());

    RegistroDiretorio registro{};
    registro.id_inode = novo_id;
    registro.eh_diretorio = eh_diretorio;
    std::strncpy(registro.nome, nome.c_str(), 27);
    registro.nome[27] = '\0';

    ChaveBtrfs chave_dir{this->id_inode, BtrfsTipoItem::ItemDiretorio, novo_id};
    auto res_dir = arvore.inserir_item_tipado(chave_dir, registro);
    if (!res_dir) return std::unexpected(res_dir.error());

    return novo_id;
}

std::expected<Arquivo, ErroDisco> Diretorio::criar_arquivo(const std::string& nome) {
    auto res = criar_entrada_interna(nome, TipoInode::Arquivo, false);
    if (!res) return std::unexpected(res.error());
    return Arquivo(arvore, *res);
}

std::expected<Diretorio, ErroDisco> Diretorio::criar_diretorio(const std::string& nome) {
    auto res = criar_entrada_interna(nome, TipoInode::Diretorio, true);
    if (!res) return std::unexpected(res.error());
    return Diretorio(arvore, *res, gerador_id);
}

std::expected<std::vector<InfoEntrada>, ErroDisco> Diretorio::listar() {
    auto res_itens = arvore.listar_itens_tipado<RegistroDiretorio>(this->id_inode, BtrfsTipoItem::ItemDiretorio);
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

std::expected<Arquivo, ErroDisco> Diretorio::abrir_arquivo(const std::string& nome) {
    auto res_itens = listar();
    if (!res_itens) return std::unexpected(res_itens.error());

    for (const auto& item : *res_itens) {
        if (!item.eh_diretorio && item.nome == nome) {
            return Arquivo(arvore, item.id_inode);
        }
    }
    return std::unexpected(ErroDisco::ForaDosLimites);
}

std::expected<Diretorio, ErroDisco> Diretorio::navegar_para(const std::string& caminho) {
    if (caminho.empty()) return *this;

    size_t pos = 0;
    uint64_t id_atual = this->id_inode;

    while (pos < caminho.size()) {
        size_t proxima_barra = caminho.find_first_of("/\\", pos);
        if (proxima_barra == std::string::npos) {
            proxima_barra = caminho.size();
        }

        std::string componente = caminho.substr(pos, proxima_barra - pos);
        if (!componente.empty() && componente != ".") {
            auto res_itens = arvore.listar_itens_tipado<RegistroDiretorio>(id_atual, BtrfsTipoItem::ItemDiretorio);
            if (!res_itens) return std::unexpected(res_itens.error());

            bool encontrado = false;
            for (const auto& reg : *res_itens) {
                if (reg.eh_diretorio && std::string(reg.nome) == componente) {
                    id_atual = reg.id_inode;
                    encontrado = true;
                    break;
                }
            }

            if (!encontrado) {
                return std::unexpected(ErroDisco::ForaDosLimites);
            }
        }

        pos = proxima_barra + 1;
    }

    return Diretorio(arvore, id_atual, gerador_id);
}

std::expected<Arquivo, ErroDisco> Diretorio::criar_arquivo_em_caminho(const std::string& caminho) {
    size_t ultima_barra = caminho.find_last_of("/\\");
    
    if (ultima_barra == std::string::npos) {
        // Sem caminho, cria na raiz
        return criar_arquivo(caminho);
    }

    std::string caminho_dir = caminho.substr(0, ultima_barra);
    std::string nome_arquivo = caminho.substr(ultima_barra + 1);

    auto res_dir = navegar_para(caminho_dir);
    if (!res_dir) return std::unexpected(res_dir.error());

    return res_dir->criar_arquivo(nome_arquivo);
}

std::expected<Arquivo, ErroDisco> Diretorio::abrir_arquivo_em_caminho(const std::string& caminho) {
    size_t ultima_barra = caminho.find_last_of("/\\");
    
    if (ultima_barra == std::string::npos) {
        // Sem caminho, abre na raiz
        return abrir_arquivo(caminho);
    }

    std::string caminho_dir = caminho.substr(0, ultima_barra);
    std::string nome_arquivo = caminho.substr(ultima_barra + 1);

    auto res_dir = navegar_para(caminho_dir);
    if (!res_dir) return std::unexpected(res_dir.error());

    return res_dir->abrir_arquivo(nome_arquivo);
}

std::expected<Diretorio, ErroDisco> Diretorio::criar_diretorio_em_caminho(const std::string& caminho) {
    std::string path = caminho;
    while (!path.empty() && (path.back() == '/' || path.back() == '\\')) path.pop_back();

    size_t ultima_barra = path.find_last_of("/\\");

    if (ultima_barra == std::string::npos) {
        return criar_diretorio(path);
    }

    std::string caminho_dir = path.substr(0, ultima_barra);
    std::string nome_dir = path.substr(ultima_barra + 1);

    auto res_dir = navegar_para(caminho_dir);
    if (!res_dir) return std::unexpected(res_dir.error());

    return res_dir->criar_diretorio(nome_dir);
}