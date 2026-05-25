#include "../src/DiscoVirtualEmMemoria.h"
#include "fs/SistemaDeArquivos.h"
#include <iostream>
#include <string>
#include <span>
#include <vector>

void exibir_diretorio(Diretorio& dir, const std::string& caminho_pasta) {
    std::cout << "\n[LS] Listando a pasta: " << caminho_pasta << "\n";
    auto itens = dir.listar();
    if (!itens) {
        std::cerr << "Erro ao listar diretorio.\n";
        return;
    }

    if (itens->empty()) {
        std::cout << "  (Pasta vazia)\n";
        return;
    }

    for (const auto& item : *itens) {
        std::cout << "  " << (item.eh_diretorio ? "[DIR] " : "[FIL] ")
                  << item.nome << " (ID Inode: " << item.id_inode << ")\n";
    }
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "         MINI-BTRFS: SISTEMA DE ARQUIVOS          \n";
    std::cout << "==================================================\n";

    const std::size_t TOTAL_BLOCOS = 64;
    const std::string ARQUIVO_DISCO = "btrfs_disco.bin";

    std::cout << "[Hardware] Inicializando disco em RAM com " << TOTAL_BLOCOS << " blocos...\n";
    DiscoVirtualEmMemoria disco(TOTAL_BLOCOS);

    std::cout << "[Hardware] Montando arquivo físico de backup: " << ARQUIVO_DISCO << "\n";
    if (auto res = disco.montar(ARQUIVO_DISCO); !res) {
        std::cerr << "Falha crítica ao montar o disco virtual.\n";
        return 1;
    }

    SistemaDeArquivos fs(disco);

    std::cout << "[FS] Formatando o disco virtual com o layout BTRFS (Nível Único + CoW)...\n";
    if (auto res = fs.formatar(); !res) {
        std::cerr << "Falha ao formatar o sistema de arquivos.\n";
        disco.desmontar();
        return 1;
    }

    std::cout << "[FS] Obtendo acesso ao diretório raiz '/'...\n";
    auto raiz_opt = fs.obter_raiz();
    if (!raiz_opt) {
        std::cerr << "Não foi possível ler a raiz.\n";
        disco.desmontar();
        return 1;
    }
    Diretorio raiz = *raiz_opt;

    exibir_diretorio(raiz, "/");

    std::cout << "\n[Operação] Criando a pasta '/documentos'...\n";
    auto pasta_docs_opt = raiz.criar_diretorio("documentos");
    auto pasta_docs = raiz.criar_diretorio("documentos");
    if (!pasta_docs) {
        std::cerr << "Erro ao criar diretório.\n";
        disco.desmontar();
        return 1;
    }

    std::cout << "[Operação] Criando o arquivo 'relatorio.txt' dentro de '/documentos'...\n";
    auto arquivo_relatorio = pasta_docs->criar_arquivo("relatorio.txt");
    if (!arquivo_relatorio) {
        std::cerr << "Erro ao criar arquivo.\n";
        disco.desmontar();
        return 1;
    }

    std::string mensagem_usuario = "Ola Banca! Este arquivo foi fatiado em Inline Chunks e salvo via Copy-on-Write (CoW).";
    std::cout << "[I/O Escrita] Gravando dados no arquivo (Tamanho: " << mensagem_usuario.size() << " bytes)...\n";

    auto span_dados = std::as_bytes(std::span(mensagem_usuario.data(), mensagem_usuario.size()));
    if (auto res = arquivo_relatorio->escrever(span_dados); !res) {
        std::cerr << "Erro ao escrever dados no arquivo.\n";
        disco.desmontar();
        return 1;
    }
    std::cout << "[BTRFS] Sucesso! O motor clonou o nó raiz antigo e gravou a nova transação com segurança.\n";

    exibir_diretorio(raiz, "/");
    exibir_diretorio(*pasta_docs, "/documentos");

    std::cout << "\n[I/O Leitura] Lendo o conteúdo de 'relatorio.txt' direto do disco virtual...\n";
    auto bytes_lidos = arquivo_relatorio->ler();
    if (!bytes_lidos) {
        std::cerr << "Erro ao ler os dados do arquivo.\n";
        disco.desmontar();
        return 1;
    }

    std::string conteudo_final(reinterpret_cast<const char*>(bytes_lidos->data()), bytes_lidos->size());
    std::cout << "--------------------------------------------------\n";
    std::cout << "CONTEÚDO LIDO: \"" << conteudo_final << "\"\n";
    std::cout << "--------------------------------------------------\n";

    std::cout << "\n[Hardware] Desmontando o sistema e salvando dados em '" << ARQUIVO_DISCO << "'...\n";
    disco.desmontar();

    std::cout << "==================================================\n";
    std::cout << "      DEMONSTRAÇÃO CONCLUÍDA COM SUCESSO!        \n";
    std::cout << "==================================================\n";
    return 0;
}