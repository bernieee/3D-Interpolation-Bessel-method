#include <QtGui> // подключаем модуль QtGui
//#include <QtCore>     // подключаем модуль QtCore
//#include <QtOpenGL>   // подключаем модуль QtOpenGL
#include "scene3D.h" // подключаем заголовочный файл scene3D.h
#include <cmath> // подключаем математическую библиотеку

#define AX_DEFAULT -1
#define BX_DEFAULT 1
#define AY_DEFAULT -1
#define BY_DEFAULT 1
#define NX_DEFAULT 5
#define NY_DEFAULT 5

GLfloat InterpolationVertex[(N_DRAW + 1) * (N_DRAW + 1)][3];
GLfloat InterpolationColor[(N_DRAW + 1) * (N_DRAW + 1)][3];
GLuint InterpolationIndex[2 * (N_DRAW + 1) * (N_DRAW + 1)][3];
GLuint InterpolationEdge[3 * (N_DRAW + 1) * (N_DRAW + 1)];

GLfloat FuncVertex[(N_DRAW + 1) * (N_DRAW + 1)][3];
GLfloat FuncColor[(N_DRAW + 1) * (N_DRAW + 1)][3];
GLuint FuncIndex[2 * (N_DRAW + 1) * (N_DRAW + 1)][3];
GLuint FuncEdge[3 * (N_DRAW + 1) * (N_DRAW + 1)];

const static double pi = 3.141593;

void Scene3D::func_name()
{
    switch (func_id) {
    case 0:
        f_name = "f(x, y) = 1";
        break;
    case 1:
        f_name = "f(x, y) = x";
        break;
    case 2:
        f_name = "f(x, y) = y";
        break;
    case 3:
        f_name = "f(x, y) = x + y";
        break;
    case 4:
        f_name = "f(x, y) = sqrt(x^2 + y^2)";
        break;
    case 5:
        f_name = "f(x, y) = x^2 + y^2";
        break;
    case 6:
        f_name = "f(x, y) = e^(x^2 - y^2)";
        break;
    case 7:
        f_name = "f(x, y) = 1 / (25 * (x^2 + y^2) + 1)";
        break;
    default:
        f_name = "";
    }
}

QSize Scene3D::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Scene3D::sizeHint() const
{
    return QSize(1000, 1000);
}

int Scene3D::parse_command_line(int argc, char *argv[])
{
    if (argc != 9 || sscanf(argv[1], "%d", &nx) != 1 ||
        sscanf(argv[2], "%d", &ny) != 1 ||
        sscanf(argv[3], "%d", &func_id) != 1 ||
        sscanf(argv[4], "%lf", &eps) != 1 || sscanf(argv[5], "%lf", &ax) != 1 ||
        sscanf(argv[6], "%lf", &by) != 1 || sscanf(argv[7], "%lf", &bx) != 1 ||
        sscanf(argv[8], "%lf", &ay) != 1 || (bx - ax < 0) || (by - ay < 0)) {
        printf("Usage %s nx ny func_id eps ax by bx ay\n", argv[0]);
    }

    return 0;
}

Scene3D::Scene3D(QWidget *parent, QLabel *log_lab)
    : QGLWidget(parent), xRot(-90), yRot(0), zRot(0), zTra(0), nSca(1),
      log_label(log_lab)
{
    ax = AX_DEFAULT;
    bx = BX_DEFAULT;
    ay = AY_DEFAULT;
    by = BY_DEFAULT;
    nx = NX_DEFAULT;
    ny = NY_DEFAULT;
    func_id = 0;
    func_name();

    f = new interpolation(ax, bx, ay, by, nx, ny, func_id);
}

Scene3D::~Scene3D()
{
    if (f) {
        delete f;
    }
}

void Scene3D::initializeGL()
{
    qglClearColor(Qt::white);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}

void Scene3D::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat ratio = (GLfloat)nHeight / (GLfloat)nWidth;

    if (nWidth >= nHeight)
        glOrtho(-1.0 / ratio, 1.0 / ratio, -1.0, 1.0, -10.0, 1.0);
    else
        glOrtho(-1.0, 1.0, -1.0 * ratio, 1.0 * ratio, -10.0, 1.0);
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

void Scene3D::change_label()
{
    QPen pen;
    std::string log_lab;

    switch (method) {
    case draw_method::origin:
        log_lab = "origin\n";
        break;
    case draw_method::bessel:
        log_lab = "bessel\n";
        break;
    case draw_method::error:
        log_lab = "error\n";
        break;
    }

    log_lab = log_lab + "func_id = " + std::to_string(func_id) + "\n";
    log_lab = log_lab + f_name + "\n";
    double f_max = f->get_f_max();
    log_lab = log_lab + "|f(x)| = " + std::to_string(f_max) + "\n";
    log_lab = log_lab + "scale = " + std::to_string(scale) + "\n";
    log_lab = log_lab + "nx = " + std::to_string(nx) + "\n";
    log_lab = log_lab + "ny = " + std::to_string(ny) + "\n";
    log_lab = log_lab + "disturbance = " + std::to_string(disturb) + "\n";

    log_label->setText(QString::fromStdString(log_lab));
}

void Scene3D::paintGL() // рисование
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(nSca, nSca, nSca);
    glTranslatef(0.0f, zTra, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);

    draw_axes();

    if (method == draw_method::origin) {
        get_vertex(FuncVertex, FuncColor, FuncEdge, FuncIndex,
                   interpolation_method::origin, draw_color::black);
        draw_func(FuncVertex, FuncColor, FuncEdge, FuncIndex);
    }
    if (method == draw_method::bessel) {
        get_vertex(InterpolationVertex, InterpolationColor, InterpolationEdge,
                   InterpolationIndex, interpolation_method::bessel,
                   draw_color::violet);
        draw_func(InterpolationVertex, InterpolationColor, InterpolationEdge,
                  InterpolationIndex);

        get_vertex(FuncVertex, FuncColor, FuncEdge, FuncIndex,
                   interpolation_method::origin, draw_color::black);
        draw_func(FuncVertex, FuncColor, FuncEdge, FuncIndex);
    }
    if (method == draw_method::error) {
        get_vertex(InterpolationVertex, InterpolationColor, InterpolationEdge,
                   InterpolationIndex, interpolation_method::error,
                   draw_color::red);
        draw_func(InterpolationVertex, InterpolationColor, InterpolationEdge,
                  InterpolationIndex);

        get_vertex(FuncVertex, FuncColor, FuncEdge, FuncIndex,
                   interpolation_method::origin, draw_color::black);
        draw_func(FuncVertex, FuncColor, FuncEdge, FuncIndex);
    }
    change_label();
}

void Scene3D::draw_axes() // построить оси координат
{
    glLineWidth(3.0f);

    glColor4f(1.00f, 0.00f, 0.00f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glEnd();

    QColor halfGreen(0, 128, 0, 255);
    qglColor(halfGreen);
    glBegin(GL_LINES);
    // ось y зеленого цвета
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, 0.0f);

    glColor4f(0.00f, 0.00f, 1.00f, 1.0f);
    // ось z синего цвета
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -1.0f);
    glEnd();
}

void set_color(GLfloat color[(N_DRAW + 1) * (N_DRAW + 1)][3], int ver,
               draw_color clr)
{
    switch (clr) {
    case draw_color::black:
        color[ver][0] = 0.f;
        color[ver][1] = 0.f;
        color[ver][2] = 0.f;
        break;
    case draw_color::violet:
        color[ver][0] = 1.f;
        color[ver][1] = 0.f;
        color[ver][2] = 1.f;
        break;
    case draw_color::red:
        color[ver][0] = 1.f;
        color[ver][1] = 0.f;
        color[ver][2] = 0.f;
        break;
    }
}

void Scene3D::get_vertex(GLfloat vertex[(N_DRAW + 1) * (N_DRAW + 1)][3],
                         GLfloat color[(N_DRAW + 1) * (N_DRAW + 1)][3],
                         GLuint edge[3 * (N_DRAW + 1) * (N_DRAW + 1)],
                         GLuint index[2 * (N_DRAW + 1) * (N_DRAW + 1)][3],
                         interpolation_method method, draw_color clr)
{
    int ver = 0;
    int ind = 0;
    int ed = 0;

    double step_x = (bx - ax) / N_DRAW;
    double step_y = (by - ay) / N_DRAW;

    for (double x = ax; x - bx < eps; x += step_x) {
        for (double y = ay; y - by < eps; y += step_y) {
            vertex[ver][0] = x;
            vertex[ver][1] = y;
            vertex[ver][2] = f->get_f_value(x, y, method);

            set_color(color, ver, clr);

            //            color[ver][0] = 1.f * (2. / 255.);
            //            color[ver][1] = 1.f * (32. / 255.);
            //            color[ver][2] = 1.f * (39. / 255.);

            for (int e = 0; e < 3; e++) {
                edge[ed * 3 + e] = GL_TRUE;
            }
            if (abs(x - ax) > eps && abs(y - ay) > eps) {
                index[ind][0] = ver;
                index[ind][2] = ver - N_DRAW - 2;
                index[ind][1] = ver - N_DRAW - 1;
                ind++;

                index[ind][0] = ver;
                index[ind][1] = ver - 1;
                index[ind][2] = ver - N_DRAW - 2;

                ind++;
            }
            ver++;
        }
    }
}

void Scene3D::draw_func(GLfloat vertex[(N_DRAW + 1) * (N_DRAW + 1)][3],
                        GLfloat color[(N_DRAW + 1) * (N_DRAW + 1)][3],
                        GLuint edge[3 * (N_DRAW + 1) * (N_DRAW + 1)],
                        GLuint index[2 * (N_DRAW + 1) * (N_DRAW + 1)][3])
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    glVertexPointer(3, GL_FLOAT, 0, vertex);
    glColorPointer(3, GL_FLOAT, 0, color);
    glEdgeFlagPointer(0, edge);
    glDrawElements(GL_LINES, 6 * (N_DRAW + 1) * (N_DRAW + 1), GL_UNSIGNED_INT,
                   index);
}

void Scene3D::change_func()
{
    func_id = (func_id + 1) % 8;
    f->change_func(func_id);
    func_name();
    disturb = 0;

    updateGL();
}

void Scene3D::change_method()
{
    switch (method) {
    case draw_method::origin:
        method = draw_method::bessel;
        break;
    case draw_method::bessel:
        method = draw_method::error;
        break;
    case draw_method::error:
        method = draw_method::origin;
        break;
    }

    updateGL();
}

void Scene3D::increase_scale()
{
    nSca *= 2;
}

void Scene3D::decrease_scale()
{
    nSca /= 2;
}

void Scene3D::increase_n()
{
    nx *= 2;
    ny *= 2;

    f->change_n(nx, ny);
    updateGL();
}

void Scene3D::decrease_n()
{
    if (nx / 2 <= 1 && ny / 2 <= 1) {
        printf("Cannot make less points than nx = 2 ny = 2\n");
        return;
    }
    nx /= 2;
    ny /= 2;

    f->change_n(nx, ny);
    updateGL();
}

void Scene3D::increase_disturb()
{
    disturb++;

    f->change_disturb(disturb);
    updateGL();
}

void Scene3D::decrease_disturb()
{
    disturb--;

    f->change_disturb(disturb);
    updateGL();
}

void Scene3D::mousePressEvent(QMouseEvent *pe) // нажатие клавиши мыши
{
    ptrMousePosition = pe->pos();
    // ptrMousePosition = (*pe).pos(); // можно и так написать
}

void Scene3D::mouseReleaseEvent(QMouseEvent *pe) // отжатие клавиши мыши
{
    // некоторые функции, которые должны выполняться при отжатии клавиши мыши
}

void Scene3D::mouseMoveEvent(
    QMouseEvent *pe) // изменение положения стрелки мыши
{
    xRot += 180 / nSca * (GLfloat)(pe->y() - ptrMousePosition.y()) /
            height(); // вычисление углов поворота
    zRot += 180 / nSca * (GLfloat)(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL(); // обновление изображения
}

void Scene3D::wheelEvent(QWheelEvent *pe)
{
    if ((pe->delta()) > 0)
        scale_plus();
    else if ((pe->delta()) < 0)
        scale_minus();

    updateGL(); // обновление изображения
}

void Scene3D::keyPressEvent(QKeyEvent *pe) // нажатие определенной клавиши
{
    switch (pe->key()) {
    case Qt::Key_Plus:
        scale_plus(); // приблизить сцену
        break;

    case Qt::Key_Equal:
        scale_plus(); // приблизить сцену
        break;

    case Qt::Key_Minus:
        scale_minus(); // удалиться от сцены
        break;

    case Qt::Key_Up:
        rotate_up(); // повернуть сцену вверх
        break;

    case Qt::Key_Down:
        rotate_down(); // повернуть сцену вниз
        break;

    case Qt::Key_Left:
        rotate_left(); // повернуть сцену влево
        break;

    case Qt::Key_Right:
        rotate_right(); // повернуть сцену вправо
        break;

    case Qt::Key_Z:
        translate_down(); // транслировать сцену вниз
        break;

    case Qt::Key_X:
        translate_up(); // транслировать сцену вверх
        break;

    case Qt::Key_Space: // клавиша пробела
        defaultScene(); // возвращение значений по умолчанию
        break;

    case Qt::Key_Escape: // клавиша "эскейп"
        this->close(); // завершает приложение
        break;
    }

    updateGL(); // обновление изображения
}

void Scene3D::scale_plus() // приблизить сцену
{
    nSca = nSca * 1.1;
}

void Scene3D::scale_minus() // удалиться от сцены
{
    nSca = nSca / 1.1;
}

void Scene3D::rotate_up() // повернуть сцену вверх
{
    xRot += 1.0;
}

void Scene3D::rotate_down() // повернуть сцену вниз
{
    xRot -= 1.0;
}

void Scene3D::rotate_left() // повернуть сцену влево
{
    zRot += 1.0;
}

void Scene3D::rotate_right() // повернуть сцену вправо
{
    zRot -= 1.0;
}

void Scene3D::translate_down() // транслировать сцену вниз
{
    zTra -= 0.05;
}

void Scene3D::translate_up() // транслировать сцену вверх
{
    zTra += 0.05;
}

void Scene3D::defaultScene() // наблюдение сцены по умолчанию
{
    xRot = -90;
    yRot = 0;
    zRot = 0;
    zTra = 0;
    nSca = 1;
}
