#include <QApplication>
#include <QMainWindow>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Julia Native");
    window.resize(800, 500);

    QLabel *label = new QLabel("Hello Julia Native!");
    label->setAlignment(Qt::AlignCenter);

    window.setCentralWidget(label);
    window.show();

    return app.exec();
}