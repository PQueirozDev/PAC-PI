#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define LARGURA 880
#define ALTURA 600
#define TOTAL 27
#define TOTAL_PERGUNTAS 15
#define PONTOS_POR_ACERTO 10

#define PRIMEIRO_CHEFE 13
#define PONTOS_CHEFE 30
#define PONTUACAO_MAXIMA ((PRIMEIRO_CHEFE * PONTOS_POR_ACERTO) + (TOTAL_PERGUNTAS - PRIMEIRO_CHEFE) * PONTOS_CHEFE)

#define TELA_MENU 0
#define TELA_TUTORIAL 1
#define TELA_CREDITOS 2
#define TELA_JOGO 3
#define TELA_FIM 4

#define TOTAL_BOTOES 3
#define BOTAO_L 260
#define BOTAO_A 50
#define BOTAO_X ((LARGURA - BOTAO_L) / 2)
#define BOTAO_Y 250
#define BOTAO_ESPACO 70

#define VOLTAR_L 180
#define VOLTAR_A 44
#define VOLTAR_X ((LARGURA - VOLTAR_L) / 2)
#define VOLTAR_Y 520

#define MAPA_LINHAS 46
#define MAPA_COLUNAS 47
#define CELULA 8
#define MAPA_X 412
#define MAPA_Y 82

char LETRAS[TOTAL + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";

char *mapa[MAPA_LINHAS] = {
    "................A..............................",
    "..............AAA..............................",
    "............AAAAA.........BB...................",
    "............AAAAA.....D..BBB...................",
    ".....CCC....CCAAAA.DDDDDBBBBB..................",
    ".....CCCCCCCCCAAAADDDDDDDBBBB..................",
    ".....CCCCCCCCCAAACDDDDDDDBBDDD.................",
    ".....CCCCCCCCCACCCCDDDDDDDBDDDDDD..............",
    ".....CCCCCCCCCCCCCCCDDDDDDDDDDDDDEE............",
    ".....CCCCCCCCCCCCCCCCDDDDDDDDDDDDEEEEE.........",
    ".....CCCCCCCCCCCCCCCCDDDDDDDDDDDEEEEEELFFF.....",
    "...CCCCCCCCCCCCCCCCCDDDDDDDDDDDDEEEEELLFFFF....",
    ".CCCCCCCCCCCCCCCCCCCDDDDDDDDDDDEEEEEELLFFFFGGG.",
    ".CCCCCCCCCCCCCCCCCCDDDDDDDDDDDDKEEEEELLLFFFGGGG",
    "CCCCCCCCCCCCCCCCCCCDDDDDDDDDDDKKEEEELLLLFFNNNNN",
    "HHCCCCCCCCCCCCCCCCCDDDDDDDDDDDKKEELLLLLLMMMNNNN",
    ".HHHHCCCCCCCIICCCCCJDDDDDDDDDKKKKELLLLLMMMMMMMM",
    ".HHHHHHCCCCIIIIJJJJJJDDDDDDDDKKKKELLLLRRRRTTTT.",
    "....HHHHH.IIIIIJJJJJJJJJJJJJKKKKKELLRRRRRRSST..",
    "....HHH...IIIIIIJJJJJJJJJJJJKKKKKRRRRRRRRRSS...",
    "...........IIIIIIJJJJJJJJJJJKKKKKRRRRRRRRRRR...",
    ".............IIIIJJJJJJJJJJJKKKKKRRRRRRRRRR....",
    "...............IJJJJJJJJJJJJPPPPPRRRRRRRRR.....",
    "................JJJJJJJJJJJJPPPPPRRVRRRRRR.....",
    "................JJJJJJJJJJJPPPPPPVVVVVRRRR.....",
    ".................JJJJJJJJJPPPPPQQVVVVVVVVR.....",
    "...................JJJJJJPPPPPPQQVVVVVVVRR.....",
    "...................JOOOOJPPPPPPPVVVVVVVVRR.....",
    "...................OOOOOOPPPPVVVVVVVVVVWW......",
    "...................OOOOOOOOPVVVVVVVVVVVWW......",
    "...................OOOOOOOOUUUUUVVVVVVVWW......",
    "...................OOOOOOOOUUUUUVVVVVVYW.......",
    "...................OOOOOOOUUUUUUUVVVVYYY.......",
    "......................OOOXXUUUUUUVUYYY.........",
    "......................OOXXXXXUUUUUU............",
    ".......................XXXXXXXUU...............",
    ".......................XXXXXXXX................",
    "........................XXXXZXZ................",
    "........................ZZZZZZZ................",
    ".......................####ZZZZ................",
    ".....................########Z.................",
    "....................#########..................",
    "....................#########..................",
    "......................#####....................",
    "........................##.....................",
    ".........................#.....................",
};

int estado_da_letra(char letra)
{
    for (int i = 0; i < TOTAL; i++) {
        if (LETRAS[i] == letra) {
            return i;
        }
    }
    return -1;
}

int estado_na_tela(int x, int y)
{
    if (x < MAPA_X || y < MAPA_Y) {
        return -1;
    }

    int coluna = (x - MAPA_X) / CELULA;
    int linha = (y - MAPA_Y) / CELULA;

    if (coluna >= MAPA_COLUNAS || linha >= MAPA_LINHAS) {
        return -1;
    }
    return estado_da_letra(mapa[linha][coluna]);
}

void escrever_grande(ALLEGRO_FONT *fonte, ALLEGRO_COLOR cor, float x, float y, int escala, char *texto)
{
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_scale_transform(&t, escala, escala);
    al_use_transform(&t);
    al_draw_text(fonte, cor, x / escala, y / escala, ALLEGRO_ALIGN_CENTRE, texto);
    al_identity_transform(&t);
    al_use_transform(&t);
}

int eh_chefe(int p)
{
    return p >= PRIMEIRO_CHEFE;
}

int pontos_da_pergunta(int p)
{
    if (eh_chefe(p)) {
        return PONTOS_CHEFE;
    }
    return PONTOS_POR_ACERTO;
}

/* A tela cheia: o jogo e desenhado em uma tela virtual de LARGURA x ALTURA
   e depois ampliado para o tamanho do monitor, com faixas pretas nas sobras. */
float tela_escala = 1;
float tela_x = 0;
float tela_y = 0;

void desenhar_tela_virtual(ALLEGRO_DISPLAY *janela, ALLEGRO_BITMAP *tela_virtual)
{
    float largura_janela = al_get_display_width(janela);
    float altura_janela = al_get_display_height(janela);

    tela_escala = largura_janela / LARGURA;
    if (altura_janela / ALTURA < tela_escala) {
        tela_escala = altura_janela / ALTURA;
    }
    tela_x = (largura_janela - LARGURA * tela_escala) / 2;
    tela_y = (altura_janela - ALTURA * tela_escala) / 2;

    al_set_target_backbuffer(janela);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(tela_virtual, 0, 0, LARGURA, ALTURA,
        tela_x, tela_y, LARGURA * tela_escala, ALTURA * tela_escala, 0);
}

/* Converte a posicao do mouse no monitor para a posicao na tela do jogo. */
int mouse_jogo_x(int x)
{
    return (int)((x - tela_x) / tela_escala);
}

int mouse_jogo_y(int y)
{
    return (int)((y - tela_y) / tela_escala);
}

int mouse_dentro(int mx, int my, int x, int y, int largura, int altura)
{
    if (mx >= x && mx <= x + largura && my >= y && my <= y + altura) {
        return 1;
    }
    return 0;
}

void desenhar_botao(ALLEGRO_FONT *fonte, char *texto, int x, int y, int largura, int altura, int destacado)
{
    ALLEGRO_COLOR cor_fundo = al_map_rgb(32, 46, 88);
    ALLEGRO_COLOR cor_borda = al_map_rgb(80, 100, 160);
    if (destacado) {
        cor_fundo = al_map_rgb(70, 150, 80);
        cor_borda = al_map_rgb(240, 200, 40);
    }
    al_draw_filled_rounded_rectangle(x, y, x + largura, y + altura, 10, 10, cor_fundo);
    al_draw_rounded_rectangle(x, y, x + largura, y + altura, 10, 10, cor_borda, 3);
    escrever_grande(fonte, al_map_rgb(255, 255, 255), x + largura / 2, y + altura / 2 - 8, 2, texto);
}

int main()
{
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_image_addon();

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    ALLEGRO_DISPLAY *janela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_BITMAP *tela_virtual = al_create_bitmap(LARGURA, ALTURA);
    int tela_cheia = 1;
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    ALLEGRO_FONT *fonte = al_create_builtin_font();

    al_set_window_title(janela, "Onde Fica?");

    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_mouse_event_source());

    char *siglas[TOTAL] = {
        "RR", "AP", "AM", "PA", "MA", "CE", "RN",
        "AC", "RO", "MT", "TO", "PI", "PE", "PB",
        "MS", "GO", "DF", "BA", "SE", "AL",
        "SP", "MG", "ES", "PR", "RJ", "SC", "RS"
    };
    char *nomes[TOTAL] = {
        "Roraima", "Amapa", "Amazonas", "Para", "Maranhao", "Ceara", "Rio Grande do Norte",
        "Acre", "Rondonia", "Mato Grosso", "Tocantins", "Piaui", "Pernambuco", "Paraiba",
        "Mato Grosso do Sul", "Goias", "Distrito Federal", "Bahia", "Sergipe", "Alagoas",
        "Sao Paulo", "Minas Gerais", "Espirito Santo", "Parana", "Rio de Janeiro",
        "Santa Catarina", "Rio Grande do Sul"
    };

    int rotulo_linha[TOTAL] = {
        3, 4, 11, 11, 12, 12, 12,
        17, 19, 21, 18, 15, 16, 14,
        30, 27, 25, 21, 19, 17,
        32, 28, 29, 35, 33, 38, 41
    };
    int rotulo_coluna[TOTAL] = {
        15, 26, 11, 25, 34, 40, 48,
        4, 13, 21, 30, 37, 48, 48,
        22, 26, 31, 38, 45, 47,
        30, 35, 43, 26, 40, 28, 24
    };

    char *monumentos[TOTAL_PERGUNTAS] = {
        "Cristo Redentor",
        "Teatro Amazonas",
        "Congresso Nacional",
        "Elevador Lacerda",
        "Arcos da Lapa",
        "Monumento as Bandeiras",
        "Praca da Se",
        "Lencois Maranhenses",
        "Opera de Arame",
        "Monumento aos Formadores de Nacionalidade",
        "Estatua do Padre Cicero",
        "Relogio Municipal",
        "Ponte dos Ingleses",
        "Forte dos Reis Magos",
        "Real Forte Principe da Beira"
    };
    char *arquivos[TOTAL_PERGUNTAS] = {
        "cristo.png",
        "teatro.png",
        "congresso.png",
        "elevador.png",
        "arcos.png",
        "bandeiras.png",
        "se.png",
        "lencois.png",
        "opera.png",
        "formadores.png",
        "padre_cicero.png",
        "relogio.png",
        "ponte_ingleses.png",
        "reis_magos.png",
        "principe_beira.png"
    };
    int respostas[TOTAL_PERGUNTAS] = {
        24,
        2,
        16,
        17,
        24,
        20,
        20,
        4,
        23,
        18,
        5,
        2,
        5,
        6,
        8
    };
    char *dicas[TOTAL_PERGUNTAS] = {
        "DICA: fica na regiao Sudeste e tem litoral.",
        "DICA: fica na regiao Norte e e o maior estado do Brasil.",
        "DICA: fica no Centro-Oeste e e onde esta a capital do pais.",
        "DICA: fica no Nordeste, no estado com o maior litoral do Brasil.",
        "DICA: fica no Sudeste, no estado que ja foi sede da capital do Brasil.",
        "DICA: fica no Sudeste, no estado mais populoso do Brasil.",
        "DICA: fica na maior cidade do Brasil.",
        "DICA: fica no Nordeste, na divisa com o Piaui.",
        "DICA: fica no Sul, no estado das Cataratas do Iguacu.",
        "DICA: fica no Nordeste, no menor estado do Brasil.",
        "DICA: fica no Nordeste, no estado cuja capital e Fortaleza.",
        "DICA: fica no Norte, na mesma cidade do Teatro Amazonas.",
        "DICA: fica no Nordeste, no mesmo estado da estatua do Padre Cicero.",
        "",
        ""
    };
    char *curiosidades[TOTAL_PERGUNTAS] = {
        "Inaugurado em 1931, fica no alto do morro do Corcovado.",
        "Inaugurado em 1896 em Manaus, na epoca do ciclo da borracha.",
        "Projetado por Oscar Niemeyer e inaugurado em 1960 em Brasilia.",
        "Inaugurado em 1873 em Salvador, liga a Cidade Baixa a Cidade Alta.",
        "Era um aqueduto que levava agua ao centro do Rio. Hoje o bondinho passa por cima.",
        "Esculpido por Victor Brecheret e inaugurado em 1953, perto do Parque Ibirapuera.",
        "E o marco zero da cidade de Sao Paulo. A catedral foi inaugurada em 1954.",
        "Dunas de areia branca com lagoas de agua da chuva. Virou parque nacional em 1981.",
        "Teatro de Curitiba feito de tubos de aco e teto transparente, cercado por um lago.",
        "Fica na orla de Atalaia, em Aracaju, com estatuas de Zumbi, Tiradentes e Dom Pedro II.",
        "Fica na Colina do Horto, em Juazeiro do Norte, e foi inaugurada em 1969.",
        "Fica no Centro Historico de Manaus. A engrenagem do relogio veio da Suica.",
        "Tambem chamada de Ponte Metalica, fica na Praia de Iracema, em Fortaleza.",
        "Comecou a ser construido em 1598 em Natal, na foz do rio Potengi.",
        "Construido entre 1776 e 1783 no rio Guapore, na fronteira com a Bolivia."
    };

    ALLEGRO_BITMAP *fotos[TOTAL_PERGUNTAS];
    for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
        fotos[p] = al_load_bitmap(arquivos[p]);
        if (fotos[p] == NULL) {
            printf("Aviso: nao achei o arquivo %s\n", arquivos[p]);
        }
    }

    char *botoes[TOTAL_BOTOES] = { "JOGAR", "COMO JOGAR", "CREDITOS" };

    char *tutorial[7] = {
        "1. Olhe a foto e o nome do monumento.",
        "2. Clique no estado do mapa onde voce acha que ele fica.",
        "3. Acertou? Voce ganha 10 pontos.",
        "4. Errou? O estado certo aparece em amarelo no mapa.",
        "5. Aperte ENTER para ir para a proxima pergunta.",
        "6. Sao 15 perguntas. No final aparece a sua pontuacao.",
        "7. As 2 ultimas sao CHEFES FINAIS: valem 30 pontos e nao tem dica!"
    };

    char *desenvolvedores[3] = { "Arthur Pimentel", "Christopher Nithigale", "Pedro Queiroz" };

    int pergunta = 0;
    int pontos = 0;
    int acertos = 0;
    int acertou[TOTAL_PERGUNTAS] = { 0 };
    int escolhido = -1;
    int dica = 0;
    int tela = TELA_MENU;
    int novo_jogo = 0;
    int mouse_x = 0;
    int mouse_y = 0;
    int rodando = 1;

    while (rodando) {
        ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
        ALLEGRO_COLOR preto = al_map_rgb(0, 0, 0);
        ALLEGRO_COLOR amarelo = al_map_rgb(240, 200, 40);
        ALLEGRO_COLOR verde_claro = al_map_rgb(120, 230, 120);
        ALLEGRO_COLOR vermelho_claro = al_map_rgb(240, 110, 100);
        ALLEGRO_COLOR cinza = al_map_rgb(170, 180, 200);
        ALLEGRO_COLOR fundo = al_map_rgb(20, 30, 60);
        ALLEGRO_COLOR painel = al_map_rgb(32, 46, 88);
        ALLEGRO_COLOR borda = al_map_rgb(80, 100, 160);

        al_set_target_bitmap(tela_virtual);
        al_clear_to_color(fundo);

        if (tela == TELA_MENU) {
            escrever_grande(fonte, amarelo, LARGURA / 2, 80, 7, "ONDE FICA?");
            al_draw_text(fonte, branco, LARGURA / 2, 160, ALLEGRO_ALIGN_CENTRE,
                "Descubra em qual estado ficam os monumentos do Brasil!");
            al_draw_line(LARGURA / 2 - 200, 190, LARGURA / 2 + 200, 190, borda, 2);

            for (int b = 0; b < TOTAL_BOTOES; b++) {
                int y = BOTAO_Y + b * BOTAO_ESPACO;
                int destacado = mouse_dentro(mouse_x, mouse_y, BOTAO_X, y, BOTAO_L, BOTAO_A);
                desenhar_botao(fonte, botoes[b], BOTAO_X, y, BOTAO_L, BOTAO_A, destacado);
            }

            al_draw_text(fonte, cinza, LARGURA / 2, 540, ALLEGRO_ALIGN_CENTRE, "Clique em um botao. ESC = sair");
            al_draw_text(fonte, cinza, LARGURA / 2, 565, ALLEGRO_ALIGN_CENTRE, "Projeto Integrador");
        }
        else if (tela == TELA_TUTORIAL) {
            al_draw_filled_rounded_rectangle(80, 30, LARGURA - 80, 500, 12, 12, painel);
            al_draw_rounded_rectangle(80, 30, LARGURA - 80, 500, 12, 12, borda, 3);
            escrever_grande(fonte, amarelo, LARGURA / 2, 55, 4, "COMO JOGAR");

            for (int i = 0; i < 7; i++) {
                al_draw_text(fonte, branco, 130, 115 + i * 26, 0, tutorial[i]);
            }

            al_draw_text(fonte, amarelo, 130, 305, 0, "CORES DO MAPA");
            al_draw_filled_rectangle(130, 325, 146, 341, al_map_rgb(130, 210, 140));
            al_draw_text(fonte, branco, 160, 330, 0, "mouse em cima do estado");
            al_draw_filled_rectangle(130, 350, 146, 366, amarelo);
            al_draw_text(fonte, branco, 160, 355, 0, "resposta certa");
            al_draw_filled_rectangle(130, 375, 146, 391, al_map_rgb(210, 60, 50));
            al_draw_text(fonte, branco, 160, 380, 0, "escolha errada");

            al_draw_text(fonte, amarelo, 470, 305, 0, "TECLAS");
            al_draw_text(fonte, branco, 470, 330, 0, "D     = mostrar uma dica");
            al_draw_text(fonte, branco, 470, 355, 0, "ENTER = proxima pergunta");
            al_draw_text(fonte, branco, 470, 380, 0, "R     = reiniciar o jogo");
            al_draw_text(fonte, branco, 470, 405, 0, "ESC   = voltar ao menu");
            al_draw_text(fonte, branco, 470, 430, 0, "F11   = tela cheia / janela");

            al_draw_text(fonte, cinza, LARGURA / 2, 455, ALLEGRO_ALIGN_CENTRE,
                "Dica: os estados pequenos do litoral tem a sigla escrita no mar.");

            desenhar_botao(fonte, "VOLTAR", VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A,
                mouse_dentro(mouse_x, mouse_y, VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A));
        }
        else if (tela == TELA_CREDITOS) {
            al_draw_filled_rounded_rectangle(160, 30, LARGURA - 160, 500, 12, 12, painel);
            al_draw_rounded_rectangle(160, 30, LARGURA - 160, 500, 12, 12, borda, 3);
            escrever_grande(fonte, amarelo, LARGURA / 2, 55, 4, "CREDITOS");

            al_draw_text(fonte, amarelo, LARGURA / 2, 150, ALLEGRO_ALIGN_CENTRE, "DESENVOLVIDO POR");
            for (int i = 0; i < 3; i++) {
                escrever_grande(fonte, branco, LARGURA / 2, 185 + i * 45, 3, desenvolvedores[i]);
            }

            al_draw_line(LARGURA / 2 - 150, 360, LARGURA / 2 + 150, 360, borda, 2);
            al_draw_text(fonte, cinza, LARGURA / 2, 385, ALLEGRO_ALIGN_CENTRE, "Projeto Integrador");
            al_draw_text(fonte, cinza, LARGURA / 2, 405, ALLEGRO_ALIGN_CENTRE, "Feito em C com Allegro 5");

            desenhar_botao(fonte, "VOLTAR", VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A,
                mouse_dentro(mouse_x, mouse_y, VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A));
        }
        else if (tela == TELA_FIM) {
            char texto[60];

            al_draw_filled_rounded_rectangle(30, 60, LARGURA - 30, 540, 12, 12, painel);
            al_draw_rounded_rectangle(30, 60, LARGURA - 30, 540, 12, 12, borda, 3);

            escrever_grande(fonte, amarelo, LARGURA / 2, 95, 4, "FIM DE JOGO!");

            snprintf(texto, sizeof(texto), "%d de %d pontos", pontos, PONTUACAO_MAXIMA);
            escrever_grande(fonte, branco, LARGURA / 2, 150, 3, texto);

            snprintf(texto, sizeof(texto), "Acertos: %d de %d", acertos, TOTAL_PERGUNTAS);
            escrever_grande(fonte, branco, LARGURA / 2, 192, 2, texto);

            al_draw_line(200, 228, 680, 228, borda, 2);

            for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
                int x = 50 + (p / 8) * 395;
                int y = 240 + (p % 8) * 18;
                char *marca = "";
                if (eh_chefe(p)) {
                    marca = " CHEFE";
                }
                if (acertou[p]) {
                    al_draw_textf(fonte, verde_claro, x, y, 0,
                        "OK %s (%s)%s", monumentos[p], siglas[respostas[p]], marca);
                }
                else {
                    al_draw_textf(fonte, vermelho_claro, x, y, 0,
                        "X  %s (%s)%s", monumentos[p], siglas[respostas[p]], marca);
                }
            }

            al_draw_line(200, 392, 680, 392, borda, 2);

            int chefes_vencidos = 0;
            for (int p = PRIMEIRO_CHEFE; p < TOTAL_PERGUNTAS; p++) {
                chefes_vencidos = chefes_vencidos + acertou[p];
            }

            if (acertos == TOTAL_PERGUNTAS) {
                escrever_grande(fonte, amarelo, LARGURA / 2, 410, 2, "Excelente! Gabaritou!");
                al_draw_text(fonte, branco, LARGURA / 2, 442, ALLEGRO_ALIGN_CENTRE,
                    "Voce conhece muito bem o Brasil e venceu os dois chefes.");
            }
            else if (chefes_vencidos == TOTAL_PERGUNTAS - PRIMEIRO_CHEFE) {
                escrever_grande(fonte, amarelo, LARGURA / 2, 410, 2, "Chefes derrotados!");
                al_draw_text(fonte, branco, LARGURA / 2, 442, ALLEGRO_ALIGN_CENTRE,
                    "Voce venceu os dois fortes. Agora tente acertar todos.");
            }
            else if (acertos > 0) {
                escrever_grande(fonte, amarelo, LARGURA / 2, 410, 2, "Bom trabalho!");
                al_draw_text(fonte, branco, LARGURA / 2, 442, ALLEGRO_ALIGN_CENTRE,
                    "Jogue de novo para tentar acertar todos.");
            }
            else {
                escrever_grande(fonte, amarelo, LARGURA / 2, 410, 2, "Nao foi dessa vez!");
                al_draw_text(fonte, branco, LARGURA / 2, 442, ALLEGRO_ALIGN_CENTRE,
                    "Use a dica (tecla D) e tente de novo.");
            }

            al_draw_text(fonte, cinza, LARGURA / 2, 500, ALLEGRO_ALIGN_CENTRE,
                "R ou ENTER = jogar de novo     ESC = menu");
        }
        else {

            al_draw_filled_rectangle(0, 0, LARGURA, 56, painel);
            al_draw_line(0, 56, LARGURA, 56, borda, 2);
            escrever_grande(fonte, amarelo, 140, 16, 3, "ONDE FICA?");
            al_draw_text(fonte, cinza, 440, 24, ALLEGRO_ALIGN_CENTRE, "D = dica   R = reiniciar   ESC = menu");
            if (eh_chefe(pergunta)) {
                al_draw_textf(fonte, vermelho_claro, LARGURA - 20, 14, ALLEGRO_ALIGN_RIGHT,
                    "CHEFE FINAL %d de %d", pergunta - PRIMEIRO_CHEFE + 1, TOTAL_PERGUNTAS - PRIMEIRO_CHEFE);
            }
            else {
                al_draw_textf(fonte, branco, LARGURA - 20, 14, ALLEGRO_ALIGN_RIGHT,
                    "Pergunta %d de %d", pergunta + 1, TOTAL_PERGUNTAS);
            }
            al_draw_textf(fonte, amarelo, LARGURA - 20, 34, ALLEGRO_ALIGN_RIGHT,
                "Pontos: %d   Acertos: %d", pontos, acertos);

            if (strlen(monumentos[pergunta]) <= 20) {
                escrever_grande(fonte, branco, 170, 66, 2, monumentos[pergunta]);
            }
            else {
                al_draw_text(fonte, branco, 170, 70, ALLEGRO_ALIGN_CENTRE, monumentos[pergunta]);
            }
            if (fotos[pergunta] != NULL) {
                al_draw_bitmap(fotos[pergunta], 20, 90, 0);
            }
            else {
                al_draw_filled_rectangle(20, 90, 320, 450, painel);
                al_draw_text(fonte, branco, 170, 260, ALLEGRO_ALIGN_CENTRE, "Imagem nao encontrada:");
                al_draw_text(fonte, amarelo, 170, 280, ALLEGRO_ALIGN_CENTRE, arquivos[pergunta]);
                al_draw_text(fonte, cinza, 170, 310, ALLEGRO_ALIGN_CENTRE, "(coloque a foto na pasta do projeto)");
            }
            if (eh_chefe(pergunta)) {
                al_draw_rectangle(20, 90, 320, 450, al_map_rgb(210, 60, 50), 5);
            }
            else {
                al_draw_rectangle(20, 90, 320, 450, branco, 3);
            }

            al_draw_text(fonte, cinza, 600, 66, ALLEGRO_ALIGN_CENTRE,
                "Clique no estado onde fica o monumento:");
            int estado_mouse = estado_na_tela(mouse_x, mouse_y);

            for (int lin = 0; lin < MAPA_LINHAS; lin++) {
                for (int col = 0; col < MAPA_COLUNAS; col++) {
                    char letra = mapa[lin][col];
                    if (letra == '.') {
                        continue;
                    }

                    int i = estado_da_letra(letra);
                    int x = MAPA_X + col * CELULA;
                    int y = MAPA_Y + lin * CELULA;

                    ALLEGRO_COLOR cor = al_map_rgb(70, 150, 80);
                    if (escolhido != -1 && i == respostas[pergunta]) {
                        cor = amarelo;
                    }
                    else if (i == escolhido) {
                        cor = al_map_rgb(210, 60, 50);
                    }
                    else if (escolhido == -1 && i == estado_mouse && !eh_chefe(pergunta)) {
                        cor = al_map_rgb(130, 210, 140);
                    }
                    al_draw_filled_rectangle(x, y, x + CELULA, y + CELULA, cor);

                    if (lin == 0 || mapa[lin - 1][col] != letra) {
                        al_draw_line(x, y, x + CELULA, y, preto, 1);
                    }
                    if (lin == MAPA_LINHAS - 1 || mapa[lin + 1][col] != letra) {
                        al_draw_line(x, y + CELULA, x + CELULA, y + CELULA, preto, 1);
                    }
                    if (col == 0 || mapa[lin][col - 1] != letra) {
                        al_draw_line(x, y, x, y + CELULA, preto, 1);
                    }
                    if (col == MAPA_COLUNAS - 1 || mapa[lin][col + 1] != letra) {
                        al_draw_line(x + CELULA, y, x + CELULA, y + CELULA, preto, 1);
                    }
                }
            }

            for (int i = 0; i < TOTAL; i++) {
                int x = MAPA_X + rotulo_coluna[i] * CELULA + CELULA / 2;
                int y = MAPA_Y + rotulo_linha[i] * CELULA;
                ALLEGRO_COLOR cor_sigla = preto;
                if (rotulo_coluna[i] >= MAPA_COLUNAS || mapa[rotulo_linha[i]][rotulo_coluna[i]] == '.') {
                    cor_sigla = branco;
                }
                al_draw_text(fonte, cor_sigla, x, y, ALLEGRO_ALIGN_CENTRE, siglas[i]);
            }

            al_draw_filled_rounded_rectangle(20, 465, LARGURA - 20, ALTURA - 10, 8, 8, painel);
            al_draw_rounded_rectangle(20, 465, LARGURA - 20, ALTURA - 10, 8, 8, borda, 2);

            if (escolhido == -1 && eh_chefe(pergunta)) {
                escrever_grande(fonte, vermelho_claro, 40 + 150, 478, 2, "CHEFE FINAL!");
                al_draw_textf(fonte, branco, 40, 505, 0,
                    "Vale %d pontos. Sem dica e sem o mapa acender: so voce e o que voce sabe.", PONTOS_CHEFE);
            }
            else if (escolhido == -1) {
                al_draw_text(fonte, branco, 40, 480, 0, "Clique em um estado no mapa...");
                if (dica) {
                    al_draw_text(fonte, amarelo, 40, 505, 0, dicas[pergunta]);
                }
                else {
                    al_draw_text(fonte, cinza, 40, 505, 0, "Precisa de ajuda? Aperte D para ver uma dica.");
                }
            }
            else {
                if (escolhido == respostas[pergunta]) {
                    char texto[40];
                    snprintf(texto, sizeof(texto), "ACERTOU! +%d pontos", pontos_da_pergunta(pergunta));
                    escrever_grande(fonte, verde_claro, LARGURA / 2, 475, 2, texto);
                    al_draw_textf(fonte, branco, LARGURA / 2, 503, ALLEGRO_ALIGN_CENTRE,
                        "Parabens! Voce marcou %s corretamente.", nomes[respostas[pergunta]]);
                }
                else {
                    escrever_grande(fonte, vermelho_claro, LARGURA / 2, 475, 2, "QUASE!");
                    al_draw_textf(fonte, branco, LARGURA / 2, 503, ALLEGRO_ALIGN_CENTRE,
                        "Voce escolheu %s, mas a resposta certa era %s (em amarelo).",
                        nomes[escolhido], nomes[respostas[pergunta]]);
                }
                al_draw_text(fonte, cinza, LARGURA / 2, 523, ALLEGRO_ALIGN_CENTRE, curiosidades[pergunta]);

                if (pergunta < TOTAL_PERGUNTAS - 1) {
                    al_draw_text(fonte, amarelo, LARGURA / 2, 555, ALLEGRO_ALIGN_CENTRE,
                        ">> Aperte ENTER para a proxima pergunta <<");
                }
                else {
                    al_draw_text(fonte, amarelo, LARGURA / 2, 555, ALLEGRO_ALIGN_CENTRE,
                        ">> Aperte ENTER para ver o resultado <<");
                }
            }
        }

        desenhar_tela_virtual(janela, tela_virtual);
        al_flip_display();

        ALLEGRO_EVENT evento;
        al_wait_for_event(fila, &evento);

        /* Daqui para baixo o mouse ja fica nas coordenadas do jogo (880 x 600). */
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES || evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            evento.mouse.x = mouse_jogo_x(evento.mouse.x);
            evento.mouse.y = mouse_jogo_y(evento.mouse.y);
        }

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = 0;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            al_acknowledge_resize(janela);
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            int tecla = evento.keyboard.keycode;

            int enter = (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER);

            if (tecla == ALLEGRO_KEY_F11) {
                tela_cheia = !tela_cheia;
                al_set_display_flag(janela, ALLEGRO_FULLSCREEN_WINDOW, tela_cheia);
            }
            else if (tecla == ALLEGRO_KEY_ESCAPE) {
                if (tela == TELA_MENU) {
                    rodando = 0;
                }
                else {
                    tela = TELA_MENU;
                }
            }
            else if (tecla == ALLEGRO_KEY_D && tela == TELA_JOGO && !eh_chefe(pergunta)) {
                dica = 1;
            }
            else if ((tecla == ALLEGRO_KEY_R && (tela == TELA_JOGO || tela == TELA_FIM)) ||
                     (enter && tela == TELA_FIM)) {
                novo_jogo = 1;
            }
            else if (enter && tela == TELA_JOGO && escolhido != -1) {
                if (pergunta < TOTAL_PERGUNTAS - 1) {
                    pergunta++;
                    escolhido = -1;
                    dica = 0;
                }
                else {
                    tela = TELA_FIM;
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && tela == TELA_MENU) {
            for (int b = 0; b < TOTAL_BOTOES; b++) {
                int y = BOTAO_Y + b * BOTAO_ESPACO;
                if (mouse_dentro(evento.mouse.x, evento.mouse.y, BOTAO_X, y, BOTAO_L, BOTAO_A)) {
                    if (b == 0) {
                        novo_jogo = 1;
                    }
                    else if (b == 1) {
                        tela = TELA_TUTORIAL;
                    }
                    else {
                        tela = TELA_CREDITOS;
                    }
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN &&
                 (tela == TELA_TUTORIAL || tela == TELA_CREDITOS)) {
            if (mouse_dentro(evento.mouse.x, evento.mouse.y, VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A)) {
                tela = TELA_MENU;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && tela == TELA_JOGO && escolhido == -1) {
            escolhido = estado_na_tela(evento.mouse.x, evento.mouse.y);

            if (escolhido != -1 && escolhido == respostas[pergunta]) {
                pontos = pontos + pontos_da_pergunta(pergunta);
                acertos++;
                acertou[pergunta] = 1;
            }
        }

        if (novo_jogo) {
            pergunta = 0;
            pontos = 0;
            acertos = 0;
            for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
                acertou[p] = 0;
            }
            escolhido = -1;
            dica = 0;
            tela = TELA_JOGO;
            novo_jogo = 0;
        }
    }

    for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
        if (fotos[p] != NULL) {
            al_destroy_bitmap(fotos[p]);
        }
    }
    al_destroy_bitmap(tela_virtual);
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_display(janela);
    return 0;
}
