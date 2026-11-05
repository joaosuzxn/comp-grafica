/* TRANSPARÊNCIA: folha (recorte), vidro (mistura) e uma pedra atrás do vidro.
   Precisa de atlas.ppm (python3 gerar_atlas.py).
   gcc com_transparencia.c -o com_transparencia -lGL -lGLU -lglut
   Teclas: a liga/desliga o teste de alfa
           b liga/desliga a mistura
           e erro de ordem: vidro desenhado antes da pedra
           setas giram a cena */
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#define N 4
float angulo = 15;
int testeAlfa = 1, mistura = 1, erroOrdem = 0;

float v[6][4][3] = {
    {{-1, 1, 1},{ 1, 1, 1},{ 1, 1,-1},{-1, 1,-1}},
    {{-1,-1,-1},{ 1,-1,-1},{ 1,-1, 1},{-1,-1, 1}},
    {{-1,-1, 1},{ 1,-1, 1},{ 1, 1, 1},{-1, 1, 1}},
    {{ 1,-1,-1},{-1,-1,-1},{-1, 1,-1},{ 1, 1,-1}},
    {{ 1,-1, 1},{ 1,-1,-1},{ 1, 1,-1},{ 1, 1, 1}},
    {{-1,-1,-1},{-1,-1, 1},{-1, 1, 1},{-1, 1,-1}}
};
float brilho[6] = {1.0, 0.5, 0.8, 0.8, 0.65, 0.65};

/* 1. Ler o PPM (igual ao com_textura.c) */
unsigned char *lerPPM(const char *nome, int *l, int *a) {
    FILE *f = fopen(nome, "rb");
    int max;
    if (!f) { printf("Rode antes: python3 gerar_atlas.py\n"); exit(1); }
    fscanf(f, "P6 %d %d %d", l, a, &max);
    fgetc(f);
    unsigned char *rgb = malloc(3 * *l * *a);
    fread(rgb, 3, *l * *a, f);
    fclose(f);
    return rgb;
}

/* 2. RGB -> RGBA: aqui o alfa ganha função */
void carregaAtlas(void) {
    int l, a, i;
    unsigned char *rgb = lerPPM("atlas.ppm", &l, &a);
    unsigned char *rgba = malloc(4 * l * a);
    for (i = 0; i < l * a; i++) {
        unsigned char r = rgb[3*i], g = rgb[3*i+1], b = rgb[3*i+2];
        rgba[4*i] = r; rgba[4*i+1] = g; rgba[4*i+2] = b;
        rgba[4*i+3] = (r == 255 && g == 0 && b == 255) ? 0 : 255;   /* cor-chave -> alfa 0 */

        int celula = ((i / l) / (a / N)) * N + (i % l) / (l / N);
        if (celula == 7) rgba[4*i+3] = 100;                          /* vidro: alfa parcial */
    }
    GLuint atlas;
    glGenTextures(1, &atlas);
    glBindTexture(GL_TEXTURE_2D, atlas);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, l, a, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    free(rgb); free(rgba);
}

/* Um cubo inteiro com a mesma célula em todas as faces, na posição (x, z) */
void cubo(float x, float z, int celula) {
    int f, i;
    float lado = 1.0 / N;
    float s0 = (celula % N) * lado, t0 = (celula / N) * lado;
    float s[4] = {s0, s0 + lado, s0 + lado, s0};
    float t[4] = {t0 + lado, t0 + lado, t0, t0};
    glPushMatrix();
    glTranslatef(x, 0, z);
    glBegin(GL_QUADS);
    for (f = 0; f < 6; f++) {
        glColor3f(brilho[f], brilho[f], brilho[f]);
        for (i = 0; i < 4; i++) { glTexCoord2f(s[i], t[i]); glVertex3fv(v[f][i]); }
    }
    glEnd();
    glPopMatrix();
}

void desenhaVidro(void) {
    if (mistura) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   /* cor = a*nova + (1-a)*existente */
    }
    if (mistura && !erroOrdem) glDepthMask(GL_FALSE);        /* testa, mas não grava profundidade */
    cubo(1.3, 1.5, 7);                                       /* vidro */
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 4, 9, 0, 0, 0, 0, 1, 0);
    glRotatef(angulo, 0, 1, 0);

    if (erroOrdem) desenhaVidro();      /* ERRADO: vidro antes da pedra */

    /* Passada 1: opacos */
    cubo(1.3, -1.3, 3);                 /* pedra, atrás do vidro */

    /* Passada 2: recorte (teste de alfa) */
    if (testeAlfa) {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.5);   /* alfa 0 (magenta) é descartado */
    }
    cubo(-1.8, 0, 6);                   /* folha */
    glDisable(GL_ALPHA_TEST);           /* senão o vidro (alfa 0,39) também seria descartado */

    /* Passada 3: semitransparentes */
    if (!erroOrdem) desenhaVidro();

    glutSwapBuffers();
}

void teclado(unsigned char k, int x, int y) {
    char titulo[120];
    if (k == 'a') testeAlfa = !testeAlfa;
    if (k == 'b') mistura = !mistura;
    if (k == 'e') erroOrdem = !erroOrdem;
    if (k == 27) exit(0);
    sprintf(titulo, "teste de alfa: %s | mistura: %s | ordem: %s",
            testeAlfa ? "ON" : "OFF", mistura ? "ON" : "OFF", erroOrdem ? "ERRADA" : "certa");
    glutSetWindowTitle(titulo);
    glutPostRedisplay();
}

void setas(int k, int x, int y) {
    if (k == GLUT_KEY_LEFT)  angulo -= 5;
    if (k == GLUT_KEY_RIGHT) angulo += 5;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 600);
    glutCreateWindow("teste de alfa: ON | mistura: ON | ordem: certa");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.55, 0.75, 0.95, 1);
    glMatrixMode(GL_PROJECTION); gluPerspective(45, 700.0 / 600, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    carregaAtlas();
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(setas);
    glutMainLoop();
    return 0;
}
