#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// Para usar strings

#ifdef WIN32
#include <windows.h>    // Apenas para Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>     // Funções da OpenGL
#include <GL/glu.h>    // Funções da GLU
#include <GL/glut.h>   // Funções da FreeGLUT
#endif

// SOIL é a biblioteca para leitura das imagens
#include "SOIL.h"

// Um pixel RGB (24 bits)
typedef struct {
    unsigned char r, g, b;
} RGB;

// Uma imagem RGB
typedef struct {
    int width, height;
    RGB* img;
} Img;

// Protótipos
void load(char* name, Img* pic);
void uploadTexture();

// Funções da interface gráfica e OpenGL
void init();
void draw();
void keyboard(unsigned char key, int x, int y);

// Funções criadas
int calcEnergy(int pxTop, int pxRig, int pxBot, int pxLef);
void removePixel(int line, int column);

// Largura e altura da janela
int width, height;

// Identificadores de textura
GLuint tex[3];

// As 3 imagens
Img pic[3];

// Imagem selecionada (0,1,2)
int sel;

// Largura desejada
int largura;

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("seamcarving [origem] [mascara] [largura]\n");
        printf("Origem é a imagem original, mascara é a máscara desejada e largura é a largura desejada\n");
        exit(1);
    }

	glutInit(&argc,argv);

	// Define do modo de operacao da GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// pic[0] -> imagem original
	// pic[1] -> máscara desejada
	// pic[2] -> resultado do algoritmo

	// Carrega as duas imagens
    load(argv[1], &pic[0]);
    load(argv[2], &pic[1]);

    // Seta a largura desejada
    largura = atoi(argv[3]);

    if (pic[0].width != pic[1].width || pic[0].height != pic[1].height)
    {
        printf("Imagem e máscara com dimensões diferentes!\n");
        exit(1);
    }

    if (largura < (pic[0].width / 2))
        printf("A largura desejada não pode ser menor que a metade da largura original");

    // A largura e altura da janela são calculadas de acordo com a maior
    // dimensão de cada imagem
    width = pic[0].width;
    height = pic[0].height;

    // A largura e altura da imagem de saída são iguais às da imagem original (1)
    pic[2].width  = pic[1].width;
    pic[2].height = pic[1].height;

	// Especifica o tamanho inicial em pixels da janela GLUT
	glutInitWindowSize(width, height);

	// Cria a janela passando como argumento o titulo da mesma
	glutCreateWindow("Seam Carving");

	// Registra a funcao callback de redesenho da janela de visualizacao
	glutDisplayFunc(draw);

	// Registra a funcao callback para tratamento das teclas ASCII
	glutKeyboardFunc (keyboard);

	// Cria texturas em memória a partir dos pixels das imagens
    tex[0] = SOIL_create_OGL_texture((unsigned char*) pic[0].img, pic[0].width, pic[0].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
    tex[1] = SOIL_create_OGL_texture((unsigned char*) pic[1].img, pic[1].width, pic[1].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

    // Exibe as dimensões na tela, para conferência
    printf("Origem  : %s %d x %d\n", argv[1], pic[0].width, pic[0].height);
    printf("Destino : %s %d x %d\n", argv[2], pic[1].width, pic[0].height);
    sel = 0; // pic1

	// Define a janela de visualizacao 2D
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0,width,height,0.0);
	glMatrixMode(GL_MODELVIEW);

    // Aloca memória para a imagem de saída
	pic[2].img = malloc(pic[1].width * pic[1].height * 3); // W x H x 3 bytes (RGB)
	// Pinta a imagem resultante de preto!
	memset(pic[2].img, 0, width*height*3);

	for (int i = 0; i < pic[0].width * pic[0].height; i++)
    {
        pic[2].img[i].r = pic[0].img[i].r;
        pic[2].img[i].g = pic[0].img[i].g;
        pic[2].img[i].b = pic[0].img[i].b;
    }

    // Cria textura para a imagem de saída
	tex[2] = SOIL_create_OGL_texture((unsigned char*) pic[2].img, pic[2].width, pic[2].height, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	// Entra no loop de eventos, não retorna
    glutMainLoop();
}

// Carrega uma imagem para a struct Img
void load(char* name, Img* pic)
{
    int chan;

    pic->img = (RGB*) SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_RGB);

    if (!pic->img)
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
        exit(1);
    }

    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

// Gerencia eventos de teclado
void keyboard(unsigned char key, int x, int y)
{
    if (key==27)
    {
      // ESC: libera memória e finaliza
      free(pic[0].img);
      free(pic[1].img);
      free(pic[2].img);
      exit(1);
    }

    if (key >= '1' && key <= '3')
        // 1-3: seleciona a imagem correspondente (origem, máscara e resultado)
        sel = key - '1';

    if (key == 's') {
        // Aplica o algoritmo e gera a saida em pic[2].img...
        // ...
        // ... (crie uma função para isso!)

        // Exemplo: pintando tudo de amarelo
        for(int i=0; i<(pic[2].height*pic[2].width)/2; i++)
            pic[2].img[i].r = pic[2].img[i].g = 255;

        // Chame uploadTexture a cada vez que mudar
        // a imagem (pic[2])
        uploadTexture();
    }

    glutPostRedisplay();
}

// Faz upload da imagem para a textura,
// de forma a exibi-la na tela
void uploadTexture()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        pic[2].width, pic[2].height, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pic[2].img);
    glDisable(GL_TEXTURE_2D);
}

// Callback de redesenho da tela
void draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Preto
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Para outras cores, veja exemplos em /etc/X11/rgb.txt

    glColor3ub(255, 255, 255);  // branco

    // Ativa a textura corresponde à imagem desejada
    glBindTexture(GL_TEXTURE_2D, tex[sel]);
    // E desenha um retângulo que ocupa toda a tela
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glTexCoord2f(0,0);
    glVertex2f(0,0);

    glTexCoord2f(1,0);
    glVertex2f(pic[sel].width,0);

    glTexCoord2f(1,1);
    glVertex2f(pic[sel].width, pic[sel].height);

    glTexCoord2f(0,1);
    glVertex2f(0,pic[sel].height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Exibe a imagem
    glutSwapBuffers();
}

int calcEnergy(int pxTop, int pxRig, int pxBot, int pxLef)
{
    int Rx = pic[0].img[pxRig].r - pic[0].img[pxLef].r;
    int Gx = pic[0].img[pxRig].g - pic[0].img[pxLef].g;
    int Bx = pic[0].img[pxRig].b - pic[0].img[pxLef].b;

    int Ry = pic[0].img[pxBot].r - pic[0].img[pxTop].r;
    int Gy = pic[0].img[pxBot].g - pic[0].img[pxTop].g;
    int By = pic[0].img[pxBot].b - pic[0].img[pxTop].b;

    int DLTx = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
    int DLTy = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);

    return(DLTx + DLTy);
}

void removePixel(int line, int column)
{
    int pxPos = (line * pic[2].width) + column;
    int pxWid = (line + 1) * pic[2].width;

    pic[2].img[pxPos].r = pic[2].img[pxPos].g = pic[2].img[pxPos].b = 0;

    for (int i = pxPos; i < pxWid; i++)
    {
        pic[1].img[i].r = pic[2].img[i + 1].r;
        pic[1].img[i].g = pic[2].img[i + 1].g;
        pic[1].img[i].b = pic[2].img[i + 1].b;

        pic[2].img[i].r = pic[2].img[i + 1].r;
        pic[2].img[i].g = pic[2].img[i + 1].g;
        pic[2].img[i].b = pic[2].img[i + 1].b;
    }

    pic[2].img[pxWid].r = pic[2].img[pxWid].g = pic[2].img[pxWid].b = 0;
}

unsigned long long int smallest(unsigned long long int x, unsigned long long int y, unsigned long long int z)
{
    if (x < y && x < z)
        return x;

    else if (y < x && y < z)
        return y;

    return z;
}

unsigned long long int smallest2(unsigned long long int x, unsigned long long int y)
{
    if (x < y)
        return x;

    return y;
}

void seam()
{
    unsigned long long int mtxEnergy[pic[2].width - 1][pic[2].height - 1];

    int iTop = 0;
    int iRig = 0;
    int iBot = 0;
    int iLef = 0;

    for (int i = 0; i < (pic[2].width * pic[2].height); i++)
    {
        iTop = i - pic[2].width;
        iBot = i + pic[2].width;

        // Primeira linha e última linha
        if (i < pic[2].width)
            iTop = (pic[2].width * (pic[2].height - 1)) + i;
        else if (i >= pic[2].width * (pic[2].height - 1))
            iBot = i - pic[2].width * (pic[2].height - 1);

        iLef = i - 1;
        iRig = i + 1;

        // Primeira coluna e última coluna
        if (i % pic[2].width == 0)
            iLef = i + pic[2].width - 1;
        else if ((i + 1) % pic[2].width == 0)
            iRig = (i - pic[2].width) + 1;

        // Cálculo matriz de energia
        int line = (i - 1) / pic[2].width;
        int column = (i % pic[2].width);

        mtxEnergy[line][column] = calcEnergy(iTop, iRig, iBot, iLef);
    }

    unsigned long long int imgLef;
    unsigned long long int imgRig;
    unsigned long long int imgCen;

    // Percorre linhas
    for (int i = 1; i < pic[2].height; i++)
    {
        imgCen = mtxEnergy[i - 1][0];
        imgRig = mtxEnergy[i - 1][1];
        mtxEnergy[i][0] = mtxEnergy[i][0] + smallest2(imgCen, imgRig);
    }
}
