# Instruções para Configuração do Projeto

## Passos Iniciais

1. Abra o terminal do WSL como administrador e execute o seguinte comando para abrir o editor de código:
    ```bash
    code .
    ```

## Instalação de Dependências

Execute os seguintes comandos para atualizar o sistema e instalar as dependências necessárias:
    ```bash
    sudo apt update
    sudo apt install build-essential
    ```

## Compilação e Execução do Servidor

Compile o arquivo `server.cpp` e execute o servidor com os seguintes comandos:
    ```bash
    g++ server.cpp -o server
    ./server
    ```
