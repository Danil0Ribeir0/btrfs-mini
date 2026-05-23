#pragma once
#include <cstddef>
#include <span>
#include <expected>
#include <string>

constexpr std::size_t TAMANHO_BLOCO = 4096;

enum class ErroDisco {
    ForaDosLimites,
    ErroIO,
    NaoMontado,
    JaMontado
};

class IDiscoVirtual {
public:
    virtual ~IDiscoVirtual() = default;

    virtual std::expected<void, ErroDisco> montar(const std::string& caminho) = 0;
    virtual std::expected<void, ErroDisco> desmontar() = 0;

    virtual std::expected<void, ErroDisco> ler_bloco(std::size_t indice_bloco, std::span<std::byte, TAMANHO_BLOCO> buffer) = 0;
    virtual std::expected<void, ErroDisco> escrever_bloco(std::size_t indice_bloco, std::span<const std::byte, TAMANHO_BLOCO> buffer) = 0;

    [[nodiscard]] virtual std::size_t total_blocos() const = 0;
};