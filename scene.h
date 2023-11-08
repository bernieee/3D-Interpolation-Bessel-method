#ifndef SCENE3D_H
#define SCENE3D_H

#include "interpolation.h"
#include <QGLWidget>
#include <QLabel>

#define N_DRAW 50
enum class draw_method {
    origin,
    bessel,
    error,
};

enum class draw_color {
    black,
    violet,
    red,
};

class Scene3D : public QGLWidget
{
    Q_OBJECT

  private:
    int nx;
    int ny;
    int func_id;
    std::string f_name;
    double ax;
    double ay;
    double bx;
    double by;
    double eps = 1.e-6;

    double center_x;
    double center_y;
    double delta_x;
    double delta_y;
    int scale = 1;
    int angle = 0;
    int disturb = 0;
    interpolation *f = nullptr;
    draw_method method = draw_method::origin;

    GLfloat xRot; // переменная хранит угол поворота вокруг оси X
    GLfloat yRot; // переменная хранит угол поворота вокруг оси Y
    GLfloat zRot; // переменная хранит угол поворота вокруг оси Z
    GLfloat zTra; // переменная хранит величину трансляции оси Z
    GLfloat nSca; // переменная отвечает за масштабирование обьекта

    GLfloat xSca;
    GLfloat ySca;
    GLfloat zSca;

    QLabel *log_label;

    QPoint ptrMousePosition; // переменная хранит координату указателя мыши в
                             // момент нажатия

    void scale_plus(); // приблизить сцену
    void scale_minus(); // удалиться от сцены
    void rotate_up(); // повернуть сцену вверх
    void rotate_down(); // повернуть сцену вниз
    void rotate_left(); // повернуть сцену влево
    void rotate_right(); // повернуть сцену вправо
    void translate_down(); // транслировать сцену вниз
    void translate_up(); // транслировать сцену вверх
    void defaultScene(); // наблюдение сцены по умолчанию

    void get_vertex(GLfloat vertex[(N_DRAW + 1) * (N_DRAW + 1)][3],
                    GLfloat color[(N_DRAW + 1) * (N_DRAW + 1)][3],
                    GLuint edge[3 * (N_DRAW + 1) * (N_DRAW + 1)],
                    GLuint index[2 * (N_DRAW + 1) * (N_DRAW + 1)][3],
                    interpolation_method method, draw_color clr);
    void draw_axes();
    void draw_func(GLfloat vertex[(N_DRAW + 1) * (N_DRAW + 1)][3],
                   GLfloat color[(N_DRAW + 1) * (N_DRAW + 1)][3],
                   GLuint edge[3 * (N_DRAW + 1) * (N_DRAW + 1)],
                   GLuint index[2 * (N_DRAW + 1) * (N_DRAW + 1)][3]);

  protected:
    void change_label();
    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *pe); // методы обработки события мыши при
                                           // нажатии клавиши мыши
    void mouseMoveEvent(
        QMouseEvent *pe); // методы обработки события мыши при перемещении мыши
    void mouseReleaseEvent(QMouseEvent *pe); // методы обработки событий мыши
                                             // при отжатии клавиши мыши
    void wheelEvent(QWheelEvent *pe); // метод обработки событий колесика мыши
    void keyPressEvent(QKeyEvent *pe); // методы обработки события при нажатии
                                       // определенной клавиши

  public:
    Scene3D(QWidget *parent, QLabel *log_lab);
    ~Scene3D();
    int parse_command_line(int argc, char *argv[]);
    void func_name();

  public slots:
    void change_func();
    void change_method();
    void increase_scale();
    void decrease_scale();
    void increase_n();
    void decrease_n();
    void increase_disturb();
    void decrease_disturb();
};

#endif // SCENE3D_H
