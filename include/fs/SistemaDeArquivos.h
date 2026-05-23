#pragma once
#include "fs/DiscoVirtual.h"
#include "fs/MapaDeBits.h"
#include "fs/GerenciadorArvoreB.h"
#include "fs/Diretorio.h"
#include <memory>
#include <expected>

class SistemaDeArquivos {
private:
    IDiscoVirtual& disco;
    std::unique_ptr<MapaDeBits> mapa;
    std::unique_ptr<GerenciadorArvoreB> arvore;
    
    uint64_t proximo_id_objeto = 256;

    [[nodiscard]] uint64_t gerar_novo_id();

public:
    explicit SistemaDeArquivos(IDiscoVirtual& disco_ref);

    std::expected<void, ErroDisco> formatar();
    
    std::expected<void, ErroDisco> montar();
    
    std::expected<Diretorio, ErroDisco> obter_raiz();
};