
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QLayoutItem>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

class JuliaRuntime : public QObject
{
public:
    QProcess *node;
    QWidget *window;
    QByteArray buffer;

    JuliaRuntime()
    {
        node = new QProcess(this);
        window = new QWidget();

        window->setWindowTitle("JuliaNative");
        window->resize(500, 350);

        connect(node, &QProcess::readyReadStandardOutput,
            this, [this]() {
                buffer += node->readAllStandardOutput();

                while (true) {
                    int index = buffer.indexOf('\n');
                    if (index < 0) break;

                    QByteArray line = buffer.left(index).trimmed();
                    buffer.remove(0, index + 1);

                    const QByteArray prefix = "__JULIA_UI__";
                    qDebug().noquote() << "[Qt] Received:" << line;

                    if (line.startsWith(prefix)) {
                        renderUI(line.mid(prefix.size()));
                    } else if (!line.isEmpty()) {
                        qDebug().noquote() << QString::fromUtf8(line);
                    }
                }
            }
        );

        connect(node, &QProcess::readyReadStandardError,
            this, [this]() {
                qDebug().noquote()
                    << node->readAllStandardError();
            }
        );
        connect(node, &QProcess::started, this, []() {
    qDebug() << "[Qt] Node.js started";
});

connect(node,
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this,
        [](int code, QProcess::ExitStatus status) {
            qDebug() << "[Qt] Node.js finished:"
                     << code << status;
        });
    }

    void start()
    {
       

         QString projectDir =
        QDir(QApplication::applicationDirPath())
            .absoluteFilePath("../..");

        projectDir = QDir::cleanPath(projectDir);

        QString mainFile =
            projectDir + "/app/build/src/main.js";

    qDebug() << "[Julia] Project:" << projectDir;
    qDebug() << "[Julia] Main file:" << mainFile;
        if (!QFileInfo::exists(mainFile)) {
            qFatal("JavaScript tidak ditemukan: %s",
                   qPrintable(mainFile));
        }

        node->setWorkingDirectory(projectDir);
        node->setProgram("node");
        node->setArguments({mainFile});

        window->show();

        node->start();

        if (!node->waitForStarted(5000)) {
            qFatal("Gagal menjalankan Node.js");
        }
    }

    QWidget *createNode(const QJsonObject &data)
    {
        QString type = data["type"].toString();
        QJsonObject props = data["props"].toObject();
        QJsonObject events = data["events"].toObject();
        QJsonArray children = data["children"].toArray();

        if (type == "Text") {
            QLabel *label = new QLabel(
                props["text"].toString()
            );

            label->setStyleSheet(
                QString("font-size:%1px; font-weight:%2;")
                    .arg(props["fontSize"].toInt(14))
                    .arg(props["fontWeight"].toString() == "bold"
                        ? "bold" : "normal")
            );

            return label;
        }

        if (type == "Button") {
            QPushButton *button = new QPushButton(
                props["text"].toString()
            );

            QString eventId = events["click"].toString();

            connect(button, &QPushButton::clicked,
                this, [this, eventId]() {
                    if (eventId.isEmpty()) return;

                    QJsonObject message;
                    message["type"] = "click";
                    message["id"] = eventId;

                    QByteArray data =
                        QJsonDocument(message)
                            .toJson(QJsonDocument::Compact);

                    node->write(data + "\n");
                }
            );

            return button;
        }

        QWidget *container = new QWidget();

        QBoxLayout *layout =
            type == "Row"
                ? static_cast<QBoxLayout *>(
                    new QHBoxLayout(container))
                : static_cast<QBoxLayout *>(
                    new QVBoxLayout(container));

        layout->setSpacing(props["spacing"].toInt(10));

        int padding = props["padding"].toInt(0);

        layout->setContentsMargins(
            padding, padding, padding, padding
        );

        for (const QJsonValue &child : children) {
            QWidget *widget = createNode(child.toObject());

            if (widget) {
                layout->addWidget(widget);
            }
        }

        return container;
    }

    void renderUI(const QByteArray &json)
    {
        qDebug() << "[Qt] renderUI called";
qDebug().noquote() << "[Qt] JSON:" << json;
        QJsonParseError error;

        QJsonDocument document =
            QJsonDocument::fromJson(json, &error);

        if (error.error != QJsonParseError::NoError) {
            qDebug() << "Invalid UI:" << error.errorString();
            return;
        }

        QJsonObject root = document.object();
        QJsonObject props = root["props"].toObject();

        window->setWindowTitle(
            props["title"].toString("JuliaNative")
        );

        window->resize(
            props["width"].toInt(500),
            props["height"].toInt(350)
        );

        QVBoxLayout *layout =
            qobject_cast<QVBoxLayout *>(window->layout());

        if (!layout) {
            layout = new QVBoxLayout(window);
            layout->setContentsMargins(0, 0, 0, 0);
        }

        // Bersihkan tampilan sebelumnya.
        while (QLayoutItem *item = layout->takeAt(0)) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }

            delete item;
        }

        QJsonArray children = root["children"].toArray();

        for (const QJsonValue &child : children) {
            QWidget *widget = createNode(child.toObject());

            if (widget) {
                layout->addWidget(widget);
            }
        }

        window->show();
    }

    ~JuliaRuntime()
    {
        if (node->state() != QProcess::NotRunning) {
            node->kill();
            node->waitForFinished();
        }

        delete window;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    JuliaRuntime runtime;
    runtime.start();

    return app.exec();
}
