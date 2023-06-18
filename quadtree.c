#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;
int originalWidth, originalHeight;

QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}

QuadNode* geraQuadtree(Img* pic, float minError)
{
    // Converte o vetor RGBPixel para uma MATRIZ que pode acessada por pixels[linha][coluna]
    RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img;

    // Veja como acessar os primeiros 10 pixels da imagem, por exemplo:
    // int i;
    // for(i=0; i<10; i++)
    //     printf("%02X %02X %02X\n",pixels[0][i].r,pixels[1][i].g,pixels[2][i].b);

    int width = pic->width;
    int height = pic->height;

    //////////////////////////////////////////////////////////////////////////
    // Implemente aqui o algoritmo que gera a quadtree, retornando o nodo raiz
    //////////////////////////////////////////////////////////////////////////

    originalWidth = width;
    originalHeight = height;

    QuadNode* raizInicial = newNode(0, 0, width, height);

    recursiveQuadtree(0, 0, width, height, raizInicial, pixels, minError);

    drawTree(raizInicial);
    writeTree(raizInicial);

    return raizInicial;
// COMENTE a linha abaixo quando seu algoritmo ja estiver funcionando
// Caso contrario, ele ira gerar uma arvore de teste com 3 nodos

#define DEMO
#ifdef DEMO

    /************************************************************/
    /* Teste: criando uma raiz e dois nodos a mais              */
    /************************************************************/

    // QuadNode* raiz = newNode(0,0,width,height);
    // raiz->status = PARCIAL;
    // raiz->color[0] = 0;
    // raiz->color[1] = 0;
    // raiz->color[2] = 255;

    // int meiaLargura = width/2;
    // int meiaAltura = height/2;

    // QuadNode* nw = newNode(meiaLargura, 0, meiaLargura, meiaAltura);
    // nw->status = PARCIAL;
    // nw->color[0] = 0;
    // nw->color[1] = 0;
    // nw->color[2] = 255;

    // // Aponta da raiz para o nodo nw
    // raiz->NW = nw;

    // QuadNode* nw2 = newNode(meiaLargura+meiaLargura/2, 0, meiaLargura/2, meiaAltura/2);
    // nw2->status = CHEIO;
    // nw2->color[0] = 255;
    // nw2->color[1] = 0;
    // nw2->color[2] = 0;

    // // Aponta do nodo nw para o nodo nw2
    // nw->NW = nw2;

#endif
    // Finalmente, retorna a raiz da árvore
    // return raiz;
}

void recursiveQuadtree(int x, int y, int width, int height, QuadNode* raiz, RGBPixel (*pixels)[width], int minError)
{
    // Calcula o erro da região
    double erroRegiao = calculaErroRegiao(x, y, width, height, pixels);

    int* coresMedias = calculaCoresMedias(x, y, width, height, pixels);

    if (width <= 1 || height <= 1) {
        raiz->status = CHEIO;
        raiz->color[0] = coresMedias[0];
        raiz->color[1] = coresMedias[1];
        raiz->color[2] = coresMedias[2];
        return;
    }

    int meiaLargura = width/2;
    int meiaAltura = height/2;


    if (erroRegiao > minError) {
        QuadNode* nw = newNode(x, y, meiaLargura, meiaAltura);
        QuadNode* ne = newNode(meiaLargura, y, meiaLargura, meiaAltura);
        QuadNode* sw = newNode(x, meiaAltura, meiaLargura, meiaAltura);
        QuadNode* se = newNode(meiaLargura, meiaAltura, meiaLargura, meiaAltura);
    
        raiz->status = PARCIAL;
        raiz->NW = nw;
        raiz->NE = ne;
        raiz->SW = sw;
        raiz->SE = se;

        recursiveQuadtree(x, y, meiaLargura, meiaAltura, nw, pixels, minError);
        recursiveQuadtree(meiaLargura, y, meiaLargura, meiaAltura, ne, pixels, minError);
        recursiveQuadtree(x, meiaAltura, meiaLargura, meiaAltura, sw, pixels, minError);
        recursiveQuadtree(meiaLargura, meiaAltura, meiaLargura, meiaAltura, se, pixels, minError);
        return;
    } else {
        raiz->status = CHEIO;
        raiz->color[0] = coresMedias[0];
        raiz->color[1] = coresMedias[1];
        raiz->color[2] = coresMedias[2];
        return;
    }

    return;

}

double calculaErroRegiao(int x, int y, int width, int height, RGBPixel (*pixels)[width]) {

    // Criando matriz com tamanho variável (Tamanho da imagem)
    int **gray = (int **)malloc(originalWidth * sizeof(int*));
    for (int i=0; i<originalWidth; i++) {
        gray[i] = (int *)malloc(originalHeight * sizeof(int));
    }

    // Criação do histograma
    int histograma[256] = {0};

    // Preenchendo a matriz com as cores da região
    for (int i = 0; i < originalWidth; i++) {         // width + x  NÃO TESTADO
        for (int j = 0; j < originalHeight; j++) {    // height + y
            gray[i][j] = (0.3 * pixels[i][j].r) + (0.59 * pixels[i][j].g) + (0.11 * pixels[i][j].b);
        }
    }

    // Preenchendo o histograma com valores
    for (int i = x; i < width + x; i++) {
        for (int j = y; j< height + y; j++) {
            histograma[gray[i][j]]++;
        }
    }

    // Teste de Histograma
    // for (int i=0; i<256; i++) {
    //     if (i==0 || i==254 || i==255) {
    //         printf("Valor do I: %d || Histograma: %d\n", i, histograma[i]);
    //     }
    // }

    // Total de pixeis
    int totalPixeisRegiao = width * height;

    // Soma das intensidades
    int somaIntensidades = 0;
    for (int i=0; i<256; i++) {
        somaIntensidades += i * histograma[i];
    }

    // Media das intensidades
    int mediaIntensidades = somaIntensidades / totalPixeisRegiao;

    // printf("Total de pixels: %d\n", totalPixeisRegiao);
    // printf("Soma das intensidades: %d\n", somaIntensidades);
    // printf("Media das intensidades: %d\n", mediaIntensidades);

    // Soma da diferença da intensidade - intensidade média
    int count = 0;
    int a = 0;
    for (int i=x; i < width + x; i++) {
        for (int j=y; j < height + y; j++) {
            int intensidade = gray[i][j];
            a += abs(intensidade-mediaIntensidades);
            count++;
        }
    }
    // printf("Soma da diferença da intensidade - intensidade média: %d\n", a);

    // Erro da região
    int erro = a / totalPixeisRegiao;
    printf("Erro da Região: %d\n", erro);

    // Liberando a matriz da memória
    free(gray);
    return erro;
}

int* calculaCoresMedias(int x, int y, int width, int height, RGBPixel (*pixels)[width]) {
    int somaRed = 0, somaGreen = 0, somaBlue = 0;

    // Calcula a soma das componentes R, G e B
    for (int i = x; i < width + x; i++) {
        for (int j = y; j < height + y; j++) {
            somaRed += pixels[i][j].r;
            somaGreen += pixels[i][j].g;
            somaBlue += pixels[i][j].b;
        }
    }

    int totalPixels = width * height;

    int mediaRed = somaRed / totalPixels;
    int mediaGreen = somaGreen / totalPixels;
    int mediaBlue = somaBlue / totalPixels;

    static int corMedia[3] = {0};
    corMedia[0] = mediaRed;
    corMedia[1] = mediaGreen;
    corMedia[2] = mediaBlue;
    
    // Teste de cores médias
    // for(int i=0; i<3; i++) {
    //     printf("Cor média: %d\n", corMedia[i]);
    // }

    return corMedia;
}

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

