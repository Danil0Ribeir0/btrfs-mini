# Mini BTRFS - Sistema de Arquivos Funcional ✅

Um sistema de arquivos em miniatura inspirado na arquitetura **BTRFS**, desenvolvido como projeto acadêmico para a disciplina de Sistemas Operacionais.

O projeto implementa conceitos avançados de sistemas de arquivos modernos:
- **Árvores B** para organização hierárquica de metadados
- **Copy-on-Write (CoW)** para consistência transacional
- **Extents** para alocação eficiente de espaço
- **Disco virtual em RAM** com persistência em arquivo binário

---

## ✨ Funcionalidades Implementadas

### 1. **Camada Física**
- ✅ **Disco Virtual** (`DiscoVirtualEmMemoria`): Simulação de disco em RAM com blocos de 4KB
- ✅ **Persistência**: Gravação automática em arquivo `btrfs_disco.bin` ao desmontar
- ✅ **Superbloco**: Metadados estáticos no Bloco 0 com parâmetros do filesystem
- ✅ **Mapa de Bits**: Gerenciamento otimizado de blocos livres (alocação bit a bit)

### 2. **Motor BTRFS (Árvore B + CoW)**
- ✅ **Chave BTRFS**: Estrutura (ID Objeto + Tipo + Offset) para organização de dados
- ✅ **Gerenciador Árvore B**: Implementação completa de árvore balanceada
- ✅ **Copy-on-Write**: Garante que dados não sejam sobrescritos antes de transação segura
- ✅ **Clonagem de Blocos**: Suporte a `clonar_bloco_cow` para operações transacionais

### 3. **API de Alto Nível**
- ✅ **Sistema de Arquivos** (`SistemaDeArquivos`): Orquestra formatação, montagem e sincronização
- ✅ **Diretórios** (`Diretorio`): Criação, listagem e navegação de pastas
- ✅ **Arquivos** (`Arquivo`): Escrita e leitura com suporte a Inline Extents
- ✅ **Persistência Automática**: Sincronização de dados com o disco

### 4. **Shell Interativo**
- ✅ **Interface CLI**: Shell funcional para interação com o filesystem
- ✅ Suporte a múltiplos comandos:
  - `ls [caminho]` - Lista arquivos e pastas
  - `mkdir <nome>` - Cria diretório
  - `touch <caminho>` - Cria arquivo
  - `cat <caminho>` - Lê conteúdo de arquivo
  - `sync` - Sincroniza dados com o disco
  - `exit` - Salva e encerra a sessão

---

## 🏗️ Arquitetura

```
app/
├── main.cpp                    # Shell interativo BTRFS
│
src/
├── DiscoVirtualEmMemoria.cpp   # Implementação do disco virtual
├── GerenciadorArvoreB.cpp      # Motor de Árvore B com CoW
├── SistemaDeArquivos.cpp       # Orquestrador do filesystem
├── Arquivo.cpp                 # Operações de arquivo
├── Diretorio.cpp               # Operações de diretório
└── MapaDeBits.cpp              # Gerencimento de blocos livres
│
include/fs/
├── DiscoVirtual.h              # Interface de disco
├── Superbloco.h                # Metadados do filesystem
├── BtrfsKey.h                  # Chave BTRFS
├── BtrfsNo.h                   # Nó da árvore B
├── BtrfsBloco.h                # Bloco BTRFS
├── GerenciadorArvoreB.h        # Gerenciador de árvore B
├── Inode.h                     # Estrutura de inode
├── MapaDeBits.h                # Mapa de bits
├── Arquivo.h                   # Interface de arquivo
├── Diretorio.h                 # Interface de diretório
└── SistemaDeArquivos.h         # Interface do filesystem
```

---

## 🚀 Como Usar

### Compilação

```bash
cmake -B build
cd build
cmake --build .
```

### Executar o Shell

```bash
./BTRFS
```

O programa oferecerá duas opções:
1. **Formatar novo disco** - Cria um filesystem vazio (64 blocos de 4KB)
2. **Montar disco existente** - Carrega um filesystem anterior de `btrfs_disco.bin`

### Exemplo de Sessão

```
==================================================
          MINI-BTRFS SHELL INTERATIVO             
==================================================

Selecione o modo de inicialização:
1. Formatar um NOVO disco (Apaga tudo)
2. Montar disco EXISTENTE (Carrega de btrfs_disco.bin)
Escolha (1/2): 1

[FS] Formatando disco via Copy-on-Write...
Bem-vindo ao shell BTRFS! Digite 'help' para comandos.

btrfs:/> mkdir documentos
Diretorio 'documentos' criado com sucesso.

btrfs:/> touch documentos/arquivo.txt
Arquivo criado! Digite o texto que deseja salvar nele:
> Olá, BTRFS!
[BTRFS] Dados gravados em Inline Extents usando CoW!

btrfs:/> cat documentos/arquivo.txt
--------------------------------
Olá, BTRFS!
--------------------------------

btrfs:/> ls
  [DIR] documentos (ID: 257)

btrfs:/> exit
[Hardware] Sincronizando filesystem e desmontando...
Sessão encerrada com segurança.
```

---

## 📋 Especificações Técnicas

| Aspecto | Detalhes |
|--------|----------|
| **Linguagem** | C++23 |
| **Padrão de Compilação** | `-Wall -Wextra -Wpedantic` (sem warnings) |
| **Tamanho do Bloco** | 4KB (4096 bytes) |
| **Total de Blocos** | 64 (256 KB total) |
| **Estrutura de Dados** | Árvore B com Copy-on-Write |
| **Persistência** | Arquivo binário `btrfs_disco.bin` |
| **Tipo de Inode** | Metadados em Inline Extents |

---

## 📚 Conceitos Implementados

### Copy-on-Write (CoW)
Garante transações seguras: dados não são sobrescritos até que uma nova transação seja confirmada. Implementado via `clonar_bloco_cow` no gerenciador.

### Árvore B
Estrutura balanceada que mantém metadados organizados e permite buscas eficientes em O(log n).

### Extents
Unidades de alocação que mapeiam blocos físicos para arquivos, otimizando fragmentação.

### Mapa de Bits
Rastreamento eficiente de blocos livres/ocupados com operações bit a bit.

---

## ✅ Testes e Validação

O projeto foi compilado e executado com sucesso demonstrando:
- ✅ Formatação de disco
- ✅ Criação de diretórios
- ✅ Criação e escrita de arquivos
- ✅ Leitura de arquivos
- ✅ Persistência de dados
- ✅ Montagem de filesystem existente
- ✅ Navegação de árvore de diretórios
- ✅ Sincronização com disco

---

## 📝 Autor

Desenvolvido por **Danil Ribeiro**, **Davi de Cerqueira**, **Mateus Farias** e **Leo Lobo** como projeto acadêmico de Sistemas Operacionais.

---

## 📖 Referências

- BTRFS Filesystem: https://btrfs.readthedocs.io/
- B-Tree Data Structures
- Copy-on-Write Semantics
- Virtual Filesystem Concepts
