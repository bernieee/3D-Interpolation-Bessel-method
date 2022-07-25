#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPalette>
#include <QToolBar>
#include <QVBoxLayout>

#include "scene3D.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyle("plastique");
    QFont f("Helvetica");
    f.setStyleHint(QFont::System);
    f.setPixelSize(18);
    f.setFamily("courier");
    f.setBold(2);
    qApp->setFont(f);

    QMainWindow *window = new QMainWindow;
    QMenuBar *menu_bar = new QMenuBar(window);
    QToolBar *tool_bar = new QToolBar(window);
    QLabel *log_label = new QLabel;
    QAction *action;

    Scene3D *scene = new Scene3D(window, log_label);

    QPalette graph_pal(scene->palette());
    graph_pal.setColor(QPalette::Window, "Honeydew");
    scene->setAutoFillBackground(true);
    scene->setPalette(graph_pal);

    QPalette tool_pal(tool_bar->palette());
    tool_pal.setColor(QPalette::Window, "WhiteSmoke");
    tool_bar->setAutoFillBackground(true);
    tool_bar->setPalette(tool_pal);

    menu_bar->setMaximumHeight(30);
    tool_bar->setFixedWidth(400);

    if (scene->parse_command_line(argc, argv) != 0) {
        qWarning("Wrong input arguments!");
        delete menu_bar;
        return -1;
    }

    action =
        menu_bar->addAction("&Change function", scene, SLOT(change_func()));
    action->setShortcut(QString("0"));
    action =
        menu_bar->addAction("&Change method", scene, SLOT(change_method()));
    action->setShortcut(QString("1"));
    //    action =
    //        menu_bar->addAction("&Increase scale", scene,
    //        SLOT(increase_scale()));
    //    action->setShortcut(QString("2"));
    //    action =
    //        menu_bar->addAction("&Decrease scale", scene,
    //        SLOT(decrease_scale()));
    //    action->setShortcut(QString("3"));
    action = menu_bar->addAction("&Increase n", scene, SLOT(increase_n()));
    action->setShortcut(QString("2"));
    action = menu_bar->addAction("&Decrease n", scene, SLOT(decrease_n()));
    action->setShortcut(QString("3"));
    action = menu_bar->addAction("&Increase disturbance", scene,
                                 SLOT(increase_disturb()));
    action->setShortcut(QString("4"));
    action = menu_bar->addAction("&Decrease disturbance", scene,
                                 SLOT(decrease_disturb()));
    action->setShortcut(QString("5"));
    action = menu_bar->addAction("E&xit", window, SLOT(close()));
    action->setShortcut(QString("Ctrl+X"));

    window->setMenuBar(menu_bar);
    window->setCentralWidget(scene);
    window->addToolBar(Qt::RightToolBarArea, tool_bar);
    tool_bar->addWidget(log_label);
    window->setWindowTitle("Graph");

    window->show();

    return app.exec();
}
