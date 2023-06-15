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

// MÉTOOD RECURSIVO QUE POSSUI OS BGLH DA REGIÃO
// SE O ERRO FOR MENOR
// PARA CADA NODO TU CHAMA DE NOVO O MÉTODO RECURSIVO

QuadNode* recursiveQuadrtree(int x, int y, int width, int height, QuadNode* raiz, GrayPixel (*graypixels)[width], RGBPixel (*pixels)[width], float error){
// Histograma da região
    int histogram[256] = {0};  // Inicializa o histograma com zeros

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int grayValue = graypixels[i][j].pixel;  // Valor de intensidade em escala de cinza
            histogram[grayValue]++;  // Incrementa a frequência do tom de cinza correspondente
        }
    }

    // int sum;
    // for (int i=0; i < 256; i++){
    //     printf("quantidade de pixeis com a itensidade igual a %d: %d\n", i, histogram[i]);
    //     sum += histogram[i];
    // }

    // printf("Total %d\n", pic->height * pic->width);
    // printf("Total %d\n", sum);

    int somaRed = 0, somaGreen = 0, somaBlue = 0;

    // Calcula a soma das componentes R, G e B
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            somaRed += pixels[i][j].r;
            somaGreen += pixels[i][j].g;
            somaBlue += pixels[i][j].b;
        }
    }

    int totalPixels = width * height;

    int mediaRed = somaRed / totalPixels;
    int mediaGreen = somaGreen / totalPixels;
    int mediaBlue = somaBlue / totalPixels;

    //printf("media dos pixeis: %d: red, %d: verde, %d: azul\n", mediaRed, mediaGreen, mediaBlue);

    int sum;
    // CALCULAR O ERRO
    for (int i=0; i<256; i++){
        sum += histogram[i] * i;
    }

    int intensidadeMedia = sum / (height * width);

    printf("%d", intensidadeMedia);

    double erroRegiao;
    for (int i=0; i<width; i++){
        for (int j=0; j<height; j++){
            double a = abs(graypixels[i][j].pixel - intensidadeMedia) * 1.0;
            double b = width * height * 1.0;

            erroRegiao += abs(graypixels[i][j].pixel - intensidadeMedia) * 1.0 / (width * height) * 1.0; //ao quadrado / width * height
           //printf("erro: %f\n", erroRegiao);
        }
    }

    if (erroRegiao < error) {
        int halfWidth = width / 2;
        int halfHeight = height / 2;

        QuadNode* ne = newNode(halfWidth, 0, halfWidth, halfHeight);
        QuadNode* nw = newNode(0, 0, halfWidth, halfHeight);
        QuadNode* se = newNode(0,0,width,height);
        QuadNode* sw = newNode(0,0,width,height);

        raiz->status = PARCIAL;
        raiz->color[0] = mediaRed;
        raiz->color[1] = mediaGreen;
        raiz->color[2] = mediaBlue;

        // Utilizando o drawNode, da pra calcular os méotods da regiao da raiz->NE e fazer ele desenhar
        drawNode(raiz->NE);

        // raiz->NE = recursiveQuadrtree(halfWidth, 0, halfWidth, halfHeight, raiz->NE, graypixels, pixels, error);
        // raiz->NW = recursiveQuadrtree(0, 0, halfWidth, halfHeight, raiz->NW, graypixels, pixels, error);
        // raiz->SE = recursiveQuadrtree(halfWidth, halfHeight, halfWidth, halfHeight, raiz->SE, graypixels, pixels, error);
        // raiz->SW = recursiveQuadrtree(0, halfHeight, halfWidth, halfHeight, raiz->SW, graypixels, pixels, error);
    } else {
        // raiz->status = CHEIO;
        // raiz->color[0] = mediaRed;
        // raiz->color[1] = mediaGreen;
        // raiz->color[2] = mediaBlue;

        // return raiz;
    }

    printf("retornou null\n");
    return raiz;
}


QuadNode* geraQuadtree(Img* pic, float minError)
{
    int width = pic->width;
    int height = pic->height;

    // Converte o vetor RGBPixel para uma MATRIZ que pode acessada por pixels[linha][coluna]
    RGBPixel (*pixels)[width] = (RGBPixel(*)[height]) pic->img;
    GrayPixel (*graypixels)[width] = (GrayPixel(*)[height]) pic->img;

    printf("Quantidade de linhas: %d\n", width);
    printf("Quantidade de colunas: %d\n", height);
    // Tentando colocar a imagem em tons de cinza.

    int i, j;
    for(i=0; i<width; i++)
         for (j=0; j<height; j++){
            graypixels[i][j].pixel = (0.3 * pixels[i][j].r) + (0.59 * pixels[i][j].g) + (0.11 * pixels[i][j].b);
            // printf("cor do pixel na linha %d e na coluna %d: %d\n", i, j, graypixels[i][j].pixel);
         } 

    QuadNode* raiz = newNode(0,0,width,height);

    QuadNode* raizFinal = recursiveQuadrtree(0, 0, width, height, raiz, graypixels, pixels, minError);
    return raiz;

    // CRIA METODO RECURSIVO

#define DEMO
#ifdef DEMO

    /************************************************************/
    /* Teste: criando uma raiz e dois nodos a mais              */
    /************************************************************/

    // Precisa-se testar e ver se conseguimos pegar a Média das cores, o histograma e calcular o nível de erro
    // De cada região, vendo se conseguimos dividir a imagem nisso, pq o fundo ta preto, não sei como fazer

    // QuadNode* raiz = newNode(0,0,width,height);
    // raiz->status = PARCIAL;
    // raiz->color[0] = 255;
    // raiz->color[1] = 0;
    // raiz->color[2] = 0;

    // int meiaLargura = width/2;
    // int meiaAltura = height/2;

    // QuadNode* nw = newNode(meiaLargura, 0, meiaLargura, meiaAltura);
    // nw->status = PARCIAL;
    // nw->color[0] = 255;
    // nw->color[1] = 0;
    // nw->color[2] = 0;

    // // Aponta da raiz para o nodo nw
    // raiz->NW = nw;

    // QuadNode* nw2 = newNode(meiaLargura+meiaLargura/2, 0, meiaLargura/2, meiaAltura/2);
    // nw2->status = CHEIO;
    // nw2->color[0] = 255;
    // nw2->color[1] = 255;
    // nw2->color[2] = 0;

    // // Aponta do nodo nw para o nodo nw2
    // nw->NW = nw2;

#endif
    // Finalmente, retorna a raiz da árvore
    // return raiz;
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

