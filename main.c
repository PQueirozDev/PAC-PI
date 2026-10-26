#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define LARGURA 880
#define ALTURA 600
#define TOTAL 27            // quantidade de estados (26 estados + DF)
#define TOTAL_PERGUNTAS 3   // quantidade de monumentos (perguntas) do jogo
#define PONTOS_POR_ACERTO 10

// Mapa
#define MAPA_X 360
#define MAPA_Y 90
#define BLOCO_L 56
#define BLOCO_A 40

int main()
{
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_image_addon();

    ALLEGRO_DISPLAY *janela = al_create_display(LARGURA, ALTURA);
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

    // coluna e linha de cada estado no mapa (mais ou menos onde fica no Brasil)
    int coluna[TOTAL] = {
        2, 4, 1, 3, 5, 6, 7,
        0, 1, 2, 4, 5, 6, 7,
        2, 3, 4, 5, 6, 7,
        3, 4, 5, 3, 4, 3, 3
    };
    int linha[TOTAL] = {
        0, 0, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3,
        4, 4, 4, 5, 5, 6, 7
    };

    // ===== PERGUNTAS =====
    // Cada pergunta usa a MESMA posicao em todos os vetores abaixo.
    // Exemplo: posicao 0 = Cristo Redentor, foto cristo.png, resposta 24 (RJ).
    char *monumentos[TOTAL_PERGUNTAS] = {
        "Cristo Redentor",
        "Teatro Amazonas",
        "Congresso Nacional"
    };
    char *arquivos[TOTAL_PERGUNTAS] = {
        "cristo.png",
        "teatro.png",
        "congresso.png"
    };
    // posicao do estado certo nos vetores siglas/nomes
    int respostas[TOTAL_PERGUNTAS] = {
        24, // RJ
        2,  // AM
        16  // DF
    };
    char *dicas[TOTAL_PERGUNTAS] = {
        "DICA: fica na regiao Sudeste e tem litoral.",
        "DICA: fica na regiao Norte e e o maior estado do Brasil.",
        "DICA: fica no Centro-Oeste e e onde esta a capital do pais."
    };
    char *curiosidades[TOTAL_PERGUNTAS] = {
        "Inaugurado em 1931, fica no alto do morro do Corcovado.",
        "Inaugurado em 1896 em Manaus, na epoca do ciclo da borracha.",
        "Projetado por Oscar Niemeyer e inaugurado em 1960 em Brasilia."
    };

    // carrega a foto de cada monumento
    ALLEGRO_BITMAP *fotos[TOTAL_PERGUNTAS];
    for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
        fotos[p] = al_load_bitmap(arquivos[p]);
        if (fotos[p] == NULL) {
            // o jogo continua, mas mostra um quadro no lugar da foto
            printf("Aviso: nao achei o arquivo %s\n", arquivos[p]);
        }
    }

    // ===== VARIAVEIS DO JOGO =====
    int pergunta = 0;   // qual pergunta esta na tela (0, 1 ou 2)
    int pontos = 0;     // pontuacao: +10 por acerto
    int acertos = 0;    // quantas perguntas o jogador acertou
    int acertou[TOTAL_PERGUNTAS] = { 0 }; // 1 = acertou aquela pergunta, 0 = errou
    int escolhido = -1; // -1 = ainda nao clicou
    int dica = 0;       // 1 = mostrar a dica
    int fim_de_jogo = 0; // 1 = mostrar a tela final
    int rodando = 1;

    while (rodando) {
        // desenha a tela
        ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
        ALLEGRO_COLOR preto = al_map_rgb(0, 0, 0);
        ALLEGRO_COLOR amarelo = al_map_rgb(240, 200, 40);
        ALLEGRO_COLOR verde_claro = al_map_rgb(120, 230, 120);
        ALLEGRO_COLOR vermelho_claro = al_map_rgb(240, 110, 100);

        al_clear_to_color(al_map_rgb(20, 30, 60));

        if (fim_de_jogo) {
            // ===== TELA FINAL =====
            al_draw_text(fonte, amarelo, LARGURA / 2, 100, ALLEGRO_ALIGN_CENTRE, "FIM DE JOGO!");
            al_draw_textf(fonte, branco, LARGURA / 2, 140, ALLEGRO_ALIGN_CENTRE,
                "Pontuacao: %d de %d pontos", pontos, TOTAL_PERGUNTAS * PONTOS_POR_ACERTO);
            al_draw_textf(fonte, branco, LARGURA / 2, 160, ALLEGRO_ALIGN_CENTRE,
                "Acertos: %d de %d", acertos, TOTAL_PERGUNTAS);

            // resumo de cada pergunta
            for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
                int y = 210 + p * 25;
                if (acertou[p]) {
                    al_draw_textf(fonte, verde_claro, LARGURA / 2, y, ALLEGRO_ALIGN_CENTRE,
                        "%s (%s) - ACERTOU", monumentos[p], nomes[respostas[p]]);
                }
                else {
                    al_draw_textf(fonte, vermelho_claro, LARGURA / 2, y, ALLEGRO_ALIGN_CENTRE,
                        "%s (%s) - ERROU", monumentos[p], nomes[respostas[p]]);
                }
            }

            // mensagem de acordo com a quantidade de acertos
            if (acertos == TOTAL_PERGUNTAS) {
                al_draw_text(fonte, amarelo, LARGURA / 2, 320, ALLEGRO_ALIGN_CENTRE,
                    "Excelente! Voce conhece muito bem o Brasil!");
            }
            else if (acertos > 0) {
                al_draw_text(fonte, amarelo, LARGURA / 2, 320, ALLEGRO_ALIGN_CENTRE,
                    "Bom trabalho! Jogue de novo para acertar todos.");
            }
            else {
                al_draw_text(fonte, amarelo, LARGURA / 2, 320, ALLEGRO_ALIGN_CENTRE,
                    "Nao foi dessa vez. Que tal tentar de novo?");
            }

            al_draw_text(fonte, branco, LARGURA / 2, 380, ALLEGRO_ALIGN_CENTRE,
                "Aperte R ou ENTER para jogar de novo. ESC para sair.");
        }
        else {
            // ===== TELA DA PERGUNTA =====
            al_draw_text(fonte, amarelo, LARGURA / 2, 15, ALLEGRO_ALIGN_CENTRE, "ONDE FICA?");
            al_draw_text(fonte, branco, LARGURA / 2, 35, ALLEGRO_ALIGN_CENTRE,
                "Clique no estado onde fica o monumento. D = dica, R = reiniciar, ESC = sair");
            al_draw_textf(fonte, branco, LARGURA - 20, 15, ALLEGRO_ALIGN_RIGHT,
                "Pergunta %d de %d   Pontos: %d", pergunta + 1, TOTAL_PERGUNTAS, pontos);

            // foto do monumento da pergunta atual
            al_draw_text(fonte, branco, 170, 70, ALLEGRO_ALIGN_CENTRE, monumentos[pergunta]);
            if (fotos[pergunta] != NULL) {
                al_draw_bitmap(fotos[pergunta], 20, 90, 0);
            }
            else {
                // quadro no lugar da foto que nao foi encontrada
                al_draw_filled_rectangle(20, 90, 320, 450, al_map_rgb(40, 50, 90));
                al_draw_text(fonte, branco, 170, 260, ALLEGRO_ALIGN_CENTRE, "Imagem nao encontrada:");
                al_draw_text(fonte, amarelo, 170, 280, ALLEGRO_ALIGN_CENTRE, arquivos[pergunta]);
            }
            al_draw_rectangle(20, 90, 320, 450, branco, 2);

            // mapa
            for (int i = 0; i < TOTAL; i++) {
                int x = MAPA_X + coluna[i] * (BLOCO_L + 6);
                int y = MAPA_Y + linha[i] * (BLOCO_A + 6);

                ALLEGRO_COLOR cor = al_map_rgb(70, 150, 80);
                if (escolhido != -1 && i == respostas[pergunta]) {
                    cor = amarelo;
                }
                else if (i == escolhido) {
                    cor = al_map_rgb(210, 60, 50);
                }

                al_draw_filled_rectangle(x, y, x + BLOCO_L, y + BLOCO_A, cor);
                al_draw_rectangle(x, y, x + BLOCO_L, y + BLOCO_A, preto, 2);
                al_draw_text(fonte, preto, x + BLOCO_L / 2, y + BLOCO_A / 2 - 4, ALLEGRO_ALIGN_CENTRE, siglas[i]);
            }

            if (dica) {
                al_draw_text(fonte, amarelo, 20, 480, 0, dicas[pergunta]);
            }

            if (escolhido == -1) {
                al_draw_text(fonte, branco, 20, 505, 0, "Clique em um estado no mapa...");
            }
            else {
                al_draw_textf(fonte, branco, 20, 505, 0, "Voce escolheu: %s", nomes[escolhido]);
                if (escolhido == respostas[pergunta]) {
                    al_draw_textf(fonte, verde_claro, 20, 525, 0,
                        "ACERTOU! +%d pontos. Parabens, resposta certa: %s.",
                        PONTOS_POR_ACERTO, nomes[respostas[pergunta]]);
                }
                else {
                    al_draw_textf(fonte, vermelho_claro, 20, 525, 0,
                        "ERROU! A resposta certa era %s (em amarelo no mapa).",
                        nomes[respostas[pergunta]]);
                }
                al_draw_text(fonte, branco, 20, 545, 0, curiosidades[pergunta]);

                if (pergunta < TOTAL_PERGUNTAS - 1) {
                    al_draw_text(fonte, amarelo, 20, 570, 0, "Aperte ENTER para a proxima pergunta.");
                }
                else {
                    al_draw_text(fonte, amarelo, 20, 570, 0, "Aperte ENTER para ver o resultado.");
                }
            }
        }

        al_flip_display();

        // espera o jogador fazer alguma coisa
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = 0;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            int tecla = evento.keyboard.keycode;

            if (tecla == ALLEGRO_KEY_ESCAPE) {
                rodando = 0;
            }
            else if (tecla == ALLEGRO_KEY_D && !fim_de_jogo) {
                dica = 1;
            }
            else if (tecla == ALLEGRO_KEY_R ||
                     (fim_de_jogo && (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER))) {
                // reinicia o jogo inteiro: volta para a primeira pergunta e zera tudo
                pergunta = 0;
                pontos = 0;
                acertos = 0;
                for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
                    acertou[p] = 0;
                }
                escolhido = -1;
                dica = 0;
                fim_de_jogo = 0;
            }
            else if ((tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER) && escolhido != -1) {
                // so passa de pergunta depois que o jogador ja respondeu
                if (pergunta < TOTAL_PERGUNTAS - 1) {
                    pergunta++;      // vai para a proxima pergunta
                    escolhido = -1;
                    dica = 0;
                }
                else {
                    fim_de_jogo = 1; // era a ultima: mostra a tela final
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && escolhido == -1 && !fim_de_jogo) {
            // ve em qual bloco o mouse clicou
            for (int i = 0; i < TOTAL; i++) {
                int x = MAPA_X + coluna[i] * (BLOCO_L + 6);
                int y = MAPA_Y + linha[i] * (BLOCO_A + 6);
                if (evento.mouse.x >= x && evento.mouse.x <= x + BLOCO_L &&
                    evento.mouse.y >= y && evento.mouse.y <= y + BLOCO_A) {
                    escolhido = i;
                }
            }

            // se clicou em um estado, confere a resposta e soma os pontos
            if (escolhido != -1 && escolhido == respostas[pergunta]) {
                pontos = pontos + PONTOS_POR_ACERTO;
                acertos++;
                acertou[pergunta] = 1;
            }
        }
    }

    for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
        if (fotos[p] != NULL) {
            al_destroy_bitmap(fotos[p]);
        }
    }
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_display(janela);
    return 0;
}
