# Mini BTRFS - Sistema de Arquivos (Em Desenvolvimento)

Este repositório contém a implementação de um "mini" sistema de arquivos inspirado na arquitetura do **BTRFS**, desenvolvido como requisito de avaliação acadêmica da disciplina de Sistemas Operacionais.

O projeto foca na demonstração de conceitos avançados como **Árvores B**, **Copy-on-Write (CoW)** e **Alocação Baseada em Extents**, operando sobre um disco virtual alocado em memória RAM.

---

## Status Atual: O Que Já Foi Construído

A arquitetura base e os motores estruturais do sistema já estão finalizados e validados com compilação C++23 limpa:

* **Fase 0: Setup de Ambiente**
    * Configuração rigorosa do CMake (`-Wall -Wextra -Wpedantic`).
    * Controle de versão limpo via `.gitignore`.
    * Separação modular de arquivos (`include/fs/` para interfaces, `src/` para implementação, `app/` para execução).
* **Fase 1: Disco Virtual**
    * `IDiscoVirtual`: Interface rigorosa com tratamento de erros baseada em `std::expected`.
    * `DiscoVirtualEmMemoria`: Simulação rápida e segura em RAM, com persistência (*flush*) em arquivo binário no momento da desmontagem. Blocos padronizados em 4KB.
* **Fase 2: Gerenciamento Físico e Superbloco**
    * `Superbloco`: Estrutura de metadados estática, gravada de forma fixa no Bloco 0, guardando parâmetros cruciais e a raiz do sistema.
    * `MapaDeBits`: Gerenciador de blocos livres que opera bit a bit para altíssimo desempenho, ditando quais blocos físicos de 4KB podem ser alocados.
* **Fase 3: Motor BTRFS (Árvore B e CoW)**
    * Abandono da arquitetura clássica Ext4 em prol das Árvores B estritamente tipadas via *Templates*.
    * `ChaveBtrfs`: A espinha dorsal do sistema (ID do Objeto + Tipo + Offset).
    * `GerenciadorArvoreB`: O coração do sistema. Implementa a lógica fundamental de **Copy-on-Write (CoW)** (`clonar_bloco_cow`), garantindo que os dados nunca sejam sobrescritos fisicamente antes de uma transação segura.
* **Fase 4: Interfaces e API (Definição)**
    * `SistemaDeArquivos`: Maestro de alto nível que orquestra formatação e montagem.
    * Classes Orientadas a Objetos: `Arquivo` e `Diretorio`, focadas em encapsular a complexidade da árvore B para o usuário final.

---

## Roadmap: O Que Falta Fazer

As próximas sessões de desenvolvimento focarão em dar vida à API Orientada a Objetos e conectar os testes automatizados.

### 1. Implementações Pendentes (Camada Lógica)
- [ ] **`src/Diretorio.cpp`**:
    - Criar a lógica para instanciar novos arquivos (`criar_arquivo`) e subpastas (`criar_diretorio`), inserindo estruturas de metadados na Árvore B através do CoW.
    - Implementar o método `listar()` iterando sobre as chaves pertencentes àquele ID de diretório.
- [ ] **`src/Arquivo.cpp`**:
    - Implementar o método `escrever()`, alocando e quebrando os dados informados em blocos lógicos (*Extents*) e gravando-os na árvore.
    - Implementar o método `ler()`, remontando o arquivo a partir das buscas por Extents na árvore.

### 2. Ponto de Entrada (Interação do Usuário)
- [ ] **`app/main.cpp`**:
    - Instanciar o `SistemaDeArquivos`, formatar um disco de demonstração, obter a pasta `/` (raiz), criar arquivos/diretórios reais e testar as operações de I/O para exibição na banca acadêmica.

### 3. Garantia de Qualidade
- [ ] **Testes Automatizados (GitHub Actions)**:
    - Configurar a pipeline CI/CD discutida na Fase 1 para instanciar o disco em RAM, rodar baterias de testes sobre a API (`main.cpp` ou módulo de testes dedicado) e garantir a "Corretude da Implementação" a cada push.
