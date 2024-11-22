



#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>


// Estructura para representar puntos en 2D.
struct Punto {
    float x, y;
};

std::vector<Punto> puntos_control;

int window_width = 1200, window_height = 600;
bool mostrar_polilinea = true, mostrar_curva = false, mostrar_puntos_control = true, mostrar_ejes = true;
bool mover_puntos = false, ingreso_puntos_activado = false;
int punto_seleccionado = -1;
bool seleccionando = false;
float altura_superficie = 0.2f; // Altura de la superficie reglada
GLfloat color_r = 0.0f, color_g = 0.0f, color_b = 1.0f;
float rotacionX = 0.0f;
float rotacionY = 0.0f;
float rotacionZ = 0.0f;
float grosor_curva = 1.0f; // Valor inicial del grosor de la curva
GLfloat color_cinta_poligonal[3] = { 1.0f, 0.0f, 0.0f }; // Color inicial: Rojo
GLfloat color_cinta_spline[3] = { 0.0f, 0.0f, 1.0f };    // Color inicial: Azul



void teclado(unsigned char key, int x, int y) {
    if (mover_puntos) return; // Solo permitir rotaciones si mover_puntos está activado
    switch (key) {
    case 'q': rotacionY += 2.0f; break; // Rotar sobre Y (horario)
    case 'Q': rotacionY += 2.0f; break;
    case 'w': rotacionY -= 2.0f; break; // Rotar sobre Y (antihorario)
    case 'W': rotacionY -= 2.0f; break;
    case 'z': rotacionZ += 2.0f; break;// Rotar sobre Z (horario)
    case 'Z': rotacionZ += 2.0f; break;
    case 'x': rotacionZ -= 2.0f; break;// Rotar sobre Z (antihorario)
    case 'X': rotacionZ -= 2.0f; break;
    case 'a': rotacionX += 2.0f; break;// Rotar sobre X (horario)
    case 'A': rotacionX += 2.0f; break;
    case 's': rotacionX -= 2.0f; break; // Rotar sobre X (antihorario)
    case 'S': rotacionX -= 2.0f; break;
    }

    // Redibujar la escena después de cada rotación
    glutPostRedisplay();
}

void dibujarCintaCatmull_Rom3D() {
    if (puntos_control.size() < 4) return;

    glColor3f(color_cinta_spline[0], color_cinta_spline[1], color_cinta_spline[2]);
    glBegin(GL_QUAD_STRIP);

    for (int i = 1; i < puntos_control.size() - 2; ++i) {
        for (float t = 0; t <= 1; t += 0.05f) {
            float t2 = t * t;
            float t3 = t2 * t;

            // Interpolación spline Catmull-Rom
            float x = 0.5f * ((2 * puntos_control[i].x) +
                (-puntos_control[i - 1].x + puntos_control[i + 1].x) * t +
                (2 * puntos_control[i - 1].x - 5 * puntos_control[i].x + 4 * puntos_control[i + 1].x - puntos_control[i + 2].x) * t2 +
                (-puntos_control[i - 1].x + 3 * puntos_control[i].x - 3 * puntos_control[i + 1].x + puntos_control[i + 2].x) * t3);

            float y = 0.5f * ((2 * puntos_control[i].y) +
                (-puntos_control[i - 1].y + puntos_control[i + 1].y) * t +
                (2 * puntos_control[i - 1].y - 5 * puntos_control[i].y + 4 * puntos_control[i + 1].y - puntos_control[i + 2].y) * t2 +
                (-puntos_control[i - 1].y + 3 * puntos_control[i].y - 3 * puntos_control[i + 1].y + puntos_control[i + 2].y) * t3);

            glVertex3f(x, y, 0.0f);
            glVertex3f(x, y, 0.5f); // Grosor de la cinta spline
        }
    }

    glEnd();
}


void dibujarCintaPoligonal3D() {
    if (puntos_control.size() < 2) return;

    float anchoCinta = 0.15f; // Ancho de la cinta
    float desplazamientoZ = 0.05f; // Incremento en la dirección Z

    // Dibuja la cinta con un QUAD_STRIP
    glColor3f(color_cinta_poligonal[0], color_cinta_poligonal[1], color_cinta_poligonal[2]);
    glBegin(GL_QUAD_STRIP);

    for (size_t i = 0; i < puntos_control.size() - 1; ++i) {
        Punto p1 = puntos_control[i];
        Punto p2 = puntos_control[i + 1];

        for (float z = 0.0f; z <= 1.0f; z += desplazamientoZ) {
            glVertex3f(p1.x - anchoCinta / 2, p1.y, z);
            glVertex3f(p2.x - anchoCinta / 2, p2.y, z);
            glVertex3f(p1.x + anchoCinta / 2, p1.y, z + desplazamientoZ);
            glVertex3f(p2.x + anchoCinta / 2, p2.y, z + desplazamientoZ);
        }
    }
    glEnd();

    // Dibujar bordes con un contorno negro
    glColor3f(0.0f, 0.0f, 0.0f); // Color negro para los bordes
    glLineWidth(1.0f); // Grosor del borde

    // Dibujar los bordes superiores de la cinta
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < puntos_control.size(); ++i) {
        glVertex3f(puntos_control[i].x - anchoCinta / 2, puntos_control[i].y, 0.0f);
    }
    glEnd();

    // Dibujar los bordes inferiores de la cinta
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < puntos_control.size(); ++i) {
        glVertex3f(puntos_control[i].x + anchoCinta / 2, puntos_control[i].y, 1.0f);
    }
    glEnd();
}
void dibujarEjes3D() {

    glBegin(GL_LINES);

    // Eje X en marrón
    glColor3f(0.6f, 0.3f, 0.0f); // Marrón
    glVertex3f(-1.5f, 0.0f, 0.0f);
    glVertex3f(1.5f, 0.0f, 0.0f);

    // Eje Y en negro
    glColor3f(0.0f, 0.0f, 0.0f); // Negro
    glVertex3f(0.0f, -1.5f, 0.0f);
    glVertex3f(0.0f, 1.5f, 0.0f);

    // Eje Z en púrpura vibrante
    glColor3f(0.6f, 0.0f, 1.0f); // Púrpura vibrante
    glVertex3f(0.0f, 0.0f, -1.5f);
    glVertex3f(0.0f, 0.0f, 1.5f);

    glEnd();
}


void dibujarEjes2D() {
    if (!mostrar_ejes) return;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
    glVertex2f(0.0f, -1.0f); glVertex2f(0.0f, 1.0f);
    glEnd();
}

void dibujarPolilinea() {
    if (!mostrar_polilinea || puntos_control.size() < 2) return;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : puntos_control) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void dibujarPuntosControl() {
    if (!mostrar_puntos_control) return;
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& p : puntos_control) {
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void dibujarCurvaCatmullRom() {
    if (!mostrar_curva || puntos_control.size() < 4) return;

    glLineWidth(grosor_curva); // Aplicar grosor a la curva
    glColor3f(color_r, color_g, color_b);
    glBegin(GL_LINE_STRIP);

    for (int i = 1; i < puntos_control.size() - 2; ++i) {
        for (float t = 0; t <= 1; t += 0.01f) {
            float t2 = t * t;
            float t3 = t2 * t;

            float p0x = puntos_control[i - 1].x;
            float p1x = puntos_control[i].x;
            float p2x = puntos_control[i + 1].x;
            float p3x = puntos_control[i + 2].x;

            float p0y = puntos_control[i - 1].y;
            float p1y = puntos_control[i].y;
            float p2y = puntos_control[i + 1].y;
            float p3y = puntos_control[i + 2].y;

            float x = 0.5f * ((2 * p1x) + (-p0x + p2x) * t +
                (2 * p0x - 5 * p1x + 4 * p2x - p3x) * t2 +
                (-p0x + 3 * p1x - 3 * p2x + p3x) * t3);

            float y = 0.5f * ((2 * p1y) + (-p0y + p2y) * t +
                (2 * p0y - 5 * p1y + 4 * p2y - p3y) * t2 +
                (-p0y + 3 * p1y - 3 * p2y + p3y) * t3);

            glVertex2f(x, y);
        }
    }

    glEnd();
    glLineWidth(1.0f); // Restaurar grosor predeterminado
}




void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Primer espacio: Trabajo en 2D
    glViewport(0, 0, window_width / 3, window_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    dibujarEjes2D();
    dibujarPuntosControl();
    dibujarPolilinea();
    dibujarCurvaCatmullRom();

    // Segundo espacio: Visualización 3D con rotaciones
    glViewport(window_width / 3, 0, window_width / 3, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)(window_width / 3) / window_height, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(3.0, 2.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Aplicar rotaciones también en el Espacio 2
    glRotatef(rotacionX, 1.0f, 0.0f, 0.0f); // Rotación en X
    glRotatef(rotacionY, 0.0f, 1.0f, 0.0f); // Rotación en Y
    glRotatef(rotacionZ, 0.0f, 0.0f, 1.0f); // Rotación en Z

    dibujarEjes3D();

    dibujarCintaPoligonal3D(); // Dibujar la cinta poligonal 3D en espacio 2
    glPopMatrix();

    // Tercer espacio: Visualización 3D con rotaciones
    glViewport(2 * window_width / 3, 0, window_width / 3, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Ajustar el campo de visión para aprovechar mejor el espacio
    gluPerspective(50.0, (float)(window_width / 3) / window_height, 0.1, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Ajustar la posición de la cámara para ampliar el contenido visible
    gluLookAt(2.5, 2.0, 4.5,  // Posición de la cámara (más cerca y centrada)
        0.0, 0.0, 0.0,  // Punto al que mira
        0.0, 1.0, 0.0); // Vector "up"

    // Aplicar rotaciones en el Espacio 3
    glRotatef(rotacionX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotacionY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotacionZ, 0.0f, 0.0f, 1.0f);

    dibujarEjes3D();
    dibujarCintaCatmull_Rom3D(); // Dibujar la cinta Cat en espacio 3
    glPopMatrix();


    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // Si estamos en el primer espacio (primer tercio horizontal)
        if (x < window_width / 3 && y >= 0 && y <= window_height) {
            glutAttachMenu(GLUT_RIGHT_BUTTON);  // Asociar el menú
        }
        else {
            glutDetachMenu(GLUT_RIGHT_BUTTON);  // Desasociar el menú
        }
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!ingreso_puntos_activado && !mover_puntos) return;  // Salir si no están activadas las funciones

        if (x < window_width / 3 && y >= 0 && y <= window_height) {
            float x_pos = (float)(x - (window_width / 6)) / (window_width / 6);
            float y_pos = -(float)(y - (window_height / 2)) / (window_height / 2);

            if (mover_puntos) {
                for (int i = 0; i < puntos_control.size(); ++i) {
                    float dx = puntos_control[i].x - x_pos;
                    float dy = puntos_control[i].y - y_pos;
                    if (dx * dx + dy * dy < 0.02f) {
                        seleccionando = true;
                        punto_seleccionado = i;
                        break;
                    }
                }
            }
            else if (ingreso_puntos_activado) {
                puntos_control.push_back({ x_pos, y_pos });
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        seleccionando = false;
        punto_seleccionado = -1;
    }

    glutPostRedisplay();
}


void motion(int x, int y) {
    if (x < window_width / 3 && y >= 0 && y <= window_height) {  // Restringir al primer tercio
        if (seleccionando && punto_seleccionado >= 0 && punto_seleccionado < puntos_control.size()) {
            float x_pos = (float)(x - (window_width / 6)) / (window_width / 6);
            float y_pos = -(float)(y - (window_height / 2)) / (window_height / 2);

            puntos_control[punto_seleccionado] = { x_pos, y_pos };
            glutPostRedisplay();
        }
    }
}



// Funciones de menú
void menu_curva_toggle(int opcion) {
    mostrar_curva = (opcion == 1);
    glutPostRedisplay();
}

void menu_color_curva(int opcion) {
    switch (opcion) {
    case 1: color_r = 1.0f; color_g = 0.0f; color_b = 0.0f; break; // Rojo
    case 2: color_r = 0.0f; color_g = 1.0f; color_b = 0.0f; break; // Verde
    case 3: color_r = 0.0f; color_g = 0.0f; color_b = 1.0f; break; // Azul
    case 4: color_r = 0.0f; color_g = 0.0f; color_b = 0.0f; break; // Negro
    }
    glutPostRedisplay();
}

void menu_toggle_puntos_control(int opcion) {
    mostrar_puntos_control = (opcion == 1);
    glutPostRedisplay();
}

void menu_toggle_ingreso(int opcion) {
    ingreso_puntos_activado = (opcion == 1);
    glutPostRedisplay();
}

void menu_toggle_mover(int opcion) {
    mover_puntos = (opcion == 1);
    glutPostRedisplay();
}
void menu_grosor_curva(int opcion) {
    switch (opcion) {
    case 1: grosor_curva = 1.0f; break; // Grosor fino
    case 2: grosor_curva = 3.0f; break; // Grosor medio
    case 3: grosor_curva = 5.0f; break; // Grosor grueso
    }
    glutPostRedisplay();
}
void menu_color_cinta_poligonal(int opcion) {
    switch (opcion) {
    case 1: color_cinta_poligonal[0] = 1.0f; color_cinta_poligonal[1] = 0.0f; color_cinta_poligonal[2] = 0.0f; break; // Rojo
    case 2: color_cinta_poligonal[0] = 0.0f; color_cinta_poligonal[1] = 1.0f; color_cinta_poligonal[2] = 0.0f; break; // Verde
    }
    glutPostRedisplay();
}

void menu_color_cinta_spline(int opcion) {
    switch (opcion) {
    case 1: color_cinta_spline[0] = 0.0f; color_cinta_spline[1] = 0.0f; color_cinta_spline[2] = 1.0f; break; // Azul
    case 2: color_cinta_spline[0] = 1.0f; color_cinta_spline[1] = 0.0f; color_cinta_spline[2] = 1.0f; break; // Magenta
    }
    glutPostRedisplay();
}


void crearMenu() {
    int menu_color = glutCreateMenu(menu_color_curva);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Negro", 4);

    int menu_graficar_curva = glutCreateMenu(menu_curva_toggle);
    glutAddMenuEntry("Activado", 1);
    glutAddMenuEntry("Desactivado", 2);

    int menu_graficar_puntos = glutCreateMenu(menu_toggle_puntos_control);
    glutAddMenuEntry("Activado", 1);
    glutAddMenuEntry("Desactivado", 2);

    int menu_ingreso_puntos = glutCreateMenu(menu_toggle_ingreso);
    glutAddMenuEntry("Activado", 1);
    glutAddMenuEntry("Desactivado", 2);

    int menu_mover_puntos = glutCreateMenu(menu_toggle_mover);
    glutAddMenuEntry("Activado", 1);
    glutAddMenuEntry("Desactivado", 2);

    int menu_grosor = glutCreateMenu(menu_grosor_curva);
    glutAddMenuEntry("Fino", 1);
    glutAddMenuEntry("Medio", 2);
    glutAddMenuEntry("Grueso", 3);

    int menu_color_cinta_poligonal1 = glutCreateMenu(menu_color_cinta_poligonal);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);

    int menu_color_cinta_spline1 = glutCreateMenu(menu_color_cinta_spline);
    glutAddMenuEntry("Azul", 1);
    glutAddMenuEntry("Magenta", 2);

    int menu_principal = glutCreateMenu(menu_curva_toggle);
    glutAddSubMenu("Insertar los puntos de control", menu_ingreso_puntos);
    glutAddSubMenu("Graficar puntos de control", menu_graficar_puntos);
    glutAddSubMenu("Graficar curva", menu_graficar_curva);
    glutAddSubMenu("Color (Curva)", menu_color);
    glutAddSubMenu("Cambiar posición de los puntos de control", menu_mover_puntos);
    glutAddSubMenu("Grosor de curva", menu_grosor);
    glutAddSubMenu("Color de la cinta poligonal", menu_color_cinta_poligonal1);
    glutAddSubMenu("Color de la cinta Spline", menu_color_cinta_spline1);


}

// Resize
void reshape(int w, int h) {
    window_width = w;
    window_height = h;
    glViewport(0, 0, w, h);
}

void initGL() {
    glClearColor(0.9f, 1.9f, 0.9f, 1.0f); // Verde claro
    glEnable(GL_DEPTH_TEST);
    srand(time(nullptr));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("PC02_CV_VALDIVIEZO_GOICOCHEA_ALDANA_CHIPANA");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    crearMenu();
    glutKeyboardFunc(teclado);
    glutMainLoop();
    return 0;
}