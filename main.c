#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define LARGURA 880
#define ALTURA 600
#define TOTAL 27

// Mapaa
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
    ALLEGRO_BITMAP *foto = al_load_bitmap("cristo.png");

    if (foto == NULL) {
        printf("Nao achei o arquivo cristo.png\n");
        return 1;
    }

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

    int resposta = 24;  // RJ
    int escolhido = -1; // -1 = ainda nao clicou
    int dica = 0;
    int rodando = 1;

    while (rodando) {
        // desenha a tela
        ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
        ALLEGRO_COLOR preto = al_map_rgb(0, 0, 0);
        ALLEGRO_COLOR amarelo = al_map_rgb(240, 200, 40);

        al_clear_to_color(al_map_rgb(20, 30, 60));

        al_draw_text(fonte, amarelo, LARGURA / 2, 15, ALLEGRO_ALIGN_CENTRE, "ONDE FICA?");
        al_draw_text(fonte, branco, LARGURA / 2, 35, ALLEGRO_ALIGN_CENTRE,
            "Clique no estado onde fica o monumento. D = dica, R = reiniciar, ESC = sair");

        // foto do Cristo Redentor
        al_draw_text(fonte, branco, 170, 70, ALLEGRO_ALIGN_CENTRE, "Cristo Redentor");
        al_draw_bitmap(foto, 20, 90, 0);
        al_draw_rectangle(20, 90, 320, 450, branco, 2);

        // mapa
        for (int i = 0; i < TOTAL; i++) {
            int x = MAPA_X + coluna[i] * (BLOCO_L + 6);
            int y = MAPA_Y + linha[i] * (BLOCO_A + 6);

            ALLEGRO_COLOR cor = al_map_rgb(70, 150, 80);
            if (escolhido != -1 && i == resposta) {
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
            al_draw_text(fonte, amarelo, 20, 480, 0, "DICA: fica na regiao Sudeste e tem litoral.");
        }

        if (escolhido == -1) {
            al_draw_text(fonte, branco, 20, 505, 0, "Clique em um estado no mapa...");
        }
        else {
            al_draw_textf(fonte, branco, 20, 505, 0, "Voce escolheu: %s", nomes[escolhido]);
            if (escolhido == resposta) {
                al_draw_text(fonte, al_map_rgb(120, 230, 120), 20, 525, 0, "ACERTOU!");
            }
            else {
                al_draw_text(fonte, al_map_rgb(240, 110, 100), 20, 525, 0,
                    "ERROU! O certo e o Rio de Janeiro (em amarelo).");
            }
            al_draw_text(fonte, branco, 20, 550, 0, "Aperte R para jogar de novo.");
        }

        al_flip_display();

        // espera o jogador fazer alguma coisa
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = 0;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                rodando = 0;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_D) {
                dica = 1;
            }
            else if (evento.keyboard.keycode == ALLEGRO_KEY_R) {
                escolhido = -1;
                dica = 0;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && escolhido == -1) {
            // ve em qual bloco o mouse clicou
            for (int i = 0; i < TOTAL; i++) {
                int x = MAPA_X + coluna[i] * (BLOCO_L + 6);
                int y = MAPA_Y + linha[i] * (BLOCO_A + 6);
                if (evento.mouse.x >= x && evento.mouse.x <= x + BLOCO_L &&
                    evento.mouse.y >= y && evento.mouse.y <= y + BLOCO_A) {
                    escolhido = i;
                }
            }
        }
    }

    al_destroy_bitmap(foto);
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_display(janela);
    return 0;
}
