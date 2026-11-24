# duddle
## Editor de texto para terminal

### Instalação da biblioteca ncurse
sudo apt update
sudo apt install libncurses-dev g++

### Como compilar
g++ -std=c++17 duddleeditor.cpp -o duddleeditor -lncurses

### Para abrir o edito
./duddleeditor

### Você também pode abrir um arquivo diretamente
./duddleeditor exemplo.txt

Se o arquivo não existir, o editor apresentará uma mensagem de erro. Você poderá criar um arquivo usando Ctrl+X (Salvar como)

### Atalhos de teclado
Tecla	Função

↑ ↓ ← →	Movimentar cursor

Home	Início da linha

End	Final da linha

Page Up	Página anterior

Page Down	Próxima página

Enter	Nova linha

Backspace	Apagar caractere anterior

Delete	Apagar caractere atual

Ctrl + S	Salvar arquivo

Ctrl + O	Abrir arquivo

Ctrl + X	Salvar como

Ctrl + Q	Sair do editor

### Organização interna do programa
O editor foi organizado em métodos para facilitar futuras melhorias:

Método	Responsabilidade

inicializar()	Configura ncurses

desenharTela()	Atualiza a interface

desenharTexto()	Exibe o documento

desenharStatus()	Exibe informações

inserirCaractere()	Insere caracteres

inserirNovaLinha()	Divide linhas

backspace()	Remove caracteres

deleteChar()	Exclui caracteres

moverCursor()	Controla a navegação

abrirArquivo()	Carrega arquivos

salvarArquivo()	Grava arquivos

processarTecla()	Interpreta atalhos

Observação técnica: esta versão trabalha com posições de caracteres em bytes. Portanto, caracteres acentuados e outros símbolos UTF-8 podem apresentar problemas de posicionamento do cursor. Também ainda não possui destaque de sintaxe.
