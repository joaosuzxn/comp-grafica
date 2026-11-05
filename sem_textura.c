/* ANTES: um bloco de grama com cor sólida + brilho por face.
   gcc sem_textura.c -o sem_textura -lGL -lGLU -lglut */
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float angulo = 0;

/* 6 faces, 4 vértices cada (base-esq, base-dir, topo-dir, topo-esq) */
float v[6][4][3] = {
    {{-1, 1, 1},{ 1, 1, 1},{ 1, 1,-1},{-1, 1,-1}},   /* topo     */
    {{-1,-1,-1},{ 1,-1,-1},{ 1,-1, 1},{-1,-1, 1}},   /* base     */
    {{-1,-1, 1},{ 1,-1, 1},{ 1, 1, 1},{-1, 1, 1}},   /* frente   */
    {{ 1,-1,-1},{-1,-1,-1},{-1, 1,-1},{ 1, 1,-1}},   /* trás     */
    {{ 1,-1, 1},{ 1,-1,-1},{ 1, 1,-1},{ 1, 1, 1}},   /* direita  */
    {{-1,-1,-1},{-1,-1, 1},{-1, 1, 1},{-1, 1,-1}}    /* esquerda */
};
float brilho[6] = {1.0, 0.5, 0.8, 0.8, 0.65, 0.65};

void display(void) {
    int f, i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(3, 3, 5, 0, 0, 0, 0, 1, 0);
    glRotatef(angulo, 0, 1, 0);

    glBegin(GL_QUADS);
    for (f = 0; f < 6; f++) {
        float b = brilho[f];
        if (f == 0) glColor3f(0.35 * b, 0.62 * b, 0.22 * b);  /* topo: verde  */
        else        glColor3f(0.49 * b, 0.33 * b, 0.22 * b);  /* resto: terra */
        for (i = 0; i < 4; i++) glVertex3fv(v[f][i]);
    }
    glEnd();
    glutSwapBuffers();
}

void timer(int x) { angulo += 0.5; glutPostRedisplay(); glutTimerFunc(16, timer, 0); }

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Antes: cor solida");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.55, 0.75, 0.95, 1);
    glMatrixMode(GL_PROJECTION); gluPerspective(45, 1, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glutDisplayFunc(display);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
