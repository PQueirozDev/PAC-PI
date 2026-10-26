# Explicação do código — "Onde Fica?"

Este documento explica o jogo para a apresentação. Todo o código está no arquivo `main.c`.

---

## 1. Visão geral

O jogo funciona assim:

1. Mostra a foto de um monumento e um mapa do Brasil feito com retângulos.
2. O jogador clica no estado onde acha que o monumento fica.
3. O jogo diz se acertou ou errou e soma os pontos.
4. O jogador aperta `Enter` para ir à próxima pergunta.
5. Depois da 3ª pergunta aparece a tela final com o resultado.
6. Na tela final, `R` ou `Enter` começam tudo de novo.

---

## 2. O que cada parte do código faz

O `main.c` está dividido em blocos, na ordem em que aparecem:

| Parte | O que faz |
|---|---|
| `#include` | Traz as bibliotecas: `stdio.h` (para `printf` e `snprintf`) e os módulos do Allegro (janela, fonte, desenho de formas e imagens). |
| `#define` | Cria constantes: tamanho da janela, número de estados (`TOTAL`), número de perguntas (`TOTAL_PERGUNTAS`), pontos por acerto e o tamanho dos blocos do mapa. Se quiser mudar um valor, muda só aqui. |
| Função `escrever_grande` | Desenha um texto maior que o normal. A fonte do Allegro é pequena (8 pixels), então a função "estica" o desenho (escala 2, 3 ou 4) e depois volta ao tamanho normal. É usada no título, no nome do monumento e na tela final. |
| Início do `main` | Liga o Allegro (`al_init`), o teclado, o mouse e os addons. Cria a janela, a fila de eventos e a fonte. |
| Vetores do mapa | `siglas`, `nomes`, `coluna` e `linha`: guardam a sigla, o nome e a posição de cada um dos 27 blocos do mapa. |
| `PERGUNTAS` | Vetores com os dados de cada pergunta: nome do monumento, arquivo da foto, resposta certa, dica e curiosidade. |
| Carregar fotos | Um laço `for` carrega a foto de cada monumento. Se a foto não existir, o jogo avisa no console e continua. |
| `VARIAVEIS DO JOGO` | Guardam o estado do jogo: pergunta atual, pontos, acertos, estado escolhido, dica, tela final e posição do mouse. |
| `while (rodando)` | O laço principal. Cada volta desenha a tela e espera uma ação do jogador. |
| `CORES USADAS NO JOGO` | Cria as cores (branco, amarelo, verde, vermelho etc.) com `al_map_rgb(vermelho, verde, azul)`. |
| `TELA FINAL` | Desenhada quando `fim_de_jogo` vale 1: pontuação, acertos, resumo de cada pergunta e mensagem final. |
| `TELA DA PERGUNTA` | Barra de cima com placar, foto do monumento, mapa e painel de mensagens embaixo. |
| Tratamento de eventos | Depois de desenhar, `al_wait_for_event` espera o jogador. Um `if / else if` decide o que fazer: fechar, tecla apertada, mouse mexeu ou mouse clicou. |
| Final do `main` | Libera da memória as fotos, a fonte, a fila e a janela (`al_destroy_...`). |

---

## 3. Conceitos básicos de C usados

| Conceito | Onde aparece no jogo |
|---|---|
| **Variáveis `int`** | `pontos`, `acertos`, `pergunta`, `escolhido`, `dica`, `fim_de_jogo`, `mouse_x`, `mouse_y`. |
| **Constantes (`#define`)** | `TOTAL`, `TOTAL_PERGUNTAS`, `PONTOS_POR_ACERTO`, `LARGURA`, `ALTURA`. |
| **Vetores** | `siglas[27]`, `nomes[27]`, `coluna[27]`, `linha[27]`, `monumentos[3]`, `respostas[3]`, `dicas[3]`, `acertou[3]`, `fotos[3]`. |
| **Vetores de texto (`char *`)** | Cada posição guarda um texto, por exemplo `nomes[24]` é `"Rio de Janeiro"`. |
| **Condicionais (`if / else if / else`)** | Verificar acerto/erro, escolher a cor do bloco, escolher a mensagem final, descobrir qual tecla foi apertada. |
| **Laço `while`** | O laço principal do jogo (`while (rodando)`), que só para quando `rodando` vira 0. |
| **Laço `for`** | Percorrer os 27 estados para desenhar o mapa e para descobrir onde o mouse clicou; carregar as 3 fotos; zerar o vetor `acertou` ao reiniciar. |
| **Operadores de comparação e lógicos** | `>=`, `<=`, `==`, `!=`, `&&` (e), `\|\|` (ou), `!` (não). |
| **Função** | `escrever_grande`, que recebe parâmetros e é chamada várias vezes. |
| **`printf` / `snprintf`** | `printf` escreve aviso no console; `snprintf` monta um texto com números (ex.: `"20 de 30 pontos"`). |
| **Ponteiros (uso simples)** | Os tipos do Allegro (`ALLEGRO_DISPLAY *`, `ALLEGRO_BITMAP *` etc.). Comparamos com `NULL` para saber se a foto carregou. |

---

## 4. Como funciona a pontuação

São duas variáveis começando em zero:

```c
int pontos = 0;  // +10 por acerto
int acertos = 0; // quantas perguntas acertou
```

Quando o jogador clica em um estado, o jogo compara com a resposta da pergunta atual:

```c
if (escolhido != -1 && escolhido == respostas[pergunta]) {
    pontos = pontos + PONTOS_POR_ACERTO; // soma 10
    acertos++;                            // soma 1
    acertou[pergunta] = 1;                // marca que acertou esta pergunta
}
```

- `escolhido != -1` garante que o clique foi em cima de um estado (e não no fundo da tela).
- Cada pergunta só pode ser respondida **uma vez**, porque o clique só é aceito quando `escolhido == -1`. Assim não dá para ganhar pontos clicando várias vezes.
- A pontuação máxima é `TOTAL_PERGUNTAS * PONTOS_POR_ACERTO` = 3 × 10 = **30**.
- O vetor `acertou` serve para a tela final mostrar `[OK]` ou `[X]` em cada monumento.

---

## 5. Como funciona a troca de perguntas

A variável `pergunta` guarda a **posição** da pergunta atual nos vetores (0, 1 ou 2). Como todos os vetores usam a mesma posição, basta mudar `pergunta` para trocar tudo de uma vez:

| `pergunta` | `monumentos[...]` | `arquivos[...]` | `respostas[...]` |
|---|---|---|---|
| 0 | Cristo Redentor | cristo.png | 24 (RJ) |
| 1 | Teatro Amazonas | teatro.png | 2 (AM) |
| 2 | Congresso Nacional | congresso.png | 16 (DF) |

Quando o jogador aperta `Enter` **depois de responder**:

```c
if (pergunta < TOTAL_PERGUNTAS - 1) {
    pergunta++;      // vai para a próxima pergunta
    escolhido = -1;  // limpa a escolha
    dica = 0;        // esconde a dica
}
else {
    fim_de_jogo = 1; // era a última: mostra a tela final
}
```

Para **reiniciar**, o jogo volta todas as variáveis ao valor inicial: `pergunta = 0`, `pontos = 0`, `acertos = 0`, `escolhido = -1`, `dica = 0`, `fim_de_jogo = 0`, e um `for` zera o vetor `acertou`.

**Para adicionar uma 4ª pergunta:** mudar `TOTAL_PERGUNTAS` para 4 e acrescentar um item em cada vetor de perguntas (monumento, arquivo, resposta, dica, curiosidade).

---

## 6. Como o mouse identifica o estado escolhido

Cada estado é um retângulo. A posição dele é calculada pela **coluna** e **linha**:

```c
int x = MAPA_X + coluna[i] * (BLOCO_L + 6);
int y = MAPA_Y + linha[i] * (BLOCO_A + 6);
```

- `MAPA_X` e `MAPA_Y` são o canto do mapa na tela.
- `BLOCO_L` e `BLOCO_A` são a largura e a altura de um bloco; o `+ 6` é o espaço entre blocos.

Exemplo com o Rio de Janeiro (posição 24, coluna 4, linha 5):
`x = 360 + 4 × 62 = 608` e `y = 90 + 5 × 46 = 320`. O bloco vai de (608, 320) até (664, 360).

Quando acontece um clique, um laço `for` testa os 27 blocos. O mouse está dentro de um bloco se estiver **entre a esquerda e a direita** e **entre o topo e a base** dele:

```c
if (evento.mouse.x >= x && evento.mouse.x <= x + BLOCO_L &&
    evento.mouse.y >= y && evento.mouse.y <= y + BLOCO_A) {
    escolhido = i;
}
```

Se nenhum bloco combinar, `escolhido` continua `-1` e nada acontece.

O **destaque verde-claro** usa a mesma conta: quando o mouse se mexe (evento `ALLEGRO_EVENT_MOUSE_AXES`), o jogo guarda a posição em `mouse_x` e `mouse_y` e pinta o bloco que está embaixo dele.

---

## 7. Possíveis perguntas do professor

**Por que vocês usaram vários vetores em vez de uma `struct`?**
Porque ainda estamos aprendendo o básico. Vetores paralelos (mesma posição = mesma pergunta) são simples de entender e funcionam bem para poucas perguntas.

**O que acontece se clicar fora do mapa?**
O `for` não encontra nenhum bloco, então `escolhido` continua `-1` e o jogo continua esperando.

**Dá para ganhar pontos clicando várias vezes na mesma pergunta?**
Não. O clique só é aceito enquanto `escolhido == -1`. Depois da primeira resposta, novos cliques são ignorados até a próxima pergunta.

**Como o jogo sabe que acabou?**
Quando o jogador aperta `Enter` na última pergunta (`pergunta == TOTAL_PERGUNTAS - 1`), a variável `fim_de_jogo` vira 1 e o laço passa a desenhar a tela final.

**O que é a fila de eventos?**
É onde o Allegro guarda tudo o que o jogador faz (teclas, cliques, mexer o mouse, fechar a janela). `al_wait_for_event` pega o próximo evento e o `if / else if` decide o que fazer.

**Por que os textos estão sem acento?**
A fonte embutida do Allegro não mostra bem os acentos, então os textos foram escritos sem eles para não aparecerem símbolos estranhos.

**O que acontece se uma foto não for encontrada?**
O jogo mostra um aviso no console e desenha um quadro com o nome do arquivo que falta, em vez de fechar.

**Por que usar `#define` para os pontos?**
Para não espalhar o número 10 pelo código. Se o professor pedir 5 pontos por acerto, basta mudar uma linha.

**Para que serve `al_destroy_...` no final?**
Para devolver ao sistema a memória usada pela janela, fonte, fila e fotos.

**Como aumentar o tamanho do texto se a fonte é pequena?**
A função `escrever_grande` usa uma transformação do Allegro que multiplica o tamanho do desenho (escala). Depois ela volta a escala para 1, para o resto da tela não ficar grande.

**O mapa é real?**
Não. É um mapa esquemático: cada estado é um retângulo colocado mais ou menos na posição dele no Brasil. As fronteiras não são reais.
