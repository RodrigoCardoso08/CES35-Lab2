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
Compile o arquivo `client.cpp` e execute o client com os seguintes comandos:
    ```bash
    g++ client.cpp -o client
    ./client droneId x y z vx vy vz
    ```
## Como usar o servidor
No terminal do servidor pode-se utilizar os seguintes comandos:
    1 (Para enviar a mensagem "quem está ai?")
    2 droneId (Para enviar a mensagem "Solicitar mais dados")
    3 droneId dx dy dz (Para enviar a mensagem de reposicionamento)