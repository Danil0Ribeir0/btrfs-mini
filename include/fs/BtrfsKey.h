#pragma once
#include <cstdint>
#include <compare>

enum class BtrfsTipoItem : uint8_t {
    InodeItem = 1,
    RefDiretorio = 2,
    ItemDiretorio = 3,
    ExtentDados = 4
};

struct ChaveBtrfs {
    uint64_t id_objeto;
    BtrfsTipoItem tipo;
    uint64_t offset;

    auto operator<=>(const ChaveBtrfs&) const = default;
};

static_assert(sizeof(ChaveBtrfs) == 24 || sizeof(ChaveBtrfs) <= 32, "A chave do BTRFS deve ser compacta e alinhada na memória.");