Trabalho Prático 2

Redimensionamento de Imagens Sensível ao Conteúdo

Programação de Software Básico

05/2018


1 Introdução

Imagine que desejamos exibir uma imagem grande em um dispositivo com a tela mais estreita, por exemplo, um celular. Uma forma de fazer isso é redimensionar toda a imagem, mas ao fazer isso deixamos de usar parte da tela, pois precisamos preservar as proporções. Outra forma seria redimensionar somente a largura da imagem - só que assim a imagem original ficaria distorcida.

Uma técnica publicada em um artigo no SIGGRAPH 2007 por Shai Avidan e Ariel Shamir tenta resolver esse problema: ela consiste essencialmente em remover as partes menos importantes da imagem. Na figura 1, a imagem original (a) tem a sua largura alterada (b) e sofre a aplicação do algoritmo (c): observe como em (c) o pássaro não sofre nenhuma distorção.

Seam carving é uma técnica sensível ao contexto para redimensionamento de imagens, onde a imagem é reduzida de um pixel de largura (ou altura) por vez. Um seam vertical (“costura”) é um caminho de pixels conectado do topo à base com apenas um pixel em cada linha. Diferentemente de técnicas tradicionais de redimensionamento (por exemplo, recortar um pedaço da imagem ou mudar a escala inteira), esta técnica preserva melhor as características mais interessantes (ou relevantes) da imagem.

Veja na figura 2 o algoritmo em funcionamento: a linha vermelha corresponde ao melhor caminho encontrado a cada momento. Observe que a cada iteração, o algoritmo retira efetivamente uma coluna da imagem, mas em posições diferentes por linha. O algoritmo termina quando a largura desejada é atingida.

O algoritmo consiste em algumas etapas:

1. Calcular a energia de cada pixel;
2. Construir uma matriz com o custo por linha, acumulando a energia de cima para baixo;
3. Encontrar e remover o caminho com menor energia total acumulada, isto é, remover um pixel de cada linha, na coluna correspondente;
4. Repetir as etapas 1-3 até atingir a largura desejada.

