#include "DiscoVirtualEmMemoria.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

DiscoVirtualEmMemoria::DiscoVirtualEmMemoria(std::size_t blocos) : qtd_blocos(blocos) {}

std::expected<void, ErroDisco> DiscoVirtualEmMemoria::montar(const std::string& caminho) {
    if (esta_montado) return std::unexpected(ErroDisco::JaMontado);
    
    arquivo_backup = caminho;
    armazenamento.resize(qtd_blocos * TAMANHO_BLOCO);
    
    if (std::filesystem::exists(caminho)) {
        std::ifstream arquivo(caminho, std::ios::binary);
        if (!arquivo) return std::unexpected(ErroDisco::ErroIO);
        arquivo.read(reinterpret_cast<char*>(armazenamento.data()), armazenamento.size());
    } else {
        std::ranges::fill(armazenamento, std::byte{0});
    }
    
    esta_montado = true;
    return {};
}

std::expected<void, ErroDisco> DiscoVirtualEmMemoria::desmontar() {
    if (!esta_montado) return std::unexpected(ErroDisco::NaoMontado);
    
    std::ofstream arquivo(arquivo_backup, std::ios::binary | std::ios::trunc);
    if (!arquivo) return std::unexpected(ErroDisco::ErroIO);
    arquivo.write(reinterpret_cast<const char*>(armazenamento.data()), armazenamento.size());
    
    esta_montado = false;
    armazenamento.clear();
    armazenamento.shrink_to_fit();
    return {};
}

std::expected<void, ErroDisco> DiscoVirtualEmMemoria::ler_bloco(std::size_t indice_bloco, std::span<std::byte, TAMANHO_BLOCO> buffer) {
    if (!esta_montado) return std::unexpected(ErroDisco::NaoMontado);
    if (indice_bloco >= qtd_blocos) return std::unexpected(ErroDisco::ForaDosLimites);
    
    auto inicio = armazenamento.begin() + (indice_bloco * TAMANHO_BLOCO);
    std::ranges::copy(inicio, inicio + TAMANHO_BLOCO, buffer.begin());
    return {};
}

std::expected<void, ErroDisco> DiscoVirtualEmMemoria::escrever_bloco(std::size_t indice_bloco, std::span<const std::byte, TAMANHO_BLOCO> buffer) {
    if (!esta_montado) return std::unexpected(ErroDisco::NaoMontado);
    if (indice_bloco >= qtd_blocos) return std::unexpected(ErroDisco::ForaDosLimites);
    
    auto inicio = armazenamento.begin() + (indice_bloco * TAMANHO_BLOCO);
    std::ranges::copy(buffer, inicio);
    return {};
}

std::size_t DiscoVirtualEmMemoria::total_blocos() const {
    return qtd_blocos;
}