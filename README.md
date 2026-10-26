# Onde Fica??? — POC

Prova de conceito (POC) de um jogo educacional feito em **C** com **Allegro 5** no **Visual Studio**.

O jogo mostra um monumento famoso do Brasil e um mapa. O jogador clica no estado onde acredita que o monumento fica. O conteúdo trabalha localização dos estados, regiões brasileiras e patrimônio cultural.

Esta POC tem apenas uma rodada: o **Cristo Redentor**, que fica no estado do **Rio de Janeiro**, na região Sudeste.

## Controles

| Ação | Tecla / mouse |
|---|---|
| Escolher um estado | Clique com o botão esquerdo no mapa |
| Mostrar dica | `D` |
| Reiniciar a rodada | `R` |
| Sair | `Esc` |

## Como funciona

- **Mapa esquemático:** os 26 estados e o Distrito Federal são retângulos com suas siglas, organizados aproximadamente pela posição geográfica. Os blocos **não** representam as fronteiras reais.
- **Detecção do clique:** a posição de cada bloco é calculada a partir da coluna e da linha do estado. Um laço `for` verifica se o mouse está dentro de algum bloco; se estiver, aquele é o estado escolhido.
- **Resultado:** depois do clique, o jogo mostra o estado escolhido e se está certo. O Rio de Janeiro fica em amarelo e uma escolha errada fica em vermelho.
- **Monumento:** o jogo mostra uma foto do Cristo Redentor (`cristo.png`, na mesma pasta do `main.c`). Para rodar o `.exe` direto, a foto precisa estar ao lado do executável.

Todo o código está em um único arquivo, `main.c`.

## Como executar

Requisitos: Visual Studio 2022 com a carga de trabalho **Desenvolvimento para desktop com C++**.

1. Clone o repositório e abra `JogoAllegro.sln`.
2. Compile com **F5**. O Visual Studio baixa automaticamente o pacote NuGet do Allegro (listado em `packages.config`).
   - Se não baixar, clique com o botão direito na solução → **Restaurar Pacotes NuGet**.
3. Os addons usados já estão ativados no projeto (**Propriedades → Allegro 5 → Add-ons**): **Font**, **Primitives** e **Image**.

## Crédito da imagem

Foto do Cristo Redentor: "Christ on Corcovado mountain", de Artyominc, [Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Christ_on_Corcovado_mountain.JPG), licença [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/). A imagem foi recortada.

## Limites desta POC

Uma única rodada, sem sons, animações, menus, ranking ou cronômetro. Os textos na tela estão sem acentos para evitar problemas de codificação com a fonte embutida do Allegro.
