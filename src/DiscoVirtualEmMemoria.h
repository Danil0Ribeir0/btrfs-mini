#pragma once
#include "fs/DiscoVirtual.h"
#include <vector>
#include <string>

class DiscoVirtualEmMemoria final : public IDiscoVirtual {
private:
    std::vector<std::byte> armazenamento;
    std::string arquivo_backup;
    bool esta_montado = false;
    std::size_t qtd_blocos;

public:
    explicit DiscoVirtualEmMemoria(std::size_t blocos);

    std::expected<void, ErroDisco> montar(const std::string& caminho) override;
    std::expected<void, ErroDisco> desmontar() override;

    std::expected<void, ErroDisco> ler_bloco(std::size_t indice_bloco, std::span<std::byte, TAMANHO_BLOCO> buffer) override;
    std::expected<void, ErroDisco> escrever_bloco(std::size_t indice_bloco, std::span<const std::byte, TAMANHO_BLOCO> buffer) override;

    [[nodiscard]] std::size_t total_blocos() const override;
};