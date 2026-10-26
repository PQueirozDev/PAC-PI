# Onde Fica?

Jogo educacional feito em **C** com **Allegro 5** no **Visual Studio**.

O jogo mostra um monumento famoso do Brasil e um mapa. O jogador clica no estado onde acredita que o monumento fica. O conteúdo trabalha localização dos estados, regiões brasileiras e patrimônio cultural.

## Perguntas

O jogo tem **3 perguntas**, uma para cada monumento:

| # | Monumento | Estado (resposta) | Região |
|---|---|---|---|
| 1 | Cristo Redentor | Rio de Janeiro (RJ) | Sudeste |
| 2 | Teatro Amazonas | Amazonas (AM) | Norte |
| 3 | Congresso Nacional | Distrito Federal (DF) | Centro-Oeste |

## Funcionalidades

- **Três perguntas em sequência**, com a foto e o nome de cada monumento.
- **Pontuação:** cada acerto vale **10 pontos** (máximo de 30). O placar aparece no canto superior direito.
- **Mensagens de acerto e erro:** ao acertar aparece "ACERTOU! +10 pontos"; ao errar aparece qual era a resposta certa. Nos dois casos o jogo mostra uma curiosidade sobre o monumento.
- **Dica:** a tecla `D` mostra uma dica diferente para cada pergunta.
- **Destaque do mouse:** o estado embaixo do mouse fica verde-claro antes do clique.
- **Cores no mapa depois da resposta:** o estado certo fica em **amarelo** e uma escolha errada fica em **vermelho**.
- **Tela final:** mostra a pontuação, a quantidade de acertos, o resultado de cada pergunta e uma mensagem de acordo com o desempenho.
- **Reiniciar:** na tela final, `R` ou `Enter` começam um jogo novo do zero.

## Controles

| Ação | Tecla / mouse |
|---|---|
| Escolher um estado | Clique com o botão esquerdo no mapa |
| Mostrar dica | `D` |
| Próxima pergunta (depois de responder) | `Enter` |
| Reiniciar o jogo (volta à pergunta 1 e zera os pontos) | `R` |
| Jogar de novo na tela final | `R` ou `Enter` |
| Sair | `Esc` |

## Como funciona

- **Vetores das perguntas:** cada pergunta ocupa a mesma posição em vários vetores (`monumentos`, `arquivos`, `respostas`, `dicas`, `curiosidades`). A variável `pergunta` diz qual posição está na tela.
- **Mapa esquemático:** os 26 estados e o Distrito Federal são retângulos com suas siglas, organizados aproximadamente pela posição geográfica. Os blocos **não** representam as fronteiras reais.
- **Detecção do clique:** a posição de cada bloco é calculada a partir da coluna e da linha do estado. Um laço `for` verifica se o mouse está dentro de algum bloco; se estiver, aquele é o estado escolhido.
- **Pontuação:** se o estado clicado for igual à resposta da pergunta, o jogo soma 10 em `pontos` e 1 em `acertos`.
- **Fotos:** cada monumento tem um arquivo de imagem na mesma pasta do `main.c`. Se uma foto não for encontrada, o jogo **não fecha**: ele mostra um quadro com o nome do arquivo que falta.

Todo o código está em um único arquivo, `main.c`. A explicação detalhada para a apresentação está em [`EXPLICACAO.md`](EXPLICACAO.md).

## Como executar

Requisitos: Visual Studio 2022 com a carga de trabalho **Desenvolvimento para desktop com C++**.

1. Clone o repositório e abra `JogoAllegro.sln`.
2. Compile com **F5**. O Visual Studio baixa automaticamente o pacote NuGet do Allegro (listado em `packages.config`).
   - Se não baixar, clique com o botão direito na solução → **Restaurar Pacotes NuGet**.
3. Os addons usados já estão ativados no projeto (**Propriedades → Allegro 5 → Add-ons**): **Font**, **Primitives** e **Image**.

## Fotos dos monumentos

| Arquivo | Monumento | Situação |
|---|---|---|
| `cristo.png` | Cristo Redentor | Incluída no projeto |
| `teatro.png` | Teatro Amazonas | Adicionar |
| `congresso.png` | Congresso Nacional | Adicionar |

Para adicionar uma foto nova:

1. Baixe uma foto de uso livre (por exemplo, do [Wikimedia Commons](https://commons.wikimedia.org)).
2. Recorte/redimensione para **300 × 360 pixels** (mesmo tamanho do `cristo.png`).
3. Salve como PNG com o nome da tabela acima, na mesma pasta do `main.c`.
4. Para rodar o `.exe` direto (fora do Visual Studio), as fotos precisam estar ao lado do executável.
5. Anote o crédito da foto na seção abaixo.

## Crédito das imagens

Foto do Cristo Redentor: "Christ on Corcovado mountain", de Artyominc, [Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Christ_on_Corcovado_mountain.JPG), licença [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/). A imagem foi recortada.

## Limites

Três perguntas fixas, sem sons, animações, menus, ranking ou cronômetro. Os textos na tela estão sem acentos para evitar problemas de codificação com a fonte embutida do Allegro.
