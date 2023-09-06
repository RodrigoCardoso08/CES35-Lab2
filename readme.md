# Instruções para Executar a aplicação

## Executando o servidor
- Abra o terminal na pasta dos arquivos server.exe e client.exe e execute o seguinte comando:
    ```bash
    ./server
    ```
## Executando o cliente
- Abra um novo terminal na pasta dos arquivos server.exe e client.exe e execute o seguinte comando:
    ```bash
    ./client localhost 2 5 6 8 1 2 3
    ```
- O comando acima irá criar um cliente com os seguintes parâmetros: 
    - hostName = localhost
    - id = 2
    - x = 5
    - y = 6
    - z = 8
    - vx = 1
    - vy = 2
    - vz = 3
# Instruções para Configuração do Projeto (Para desenvolvedores)

## Passos Iniciais

1. (Para usuários de Windows) Abra o terminal do WSL na raíz do projeto como administrador e execute o seguinte comando para abrir o editor de código:
    ```bash
    code .
    ```
2. Execute os seguintes comandos para atualizar o sistema e instalar as dependências necessárias:
    ```bash
    sudo apt update
    sudo apt install build-essential
    ```

## Compilação e Execução do Servidor

1. Compile o arquivo `server.cpp` e execute o servidor com os seguintes comandos:
    ```bash
    g++ server.cpp -o server

    ./server
    ```
2. Compile o arquivo `client.cpp` e execute o client com os seguintes comandos:
    ```bash
    g++ client.cpp -o client

    ./client droneId x y z vx vy vz
    ```
## Como usar o servidor
No terminal do servidor pode-se utilizar os seguintes comandos:
-    ```1 ``` (Para enviar a mensagem "quem está ai?")
-    ```2 droneId``` (Para enviar a mensagem "Solicitar mais dados")
-    ```3 droneId dx dy dz``` (Para enviar a mensagem de reposicionamento)