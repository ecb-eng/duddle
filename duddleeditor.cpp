
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Editor {
private:
    vector<string> linhas;

    string nomeArquivo = "sem_nome.txt";

    int cursorX = 0;
    int cursorY = 0;

    int deslocamentoX = 0;
    int deslocamentoY = 0;

    int largura;
    int altura;

    bool modificado = false;
    bool executando = true;

    string mensagem = "Bem-vindo ao Editor Duddle!";
    string ultimaMensagem = "";

    // Inicializacao da interface
    void inicializar() {
        initscr();
        raw();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(1);

        if (has_colors()) {
            start_color();
            use_default_colors();

            init_pair(1, COLOR_BLACK, COLOR_CYAN);
            init_pair(2, COLOR_YELLOW, -1);
            init_pair(3, COLOR_GREEN, -1);
        }

        getmaxyx(stdscr, altura, largura);

        if (linhas.empty()) {
            linhas.push_back("");
        }
    }

    // Atualiza dimensoes do terminal
    void atualizarDimensoes() {
        getmaxyx(stdscr, altura, largura);
    }

    // Mantem o cursor visivel
    void ajustarCursor() {
        int areaTexto = altura - 2;

        if (cursorY < deslocamentoY) {
            deslocamentoY = cursorY;
        }

        if (cursorY >= deslocamentoY + areaTexto) {
            deslocamentoY = cursorY - areaTexto + 1;
        }

        int areaColunas = max(1, largura - 7);

        if (cursorX < deslocamentoX) {
            deslocamentoX = cursorX;
        }

        if (cursorX >= deslocamentoX + areaColunas) {
            deslocamentoX = cursorX - areaColunas + 1;
        }
    }

    // Desenha a area de edicao
    void desenharTexto() {
        int areaTexto = altura - 2;

        for (int y = 0; y < areaTexto; y++) {
            int linhaAtual = y + deslocamentoY;

            move(y, 0);
            clrtoeol();

            attron(COLOR_PAIR(2));

            if (linhaAtual < (int)linhas.size()) {
                mvprintw(y, 0, "%4d ", linhaAtual + 1);
            } else {
                mvprintw(y, 0, "   ~ ");
            }

            attroff(COLOR_PAIR(2));

            if (linhaAtual < (int)linhas.size()) {
                string texto = linhas[linhaAtual];

                int inicio = deslocamentoX;

                if (inicio < (int)texto.size()) {
                    string visivel = texto.substr(
                        inicio,
                        max(0, largura - 6)
                    );

                    mvaddnstr(y, 5, visivel.c_str(),
                              largura - 6);
                }
            }
        }
    }

    // Desenha a barra de status
    void desenharStatus() {
        attron(COLOR_PAIR(1) | A_BOLD);

        move(altura - 2, 0);
        clrtoeol();

        mvprintw(
            altura - 2,
            0,
            " %s %s | %d linhas | %s ",
            nomeArquivo.c_str(),
            modificado ? "[+]" : "",
            (int)linhas.size(),
            "C++ Editor"
        );

        attroff(COLOR_PAIR(1) | A_BOLD);

        // Barra de mensagens
        move(altura - 1, 0);
        clrtoeol();

        attron(A_BOLD);
        mvprintw(altura - 1, 0, "%s", mensagem.c_str());
        attroff(A_BOLD);

        // Posiciona o cursor
        int telaY = cursorY - deslocamentoY;
        int telaX = cursorX - deslocamentoX + 5;

        if (telaY >= 0 && telaY < altura - 2 &&
            telaX >= 5 && telaX < largura) {
            move(telaY, telaX);
        }
    }

    // Redesenha a tela
    void desenharTela() {
        atualizarDimensoes();
        ajustarCursor();

        erase();

        desenharTexto();
        desenharStatus();

        refresh();
    }

    // Insere caractere
    void inserirCaractere(int caractere) {
        string& linha = linhas[cursorY];

        linha.insert(
            linha.begin() + cursorX,
            (char)caractere
        );

        cursorX++;
        modificado = true;
    }

    // Insere nova linha
    void inserirNovaLinha() {
        string& linha = linhas[cursorY];

        string novaLinha = linha.substr(cursorX);

        linha.erase(cursorX);

        linhas.insert(
            linhas.begin() + cursorY + 1,
            novaLinha
        );

        cursorY++;
        cursorX = 0;

        modificado = true;
    }

    // Apaga caractere anterior
    void backspace() {
        if (cursorX > 0) {
            string& linha = linhas[cursorY];

            linha.erase(cursorX - 1, 1);

            cursorX--;
            modificado = true;
        }
        else if (cursorY > 0) {
            int tamanhoAnterior = linhas[cursorY - 1].size();

            linhas[cursorY - 1] += linhas[cursorY];

            linhas.erase(linhas.begin() + cursorY);

            cursorY--;
            cursorX = tamanhoAnterior;

            modificado = true;
        }
    }

    // Apaga caractere atual
    void deleteChar() {
        string& linha = linhas[cursorY];

        if (cursorX < (int)linha.size()) {
            linha.erase(cursorX, 1);
            modificado = true;
        }
        else if (cursorY < (int)linhas.size() - 1) {
            linha += linhas[cursorY + 1];

            linhas.erase(linhas.begin() + cursorY + 1);

            modificado = true;
        }
    }

    // Abre arquivo
    void abrirArquivo(string nome) {
        ifstream arquivo(nome);

        if (!arquivo.is_open()) {
            mensagem = "Erro ao abrir arquivo!";
            return;
        }

        vector<string> novasLinhas;
        string linha;

        while (getline(arquivo, linha)) {
            novasLinhas.push_back(linha);
        }

        if (novasLinhas.empty()) {
            novasLinhas.push_back("");
        }

        linhas = novasLinhas;
        nomeArquivo = nome;

        cursorX = 0;
        cursorY = 0;
        deslocamentoX = 0;
        deslocamentoY = 0;

        modificado = false;

        mensagem = "Arquivo aberto com sucesso!";
    }

    // Salva arquivo
    void salvarArquivo() {
        ofstream arquivo(nomeArquivo);

        if (!arquivo.is_open()) {
            mensagem = "Erro ao salvar arquivo!";
            return;
        }

        for (const string& linha : linhas) {
            arquivo << linha << '\n';
        }

        arquivo.close();

        modificado = false;
        mensagem = "Arquivo salvo com sucesso!";
    }

    // Le nome do arquivo pela barra de comandos
    string solicitarNome(string texto) {
        string nome = nomeArquivo;

        curs_set(1);
        echo();

        move(altura - 1, 0);
        clrtoeol();

        printw("%s", texto.c_str());
        refresh();

        int resultado = getnstr(&nome[0], 0);

        noecho();

        if (resultado == ERR) {
            return "";
        }

        return nome;
    }

    // Versao segura da solicitacao de nome
    string lerNome(string texto) {
        string nome;

        move(altura - 1, 0);
        clrtoeol();

        printw("%s", texto.c_str());
        refresh();

        int tecla;

        while ((tecla = getch()) != '\n' &&
               tecla != KEY_ENTER &&
               tecla != 27) {

            if (tecla == KEY_BACKSPACE || tecla == 127) {
                if (!nome.empty()) {
                    nome.pop_back();
                }
            }
            else if (tecla >= 32 && tecla <= 126) {
                nome += (char)tecla;
            }

            move(altura - 1, 0);
            clrtoeol();

            printw("%s%s", texto.c_str(), nome.c_str());
            refresh();
        }

        if (tecla == 27) {
            return "";
        }

        return nome;
    }

    // Salvar como
    void salvarComo() {
        string nome = lerNome("Salvar como: ");

        if (!nome.empty()) {
            nomeArquivo = nome;
            salvarArquivo();
        }
    }

    // Abrir arquivo pelo teclado
    void abrirPeloTeclado() {
        string nome = lerNome("Abrir arquivo: ");

        if (!nome.empty()) {
            abrirArquivo(nome);
        }
    }

    // Movimentacao do cursor
    void moverCursor(int tecla) {
        switch (tecla) {
            case KEY_UP:
                if (cursorY > 0) {
                    cursorY--;
                }
                break;

            case KEY_DOWN:
                if (cursorY < (int)linhas.size() - 1) {
                    cursorY++;
                }
                break;

            case KEY_LEFT:
                if (cursorX > 0) {
                    cursorX--;
                }
                else if (cursorY > 0) {
                    cursorY--;
                    cursorX = linhas[cursorY].size();
                }
                break;

            case KEY_RIGHT:
                if (cursorX < (int)linhas[cursorY].size()) {
                    cursorX++;
                }
                else if (cursorY < (int)linhas.size() - 1) {
                    cursorY++;
                    cursorX = 0;
                }
                break;

            case KEY_HOME:
                cursorX = 0;
                break;

            case KEY_END:
                cursorX = linhas[cursorY].size();
                break;

            case KEY_PPAGE:
                cursorY = max(0, cursorY - (altura - 2));
                break;

            case KEY_NPAGE:
                cursorY = min(
                    (int)linhas.size() - 1,
                    cursorY + altura - 2
                );
                break;
        }

        cursorX = min(
            cursorX,
            (int)linhas[cursorY].size()
        );
    }

    // Processa teclas
    void processarTecla(int tecla) {
        switch (tecla) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
            case KEY_HOME:
            case KEY_END:
            case KEY_PPAGE:
            case KEY_NPAGE:
                moverCursor(tecla);
                break;

            case KEY_BACKSPACE:
            case 127:
            case 8:
                backspace();
                break;

            case KEY_DC:
                deleteChar();
                break;

            case '\n':
            case KEY_ENTER:
                inserirNovaLinha();
                break;

            case 19: // Ctrl + S
                salvarArquivo();
                break;

            case 15: // Ctrl + O
                abrirPeloTeclado();
                break;

            case 17: // Ctrl + Q
                if (modificado) {
                    if (ultimaMensagem == "sair") {
                        executando = false;
                    } else {
                        mensagem = "Alteracoes nao salvas! Ctrl+Q novamente para sair.";
                        ultimaMensagem = "sair";
                    }
                } else {
                    executando = false;
                }
                break;

            case 24: // Ctrl + X
                salvarComo();
                break;

            default:
                if (tecla >= 32 && tecla <= 255) {
                    inserirCaractere(tecla);
                }
                break;
        }

        if (tecla != 17) {
            ultimaMensagem.clear();
        }
    }

public:
    Editor() {
        linhas.push_back("");
    }

    ~Editor() {
        endwin();
    }

    // Loop principal
    void executar(int argc, char* argv[]) {
        inicializar();

        if (argc > 1) {
            abrirArquivo(argv[1]);
        }

        while (executando) {
            desenharTela();

            int tecla = getch();

            if (tecla == KEY_RESIZE) {
                atualizarDimensoes();
            } else {
                processarTecla(tecla);
            }
        }
    }
};

int main(int argc, char* argv[]) {
    Editor editor;

    editor.executar(argc, argv);

    return 0;
}
