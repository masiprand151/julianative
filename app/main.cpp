#include <QApplication>
#include "JuliaRuntime.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("JuliaNative");
    app.setApplicationName("JuliaNative");
    JuliaRuntime runtime;
    runtime.start(app.arguments().contains("--dev"));
    return app.exec();
}
