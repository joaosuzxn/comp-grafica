/* DEPOIS: o mesmo bloco, texturizado pelo atlas.
   Precisa de atlas.ppm (python3 gerar_atlas.py).
   gcc com_textura.c -o com_textura -lGL -lGLU -lglut
   Tecla f: alterna GL_NEAREST / GL_LINEAR */
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#define N 4                       /* atlas 4x4 células */
float angulo = 0;
int nearest = 1;

float v[6][4][3] = {
    {{-1, 1, 1},{ 1, 1, 1},{ 1, 1,-1},{-1, 1,-1}},   /* topo     */
    {{-1,-1,-1},{ 1,-1,-1},{ 1,-1, 1},{-1,-1, 1}},   /* base     */
    {{-1,-1, 1},{ 1,-1, 1},{ 1, 1, 1},{-1, 1, 1}},   /* frente   */
    {{ 1,-1,-1},{-1,-1,-1},{-1, 1,-1},{ 1, 1,-1}},   /* trás     */
    {{ 1,-1, 1},{ 1,-1,-1},{ 1, 1,-1},{ 1, 1, 1}},   /* direita  */
    {{-1,-1,-1},{-1,-1, 1},{-1, 1, 1},{-1, 1,-1}}    /* esquerda */
};
float brilho[6] = {1.0, 0.5, 0.8, 0.8, 0.65, 0.65};
int celula[6]   = {0, 2, 1, 1, 1, 1};   /* topo: grama, base: terra, laterais: grama lateral */

/* 1. Ler o PPM */
unsigned char *lerPPM(const char *nome, int *l, int *a) {
    FILE *f = fopen(nome, "rb");
    int max;
    if (!f) { printf("Rode antes: python3 gerar_atlas.py\n"); exit(1); }
    fscanf(f, "P6 %d %d %d", l, a, &max);
    fgetc(f);                               /* espaço após o cabeçalho */
    unsigned char *rgb = malloc(3 * *l * *a);
    fread(rgb, 3, *l * *a, f);
    fclose(f);
    return rgb;
}

/* 2. Enviar à placa de vídeo */
void carregaAtlas(void) {
    int l, a, i;
    unsigned char *rgb = lerPPM("atlas.ppm", &l, &a);
    unsigned char *rgba = malloc(4 * l * a);
    for (i = 0; i < l * a; i++) {           /* RGB -> RGBA, alfa opaco */
        rgba[4*i] = rgb[3*i]; rgba[4*i+1] = rgb[3*i+1]; rgba[4*i+2] = rgb[3*i+2];
        rgba[4*i+3] = 255;
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

/* 3. Desenhar cada face com a célula do atlas */
void display(void) {
    int f, i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(3, 3, 5, 0, 0, 0, 0, 1, 0);
    glRotatef(angulo, 0, 1, 0);

    glBegin(GL_QUADS);
    for (f = 0; f < 6; f++) {
        float lado = 1.0 / N;
        float s0 = (celula[f] % N) * lado;  /* coluna / N */
        float t0 = (celula[f] / N) * lado;  /* linha  / N */
        /* t trocado dentro da célula: a 1ª linha do arquivo cai em t = 0 */
        float s[4] = {s0, s0 + lado, s0 + lado, s0};
        float t[4] = {t0 + lado, t0 + lado, t0, t0};

        glColor3f(brilho[f], brilho[f], brilho[f]);   /* MODULATE: textura x brilho */
        for (i = 0; i < 4; i++) {
            glTexCoord2f(s[i], t[i]);
            glVertex3fv(v[f][i]);
        }
    }
    glEnd();
    glutSwapBuffers();
}

void teclado(unsigned char k, int x, int y) {
    if (k == 'f') {
        nearest = !nearest;
        GLint filtro = nearest ? GL_NEAREST : GL_LINEAR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtro);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtro);
        glutSetWindowTitle(nearest ? "Depois: GL_NEAREST" : "Depois: GL_LINEAR");
    }
    if (k == 27) exit(0);
}

void timer(int x) { angulo += 0.5; glutPostRedisplay(); glutTimerFunc(16, timer, 0); }

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Depois: GL_NEAREST");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.55, 0.75, 0.95, 1);
    glMatrixMode(GL_PROJECTION); gluPerspective(45, 1, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    carregaAtlas();                          /* depois de criar a janela */
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
