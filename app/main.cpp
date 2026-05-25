#include "../src/DiscoVirtualEmMemoria.h"
#include "fs/SistemaDeArquivos.h"
#include <iostream>
#include <string>
#include <sstream>
#include <span>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

void exibir_diretorio(Diretorio& dir) {
    auto itens = dir.listar();
    if (!itens || itens->empty()) {
        std::cout << "  (Pasta vazia)\n";
        return;
    }
    for (const auto&[nome, id_inode, eh_diretorio] : *itens) {
        std::cout << "  " << (eh_diretorio ? "[DIR] " : "[FIL] ")
                  << nome << " (ID: " << id_inode << ")\n";
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "==================================================\n";
    std::cout << "          MINI-BTRFS SHELL INTERATIVO             \n";
    std::cout << "==================================================\n";

    constexpr std::size_t TOTAL_BLOCOS = 64;
    const std::string ARQUIVO_DISCO = "btrfs_disco.bin";
    DiscoVirtualEmMemoria disco(TOTAL_BLOCOS);

    std::cout << "Selecione o modo de inicialização:\n";
    std::cout << "1. Formatar um NOVO disco (Apaga tudo)\n";
    std::cout << "2. Montar disco EXISTENTE (Carrega de btrfs_disco.bin)\n";
    std::cout << "Escolha (1/2): ";

    std::string escolha;
    std::getline(std::cin, escolha);

    if (!disco.montar(ARQUIVO_DISCO)) {
        std::cerr << "Erro ao montar o disco virtual (Falha de I/O ou arquivo bloqueado).\n";
        return 1;
    }

    SistemaDeArquivos fs(disco);

    if (escolha == "1") {
        std::cout << "[FS] Formatando disco via Copy-on-Write...\n";
        if (!fs.formatar()) {
            std::cerr << "Erro durante formatação do filesystem.\n";
            if (!disco.desmontar()) {}
            return 1;
        }
    } else {
        std::cout << "[FS] Montando sistema de arquivos...\n";
        if (!fs.montar()) {
            std::cerr << "Disco existe mas não é um BTRFS válido (ou está corrompido).\n";
            if (!disco.desmontar()) {}
            return 1;
        }
    }

    auto raiz_opt = fs.obter_raiz();
    if (!raiz_opt) {
        std::cerr << "Falha ao obter a pasta raiz do sistema.\n";
        if (!disco.desmontar()) {}
        return 1;
    }
    Diretorio raiz = *raiz_opt;

    std::cout << "\nBem-vindo ao shell BTRFS! Digite 'help' para comandos.\n";

    // O Loop Principal de Interação
    while (true) {
        std::cout << "\nbtrfs:/> ";
        std::string linha;
        if (!std::getline(std::cin, linha)) break;

        if (linha.empty()) continue;

        std::istringstream iss(linha);
        std::string comando, argumento;
        iss >> comando >> argumento;

        if (comando == "exit" || comando == "sair") {
            break;
        }
        else if (comando == "help") {
            std::cout << "Comandos disponiveis:\n"
                      << "  ls           - Lista os arquivos na raiz\n"
                      << "  mkdir <nome> - Cria uma subpasta\n"
                      << "  touch <nome> - Cria um arquivo e permite escrever nele\n"
                      << "  cat <nome>   - Le e exibe o conteudo de um arquivo\n"
                      << "  exit         - Salva o disco e encerra\n";
        }
        else if (comando == "ls") {
            exibir_diretorio(raiz);
        }
        else if (comando == "mkdir") {
            if (argumento.empty()) { std::cout << "Uso: mkdir <nome>\n"; continue; }
            if (auto nova_pasta = raiz.criar_diretorio(argumento)) std::cout << "Diretorio '" << argumento << "' criado com sucesso.\n";
            else std::cout << "Erro ao criar diretorio.\n";
        }
        else if (comando == "touch") {
            if (argumento.empty()) { std::cout << "Uso: touch <nome>\n"; continue; }
            auto novo_arquivo = raiz.criar_arquivo(argumento);
            if (!novo_arquivo) { std::cout << "Erro ao criar arquivo.\n"; continue; }

            std::cout << "Arquivo criado! Digite o texto que deseja salvar nele:\n> ";
            std::string conteudo;
            std::getline(std::cin, conteudo);

            auto span_dados = std::as_bytes(std::span(conteudo.data(), conteudo.size()));
            if (novo_arquivo->escrever(span_dados)) {
                std::cout << "[BTRFS] Dados gravados em Inline Extents usando CoW!\n";
            } else {
                std::cout << "Erro ao gravar dados no disco.\n";
            }
        }
        else if (comando == "cat") {
            if (argumento.empty()) { std::cout << "Uso: cat <nome>\n"; continue; }
            auto arquivo = raiz.abrir_arquivo(argumento);
            if (!arquivo) { std::cout << "Arquivo '" << argumento << "' nao encontrado!\n"; continue; }

            auto bytes = arquivo->ler();
            if (bytes) {
                std::string texto(reinterpret_cast<const char*>(bytes->data()), bytes->size());
                std::cout << "--------------------------------\n";
                std::cout << texto << "\n";
                std::cout << "--------------------------------\n";
            } else {
                std::cout << "Erro ao ler o arquivo.\n";
            }
        }
        else {
            std::cout << "Comando desconhecido: " << comando << ". Digite 'help'.\n";
        }
    }

    std::cout << "\n[Hardware] Desmontando sistema e efetuando dump (flush) da RAM...\n";
    if (!disco.desmontar()) {}
    std::cout << "Sessão encerrada com segurança.\n";
    return 0;
}