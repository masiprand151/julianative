
// #include <QApplication>
// #include <QWidget>
// #include <QLabel>
// #include <QPushButton>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QBoxLayout>
// #include <QLayoutItem>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QProcess>
// #include <QDir>
// #include <QFileInfo>
// #include <QCoreApplication>
// #include <QSizePolicy>
// #include <QList>

// // Mengelola komunikasi antara runtime C++ Qt dan aplikasi JavaScript Node.js.
// class JuliaRuntime : public QObject
// {
// public:
//     // Proses Node.js yang menjalankan kode JavaScript.
//     QProcess *node;

//     // Window utama aplikasi JuliaNative.
//     QWidget *window;

//     // Menampung data output Node.js sebelum diproses per baris.
//     QByteArray buffer;

//     // Constructor: menyiapkan window dan koneksi komunikasi Node.js.
//     JuliaRuntime()
//     {
//         node = new QProcess(this);
//         window = new QWidget();
//         // Memisahkan output stdout dan stderr Node.js.
//         node->setProcessChannelMode(QProcess::SeparateChannels);


//         // Pengaturan awal window.
//         window->setWindowTitle("JuliaNative");
//         window->resize(500, 350);

//         // Menerima output dari Node.js ketika tersedia.
//         connect(node, &QProcess::readyReadStandardOutput,
//             this, [this]() {
                

//                 // Menggabungkan data baru dengan data yang belum selesai dibaca.
//                 buffer += node->readAllStandardOutput();

//                 // Membaca output berdasarkan pemisah baris.
//                 while (true) {
//                     int index = buffer.indexOf('\n');

//                     if (index < 0) break;

//                     // Mengambil satu baris data.
//                     QByteArray line = buffer.left(index).trimmed();
//                     buffer.remove(0, index + 1);

//                     // Penanda khusus untuk membedakan pesan UI dari output biasa.
//                     const QByteArray prefix = "__JULIA_UI__";
//                     if (line.startsWith(prefix)) {
//                         QByteArray json = line.mid(prefix.size());

//                         QJsonDocument doc = QJsonDocument::fromJson(json);

//                         if (!doc.isObject()) continue;

//                         QJsonObject message = doc.object();

//                         QString type = message["type"].toString();

//                         // Perintah dari JavaScript
//                         if (type == "command") {
//                             QString command = message["command"].toString();
//                             // Jika perintah adalah "exit", tutup aplikasi.
//                             if (command == "exit") {
//                                 QCoreApplication::quit();
//                             }
//                         }
//                         // Pesan untuk render UI
//                         else {
//                             renderUI(json);
//                         }
//                     }
                   
//                 }
//             }
//         );

//         connect(node, &QProcess::readyReadStandardError,
//             this, [this]() {
//                 QByteArray error = node->readAllStandardError();
//                 qDebug().noquote() << "[NODE STDERR]" << error;
//             }
//         );
//     }

//     // Menjalankan Node.js dan menentukan lokasi file JavaScript.
//     void start()
//     {
//         // Mengambil lokasi folder tempat executable JuliaNative berada.
//         QString appDir =
//             QCoreApplication::applicationDirPath();

//         // Lokasi Node.js untuk versi portable.
//         QString nodePath =
//             QDir(appDir).filePath("runtime/node/node.exe");

//         // Lokasi file utama JavaScript untuk versi portable.
//         QString mainFile =
//             QDir(appDir).filePath("runtime/app/src/main.js");

//         // Folder kerja Node.js untuk versi portable.
//         QString workingDir =
//             QDir(appDir).filePath("runtime/app");

//         // Jika file portable tidak ditemukan, gunakan struktur project development.
//         if (!QFileInfo::exists(nodePath) ||
//             !QFileInfo::exists(mainFile))
//         {
//             // Mengambil lokasi root project dari folder executable.
//             QString projectDir =
//                 QDir(appDir).absoluteFilePath("../..");

//             projectDir = QDir::cleanPath(projectDir);

//             // Lokasi Node.js pada project development.
//             nodePath =
//                 QDir(projectDir).filePath("app/node/node.exe");

//             // Lokasi hasil compile TypeScript.
//             mainFile =
//                 QDir(projectDir).filePath("app/build/src/main.js");

//             // Folder kerja Node.js.
//             workingDir = projectDir;
//         }

//         // Memastikan Node.js dan file utama JavaScript tersedia.
//         if (!QFileInfo::exists(nodePath) ||
//             !QFileInfo::exists(mainFile))
//         {
//             window->close();
//             return;
//         }

//         // Menentukan executable Node.js yang akan dijalankan.
//         node->setProgram(nodePath);

//         // Mengirimkan file JavaScript sebagai argumen Node.js.
//         node->setArguments({mainFile});

//         // Menentukan direktori kerja proses Node.js.
//         node->setWorkingDirectory(workingDir);

//         // Menampilkan window utama.
//         window->show();

//         // Memulai proses Node.js.
//         node->start();

//         // Menunggu sampai proses Node.js berhasil dijalankan.
//         if (!node->waitForStarted(5000)) {
//             window->close();
//         }
//     }

//     QWidget *createNode(const QJsonObject &data)
//     {
//         QString type = data["type"].toString();

//         QJsonObject props = data["props"].toObject();
//         QJsonObject events = data["events"].toObject();

//         if (type == "Text") {
//             QLabel *label = new QLabel(props["text"].toString());
       
//             label->setStyleSheet(
//                 QString("font-size:%1px; font-weight:%2;")
//                     .arg(props["fontSize"].toInt(14))
//                     .arg(
//                         props["fontWeight"].toString() == "bold"
//                             ? "bold"
//                             : "normal"
//                     )
//             );

//             return label;
//         }

//         if (type == "Button") {
//             QPushButton *button = new QPushButton(
//                 props["text"].toString()
//             );


//             QString eventId = events["click"].toString();

//             connect(button, &QPushButton::clicked,
//                 this, [this, eventId]() {

//                     if (eventId.isEmpty()) return;

//                     QJsonObject message;
//                     message["type"] = "click";
//                     message["id"] = eventId;

//                     QByteArray data =
//                         QJsonDocument(message)
//                             .toJson(QJsonDocument::Compact);

//                     node->write(data + "\n");
//                 }
//             );

//             return button;
//         }

//         if (type == "Row") {
//             return createRow(data);
//         }

//         if (type == "Column") {
//             return createColumn(data);
//         }

//         if (type == "View") {
//             return createView(data);
//         }

//         return nullptr;
//     }

//     QWidget *createView(const QJsonObject &data)
//     {
//          QString direction = data["props"].toObject()
//                             ["direction"].toString("column");

//         return createFlexContainer(data, direction);
//     }

//     QWidget *createRow(const QJsonObject &data)
//     {
//         return createFlexContainer(data, "row");
//     }

//     QWidget *createColumn(const QJsonObject &data)
//     {
//         return createFlexContainer(data, "column");
//     }
    

//     QWidget *createFlexContainer(
//     const QJsonObject &data,
//     const QString &direction
//     ) {
//         QJsonObject props = data["props"].toObject();
//         QJsonArray children = data["children"].toArray();

//         QWidget *container = new QWidget();

//         // Container dapat memenuhi ruang dari parent.
//         container->setSizePolicy(
//             QSizePolicy::Expanding,
//             QSizePolicy::Expanding
//         );

//         QBoxLayout *layout;

//         bool isRow = direction == "row";

//         if (isRow) {
//             layout = new QHBoxLayout(container);
//         } else {
//             layout = new QVBoxLayout(container);
//         }

//         int padding = props["padding"].toInt(0);
//         int spacing = props["spacing"].toInt(0);

//         QString justify = props["justifyContent"].toString("start");
//         QString align = props["alignItems"].toString("stretch");

//         layout->setContentsMargins(
//             padding, padding, padding, padding
//         );

//         // Spacing diatur manual
//         layout->setSpacing(0);

//         struct FlexItem {
//             QWidget *widget;
//             int flex;
//         };

//         QList<FlexItem> items;

//         int totalFlex = 0;

//         // Membuat children
//         for (const QJsonValue &child : children) {
//             QJsonObject childData = child.toObject();

//             QWidget *widget = createNode(childData);

//             if (!widget) continue;

//             QJsonObject childProps = childData["props"].toObject();

//             int flex = qMax(0, childProps["flex"].toInt(0));

//             QSizePolicy policy = widget->sizePolicy();

//             // Mengatur alignItems
//             if (isRow) {
//                 policy.setVerticalPolicy(
//                     align == "stretch"
//                         ? QSizePolicy::Expanding
//                         : QSizePolicy::Preferred
//                 );

//                 if (flex > 0) {
//                     policy.setHorizontalPolicy(QSizePolicy::Expanding);
//                 }
//             } else {
//                 policy.setHorizontalPolicy(
//                     align == "stretch"
//                         ? QSizePolicy::Expanding
//                         : QSizePolicy::Preferred
//                 );

//                 if (flex > 0) {
//                     policy.setVerticalPolicy(QSizePolicy::Expanding);
//                 }
//             }

//             widget->setSizePolicy(policy);

//             items.append({widget, flex});

//             totalFlex += flex;
//         }

//         int count = items.size();

//         // Menentukan alignment cross-axis
//         Qt::Alignment alignment;

//         if (align == "center") {
//             alignment = isRow
//                 ? Qt::AlignVCenter
//                 : Qt::AlignHCenter;
//         }
//         else if (align == "start") {
//             alignment = isRow
//                 ? Qt::AlignTop
//                 : Qt::AlignLeft;
//         }
//         else if (align == "end") {
//             alignment = isRow
//                 ? Qt::AlignBottom
//                 : Qt::AlignRight;
//         }
//         // stretch menggunakan alignment kosong

//         // Justify hanya membagikan ruang jika tidak ada flex
//         bool useJustify = totalFlex == 0;

//         // Ruang sebelum item
//         if (useJustify) {
//             if (justify == "center" ||
//                 justify == "end" ||
//                 justify == "space-around" ||
//                 justify == "space-evenly") {

//                 layout->addStretch(1);
//             }
//         }

//         for (int i = 0; i < count; i++) {

//             // Gap antar item
//             if (i > 0) {
//                 layout->addSpacing(spacing);

//                 if (useJustify) {
//                     if (justify == "space-between") {
//                         layout->addStretch(1);
//                     }
//                     else if (justify == "space-around") {
//                         layout->addStretch(2);
//                     }
//                     else if (justify == "space-evenly") {
//                         layout->addStretch(1);
//                     }
//                 }
//             }

//             layout->addWidget(
//                 items[i].widget,
//                 items[i].flex,
//                 alignment
//             );
//         }

//         // Ruang setelah item
//         if (useJustify) {
//             if (justify == "start" ||
//                 justify == "center" ||
//                 justify == "space-around" ||
//                 justify == "space-evenly" ||
//                 (justify == "space-between" && count == 1)) {

//                 layout->addStretch(1);
//             }
//         }


//         return container;
//     }
//     Qt::Alignment getAlignment(
//     const QString &align,
//     const QString &direction
//     ) {
//         bool isRow = direction == "row";

//         if (align == "center") {
//             return isRow ? Qt::AlignVCenter : Qt::AlignHCenter;
//         }

//         if (align == "end") {
//             return isRow ? Qt::AlignBottom : Qt::AlignRight;
//         }

//         if (align == "start") {
//             return isRow ? Qt::AlignTop : Qt::AlignLeft;
//         }

//         // stretch: tidak memberikan alignment khusus
//         return {};
//     }

//     void addJustifyStart(QBoxLayout *layout)
//     {
//         layout->addStretch(1);
//     }

//     void addJustifyCenter(QBoxLayout *layout)
//     {
//         layout->insertStretch(0, 1);
//         layout->addStretch(1);
//     }

//     void addJustifyEnd(QBoxLayout *layout)
//     {
//         layout->insertStretch(0, 1);
//     }


//     // Menerima JSON dari Node.js dan merender UI ke window Qt.
//     void renderUI(const QByteArray &json)
//     {
//         // Menyiapkan pemeriksaan format JSON.
//         QJsonParseError error;

//         // Mengubah data JSON teks menjadi QJsonDocument.
//         QJsonDocument document =
//             QJsonDocument::fromJson(json, &error);

//         // Menghentikan proses jika JSON tidak valid atau bukan object.
//         if (error.error != QJsonParseError::NoError ||
//             !document.isObject())
//         {
//             return;
//         }

//         // Mengambil object utama dan properti window.
//         QJsonObject root = document.object();
//         QJsonObject props = root["props"].toObject();

//         // Mengatur judul window dari properti Widget.
//         window->setWindowTitle(
//             props["title"].toString("JuliaNative")
//         );

//         // Mengatur ukuran window dari properti Widget.
//         window->resize(
//             props["width"].toInt(500),
//             props["height"].toInt(350)
//         );

//         // Mengambil layout utama window jika sudah tersedia.
//         QVBoxLayout *layout =
//             qobject_cast<QVBoxLayout *>(window->layout());

//         // Membuat layout utama jika belum tersedia.
//        if (!layout) {
//             layout = new QVBoxLayout(window);

//             layout->setContentsMargins(0, 0, 0, 0);
//             layout->setSpacing(0);
//         }

//         // Menghapus seluruh widget dari tampilan sebelumnya.
//         while (QLayoutItem *item = layout->takeAt(0)) {
//             if (QWidget *widget = item->widget()) {
//                 delete widget;
//             }

//             delete item;
//         }

//         // Mengambil children dari root JSON.
//         QJsonArray children = root["children"].toArray();

//         // Membuat dan menambahkan children ke window.
//         for (const QJsonValue &child : children) {
//             QWidget *widget = createNode(child.toObject());

//             if (widget) {
//                 layout->addWidget(widget);
//             }
//         }

//         // Menampilkan window setelah UI selesai dibuat.
//         window->show();
//     }

//     // Destructor: menghentikan Node.js dan membersihkan window.
//     ~JuliaRuntime()
//     {
//         // Memastikan proses Node.js berhenti sebelum runtime dihancurkan.
//         if (node->state() != QProcess::NotRunning) {
//             node->kill();
//             node->waitForFinished();
//         }

//         // Menghapus window utama.
//         delete window;
//     }
// };

// // Entry point aplikasi JuliaNative.
// int main(int argc, char *argv[])
// {
//     // Membuat aplikasi Qt.
//     QApplication app(argc, argv);

//     // Membuat runtime JuliaNative.
//     JuliaRuntime runtime;

//     // Menjalankan Node.js dan menampilkan window.
//     runtime.start();

//     // Menjalankan event loop Qt.
//     return app.exec();
// }


#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QWindow>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleValidator>
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
#include <QFile>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QCoreApplication>
#include <QSizePolicy>
#include <QList>
#include <QCursor>
#include <QHash>
#include <QEvent>
#include <QSet>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QSignalBlocker>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

// Mengelola komunikasi antara runtime C++ Qt dan aplikasi JavaScript Node.js.
class JuliaRuntime : public QObject
{
public:
    // Proses Node.js yang menjalankan kode JavaScript.
    QProcess *node;

    // Window utama aplikasi JuliaNative.
    QWidget *window;
    QScrollArea *scrollArea = nullptr;
    QWidget *scrollContent = nullptr;
    QVBoxLayout *contentLayout = nullptr;
    QJsonObject windowEvents;
    QLineEdit *autoFocusInput = nullptr;
    QList<QDialog *> modalDialogs;
    QList<QWidget *> floatingActionButtons;
    int styleObjectCounter = 0;

    // Menampung data output Node.js sebelum diproses per baris.
    QByteArray buffer;

#ifdef Q_OS_WIN
    HANDLE nodeJob = nullptr;
#endif
    QFileSystemWatcher *reloadWatcher = nullptr;
    QTimer *reloadTimer = nullptr;
    QString nodePath;
    QString mainFile;
    QString workingDir;
    bool applyingWindowProps = false;
    bool hasAppliedPosition = false;
    QPoint lastAppliedPosition;
    QPoint requestedPosition;
    bool hasRequestedPosition = false;
    bool hasAppliedWindowSize = false;
    QSize lastAppliedWindowSize;
    bool windowAlwaysOnTop = false;
    bool windowHideTitleBar = false;
    bool hasAppliedWindowFlags = false;
    bool windowResizable = true;
    bool closePending = false;
    bool closeAuthorized = false;
    bool hasWindowState = false;
    Qt::WindowStates lastWindowState;

    // Constructor: menyiapkan window dan koneksi komunikasi Node.js.
    JuliaRuntime()
    {
        node = new QProcess(this);
        window = new QWidget();
        // Memisahkan output stdout dan stderr Node.js.
        node->setProcessChannelMode(QProcess::SeparateChannels);
        window->installEventFilter(this);

        // Pengaturan awal window.
        window->setWindowTitle("JuliaNative");
        window->resize(500, 350);

        // Menerima output dari Node.js ketika tersedia.
        connect(node, &QProcess::readyReadStandardOutput,
            this, [this]() {
                
                // Menggabungkan data baru dengan data yang belum selesai dibaca.
                buffer += node->readAllStandardOutput();

                // Membaca output berdasarkan pemisah baris.
                while (true) {
                    int index = buffer.indexOf('\n');

                    if (index < 0) break;

                    // Mengambil satu baris data.
                    QByteArray line = buffer.left(index).trimmed();
                    buffer.remove(0, index + 1);

                    // Penanda khusus untuk membedakan pesan UI dari output biasa.
                    const QByteArray prefix = "__JULIA_UI__";
                    if (line.startsWith(prefix)) {
                        QByteArray json = line.mid(prefix.size());

                        QJsonDocument doc = QJsonDocument::fromJson(json);

                        if (!doc.isObject()) continue;

                        QJsonObject message = doc.object();

                        QString type = message["type"].toString();

                        // Perintah dari JavaScript
                        if (type == "command") {
                            QString command = message["command"].toString();
                            // Jika perintah adalah "exit", tutup aplikasi.
                            if (command == "exit") {
                                QCoreApplication::quit();
                            } else if (command == "windowCloseAck" &&
                                       closePending)
                            {
                                closeAuthorized = true;
                                window->close();
                            }
                        }
                        // Pesan untuk render UI
                        else {
                            renderUI(json);
                        }
                    }
                }
            }
        );

        connect(node, &QProcess::readyReadStandardError,
            this, [this]() {
                QByteArray error = node->readAllStandardError();
                qDebug().noquote() << "[NODE STDERR]" << error;
            }
        );
    }

    // Menjalankan Node.js dan menentukan lokasi file JavaScript.
    void start(bool developmentMode = false)
    {
        // Mengambil lokasi folder tempat executable JuliaNative berada.
        QString appDir =
            QCoreApplication::applicationDirPath();

        // Lokasi Node.js untuk versi portable.
        nodePath =
            QDir(appDir).filePath("runtime/node/node.exe");

        // Lokasi file utama JavaScript untuk versi portable.
        mainFile =
            QDir(appDir).filePath("runtime/app/src/main.js");

        // Folder kerja Node.js untuk versi portable.
        workingDir =
            QDir(appDir).filePath("runtime/app");

        QString projectDir;
        if (developmentMode) {
            projectDir = QDir::currentPath();
        }

        // Jika file portable tidak ditemukan, gunakan struktur project development.
        if (developmentMode ||
            !QFileInfo::exists(nodePath) ||
            !QFileInfo::exists(mainFile))
        {
            // Mengambil lokasi root project dari folder executable.
            if (projectDir.isEmpty()) {
                projectDir = QDir(appDir).absoluteFilePath("../..");
            }

            projectDir = QDir::cleanPath(projectDir);

            // Lokasi Node.js pada project development.
            nodePath =
                QDir(projectDir).filePath("app/node/node.exe");

            // Lokasi hasil compile TypeScript.
            mainFile =
                QDir(projectDir).filePath("app/build/src/main.js");

            // Folder kerja Node.js.
            workingDir = projectDir;
        }

        // Memastikan Node.js dan file utama JavaScript tersedia.
        if (!QFileInfo::exists(nodePath) ||
            !QFileInfo::exists(mainFile))
        {
            window->close();
            return;
        }

        // Menentukan executable Node.js yang akan dijalankan.
        node->setProgram(nodePath);

        // Mengirimkan file JavaScript sebagai argumen Node.js.
        node->setArguments({mainFile});

        // Menentukan direktori kerja proses Node.js.
        node->setWorkingDirectory(workingDir);

        if (developmentMode) {
            const QString reloadSignalPath =
                QDir(projectDir).filePath("app/build/.dev-reload");
            QFile reloadSignal(reloadSignalPath);
            if (!reloadSignal.open(QIODevice::WriteOnly)) {
                qWarning() << "Could not create development reload signal:"
                           << reloadSignal.errorString();
                window->close();
                return;
            }
            reloadSignal.close();

            reloadWatcher = new QFileSystemWatcher(this);
            reloadTimer = new QTimer(this);
            reloadTimer->setSingleShot(true);
            connect(reloadWatcher, &QFileSystemWatcher::fileChanged,
                this, [this, reloadSignalPath]() {
                    if (QFileInfo::exists(reloadSignalPath) &&
                        !reloadWatcher->files().contains(reloadSignalPath))
                    {
                        reloadWatcher->addPath(reloadSignalPath);
                    }
                    reloadTimer->start(150);
                }
            );
            connect(reloadTimer, &QTimer::timeout,
                this, [this]() { restartNode(); }
            );

            if (!reloadWatcher->addPath(reloadSignalPath)) {
                qWarning() << "Could not watch development reload signal:"
                           << reloadSignalPath;
                window->close();
                return;
            }
        }

        // Menampilkan window utama.
        window->show();

        // Memulai proses Node.js.
        node->start();

        // Menunggu sampai proses Node.js berhasil dijalankan.
        if (!node->waitForStarted(5000)) {
            window->close();
            return;
        }

#ifdef Q_OS_WIN
        nodeJob = CreateJobObjectW(nullptr, nullptr);
        if (!nodeJob) {
            qWarning() << "Could not create Node.js process job:"
                       << GetLastError();
            node->kill();
            node->waitForFinished();
            window->close();
            return;
        }

        if (!attachNodeToJob()) {
            node->kill();
            node->waitForFinished();
            window->close();
        }
#endif
    }

    void restartNode()
    {
        if (node->state() != QProcess::NotRunning) {
            node->kill();
            node->waitForFinished();
        }

        buffer.clear();
        node->setProgram(nodePath);
        node->setArguments({mainFile});
        node->setWorkingDirectory(workingDir);
        node->start();

        if (!node->waitForStarted(5000)) {
            qWarning() << "Could not restart Node.js:" << node->errorString();
            return;
        }

#ifdef Q_OS_WIN
        if (!attachNodeToJob()) {
            qWarning() << "Could not manage restarted Node.js process:"
                       << GetLastError();
            node->kill();
            node->waitForFinished();
        }
#endif
    }

        bool eventFilter(QObject *watched, QEvent *event) override
        {
            if (auto *viewport = qobject_cast<QWidget *>(watched)) {
                if (event->type() == QEvent::MouseButtonPress) {
                    viewport->setProperty("juliaResizeInProgress", true);
                    viewport->setProperty("juliaPendingResizeColumn", -1);
                } else if (event->type() == QEvent::MouseButtonRelease) {
                    const QString eventId =
                        viewport->property("juliaTableResizeEvent").toString();
                    const int column =
                        viewport->property("juliaPendingResizeColumn").toInt();
                    const int width =
                        viewport->property("juliaPendingResizeWidth").toInt();
                    const bool resizeInProgress =
                        viewport->property("juliaResizeInProgress").toBool();
                    if (resizeInProgress && !eventId.isEmpty() &&
                        column >= 0 && width > 0) {
                        QJsonObject message;
                        message["type"] = "tableResize";
                        message["id"] = eventId;
                        message["columnIndex"] = column;
                        message["width"] = width;
                        node->write(
                            QJsonDocument(message)
                                .toJson(QJsonDocument::Compact) + "\n"
                        );
                        viewport->setProperty(
                            "juliaPendingResizeColumn",
                            -1
                        );
                    }
                    viewport->setProperty("juliaResizeInProgress", false);
                }
            }

            if (watched == window) {
                if (event->type() == QEvent::Close) {
                    const QString eventId = windowEvents["close"].toString();
                    if (!eventId.isEmpty() && !closeAuthorized) {
                        auto *closeEvent = static_cast<QCloseEvent *>(event);
                        closeEvent->ignore();
                        if (!closePending) {
                            closePending = true;
                            sendWindowEvent(eventId, "close");
                        }
                        return true;
                    }
                    closePending = false;
                    closeAuthorized = false;
                    return false;
                }

                if (event->type() == QEvent::Resize && !applyingWindowProps) {
                    positionFloatingActionButtons();
                    const QString eventId = windowEvents["resize"].toString();
                    if (!eventId.isEmpty()) {
                        const auto *resizeEvent =
                            static_cast<QResizeEvent *>(event);
                        sendWindowEvent(
                            eventId,
                            "resize",
                            resizeEvent->size().width(),
                            resizeEvent->size().height()
                        );
                    }
                }

                if (event->type() == QEvent::WindowStateChange) {
                    const Qt::WindowStates state = window->windowState();
                    if (hasWindowState && state != lastWindowState) {
                        if (state.testFlag(Qt::WindowMaximized) &&
                            !lastWindowState.testFlag(Qt::WindowMaximized))
                        {
                            const QString eventId =
                                windowEvents["maximize"].toString();
                            if (!eventId.isEmpty()) {
                                sendWindowEvent(eventId, "maximize");
                            }
                        }
                        if (state.testFlag(Qt::WindowMinimized) &&
                            !lastWindowState.testFlag(Qt::WindowMinimized))
                        {
                            const QString eventId =
                                windowEvents["minimize"].toString();
                            if (!eventId.isEmpty()) {
                                sendWindowEvent(eventId, "minimize");
                            }
                        }
                    }
                    lastWindowState = state;
                    hasWindowState = true;
                }
            }

            if (event->type() == QEvent::Resize) {
                if (QWidget *parent = qobject_cast<QWidget *>(watched)) {
                    updatePercentSizedChildren(parent);
                }
            } else if (event->type() == QEvent::Enter ||
                       event->type() == QEvent::Leave)
            {
                if (QWidget *widget = qobject_cast<QWidget *>(watched)) {
                    const QVariant hoverCursor =
                        widget->property("juliaHoverCursor");
                    if (event->type() == QEvent::Enter && hoverCursor.isValid()) {
                        widget->setCursor(
                            QCursor(static_cast<Qt::CursorShape>(
                                hoverCursor.toInt()
                            ))
                        );
                    } else if (event->type() == QEvent::Leave) {
                        const QVariant baseCursor =
                            widget->property("juliaBaseCursor");
                        if (baseCursor.isValid()) {
                            widget->setCursor(
                                QCursor(static_cast<Qt::CursorShape>(
                                    baseCursor.toInt()
                                ))
                            );
                        } else {
                            widget->unsetCursor();
                        }
                    }
                }
            }

            return QObject::eventFilter(watched, event);
        }

        void sendWindowEvent(
            const QString &id,
            const QString &event,
            int width = 0,
            int height = 0
        ) {
            QJsonObject message;
            message["type"] = "windowEvent";
            message["id"] = id;
            message["event"] = event;
            if (event == "resize") {
                message["width"] = width;
                message["height"] = height;
            }
            const QByteArray output =
                QJsonDocument(message).toJson(QJsonDocument::Compact);
            node->write(output + "\n");
        }

        void updatePercentSizedChildren(QWidget *parent)
        {
            const QRect available = parent->contentsRect();
            const auto children = parent->findChildren<QWidget *>(
                QString(),
                Qt::FindDirectChildrenOnly
            );

            for (QWidget *child : children) {
                const QVariant widthPercent =
                    child->property("juliaWidthPercent");
                const QVariant heightPercent =
                    child->property("juliaHeightPercent");
                const QVariant minWidthPercent =
                    child->property("juliaMinWidthPercent");
                const QVariant minHeightPercent =
                    child->property("juliaMinHeightPercent");
                const QVariant maxWidthPercent =
                    child->property("juliaMaxWidthPercent");
                const QVariant maxHeightPercent =
                    child->property("juliaMaxHeightPercent");

                if (widthPercent.isValid()) {
                    child->setFixedWidth(
                        qRound(available.width() * widthPercent.toDouble() / 100.0)
                    );
                }
                if (heightPercent.isValid()) {
                    child->setFixedHeight(
                        qRound(available.height() * heightPercent.toDouble() / 100.0)
                    );
                }
                if (minWidthPercent.isValid()) {
                    child->setMinimumWidth(
                        qRound(available.width() * minWidthPercent.toDouble() / 100.0)
                    );
                }
                if (minHeightPercent.isValid()) {
                    child->setMinimumHeight(
                        qRound(available.height() * minHeightPercent.toDouble() / 100.0)
                    );
                }
                if (maxWidthPercent.isValid()) {
                    child->setMaximumWidth(
                        qRound(available.width() * maxWidthPercent.toDouble() / 100.0)
                    );
                }
                if (maxHeightPercent.isValid()) {
                    child->setMaximumHeight(
                        qRound(available.height() * maxHeightPercent.toDouble() / 100.0)
                    );
                }
            }
        }

        bool cursorShapeFor(const QString &name, Qt::CursorShape *shape) const
        {
            static const QHash<QString, Qt::CursorShape> cursorShapes = {
                {"default", Qt::ArrowCursor},
                {"none", Qt::BlankCursor},
                {"context-menu", Qt::ArrowCursor},
                {"help", Qt::WhatsThisCursor},
                {"pointer", Qt::PointingHandCursor},
                {"progress", Qt::BusyCursor},
                {"wait", Qt::WaitCursor},
                {"cell", Qt::CrossCursor},
                {"crosshair", Qt::CrossCursor},
                {"text", Qt::IBeamCursor},
                {"vertical-text", Qt::IBeamCursor},
                {"alias", Qt::DragLinkCursor},
                {"copy", Qt::DragCopyCursor},
                {"move", Qt::SizeAllCursor},
                {"no-drop", Qt::ForbiddenCursor},
                {"not-allowed", Qt::ForbiddenCursor},
                {"grab", Qt::OpenHandCursor},
                {"grabbing", Qt::ClosedHandCursor},
                {"all-scroll", Qt::SizeAllCursor},
                {"col-resize", Qt::SizeHorCursor},
                {"row-resize", Qt::SizeVerCursor},
                {"e-resize", Qt::SizeHorCursor},
                {"w-resize", Qt::SizeHorCursor},
                {"ew-resize", Qt::SizeHorCursor},
                {"n-resize", Qt::SizeVerCursor},
                {"s-resize", Qt::SizeVerCursor},
                {"ns-resize", Qt::SizeVerCursor},
                {"ne-resize", Qt::SizeBDiagCursor},
                {"sw-resize", Qt::SizeBDiagCursor},
                {"nw-resize", Qt::SizeFDiagCursor},
                {"se-resize", Qt::SizeFDiagCursor},
                {"nesw-resize", Qt::SizeBDiagCursor},
                {"nwse-resize", Qt::SizeFDiagCursor},
                {"zoom-in", Qt::PointingHandCursor},
                {"zoom-out", Qt::PointingHandCursor}
            };
            if (!cursorShapes.contains(name)) return false;
            *shape = cursorShapes.value(name);
            return true;
        }

        QString cssPropertyName(const QString &name) const
        {
            if (name == "foregroundColor") return "color";

            QString result;
            for (const QChar character : name) {
                if (character.isUpper()) {
                    result += '-';
                    result += character.toLower();
                } else {
                    result += character;
                }
            }
            return result;
        }

        QString cssValue(const QString &property, const QJsonValue &value) const
        {
            if (value.isString()) {
                return value.toString();
            }
            if (!value.isDouble()) {
                return {};
            }

            const QString number = QString::number(value.toDouble());
            static const QSet<QString> lengthProperties = {
                "width", "height", "min-width", "min-height", "max-width",
                "max-height", "margin", "margin-left", "margin-right",
                "margin-top", "margin-bottom", "padding", "padding-left",
                "padding-right", "padding-top", "padding-bottom", "border-width",
                "border-radius", "font-size", "letter-spacing"
            };
            return lengthProperties.contains(property) ? number + "px" : number;
        }

        QString cssDeclarations(
            const QJsonObject &style,
            bool includePadding,
            bool includeGeometry
        ) const
        {
            static const QSet<QString> supportedProperties = {
                "background", "background-color", "color", "border",
                "border-color", "border-width", "border-style", "border-radius",
                "border-top-left-radius", "border-top-right-radius",
                "border-bottom-left-radius", "border-bottom-right-radius",
                "font", "font-family", "font-size", "font-weight", "font-style",
                "text-align", "padding", "padding-left", "padding-right",
                "padding-top", "padding-bottom", "margin", "margin-left",
                "margin-right", "margin-top", "margin-bottom", "width", "height",
                "min-width", "min-height", "max-width", "max-height",
                "letter-spacing", "selection-color", "selection-background-color"
            };
            QStringList declarations;

            for (auto it = style.begin(); it != style.end(); ++it) {
                const QString key = it.key();
                if (key.startsWith(':') || key == "cursor" || key == "flex" ||
                    key == "gap" || key == "flexDirection" ||
                    key == "justifyContent" || key == "alignItems")
                {
                    continue;
                }

                const QString property = cssPropertyName(key);
                if (!supportedProperties.contains(property)) continue;
                if (!includePadding && property.startsWith("padding")) continue;
                const bool isGeometry =
                    property == "width" || property == "height" ||
                    property.startsWith("min-") || property.startsWith("max-");
                if (!includeGeometry &&
                    isGeometry)
                {
                    continue;
                }
                if (isGeometry && it.value().isString() &&
                    it.value().toString().trimmed().endsWith('%'))
                {
                    continue;
                }

                const QString value = cssValue(property, it.value());
                if (!value.isEmpty()) {
                    declarations.append(
                        QString("%1: %2;").arg(property, value)
                    );
                }
            }

            if (style.contains("borderWidth") || style.contains("border-width")) {
                const QString width = cssValue(
                    "border-width",
                    style.contains("borderWidth")
                        ? style["borderWidth"]
                        : style["border-width"]
                );
                const QString borderStyle = style["borderStyle"].toString(
                    style["border-style"].toString("solid")
                );
                const QString borderColor = style["borderColor"].toString(
                    style["border-color"].toString("#000000")
                );
                if (!width.isEmpty()) {
                    declarations.append(
                        QString("border: %1 %2 %3;")
                            .arg(width, borderStyle, borderColor)
                    );
                }
            }

            return declarations.join('\n');
        }

        QString styleSheetFor(QWidget *widget, const QJsonObject &style,
                              bool includePadding)
        {
            const QString base = cssDeclarations(style, includePadding, true);
            const QString className = QString("juliaStyle%1")
                .arg(++styleObjectCounter);
            QString result = QString("#%1 { %2 }\n").arg(className, base);
            bool hasPseudoStyle = false;

            for (auto it = style.begin(); it != style.end(); ++it) {
                if (!it.key().startsWith(':') || !it.value().isObject()) continue;
                const QString state = it.key().mid(1);
                static const QSet<QString> supportedStates = {
                    "hover", "focus", "active", "pressed", "disabled"
                };
                if (!supportedStates.contains(state)) continue;

                const QString declarations = cssDeclarations(
                    it.value().toObject(),
                    includePadding,
                    true
                );
                if (declarations.isEmpty()) continue;
                const QString qtState = state == "active" ? "pressed" : state;
                result += QString("#%1:%2 { %3 }\n")
                    .arg(className, qtState, declarations);
                hasPseudoStyle = true;
            }

            if (!hasPseudoStyle) {
                return base;
            }

            widget->setObjectName(className);
            return result;
        }

#ifdef Q_OS_WIN
    bool attachNodeToJob()
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo{};
        jobInfo.BasicLimitInformation.LimitFlags =
            JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

        if (!SetInformationJobObject(
                nodeJob,
                JobObjectExtendedLimitInformation,
                &jobInfo,
                sizeof(jobInfo)
            ))
        {
            return false;
        }

        HANDLE nodeProcess = OpenProcess(
            PROCESS_SET_QUOTA | PROCESS_TERMINATE,
            FALSE,
            static_cast<DWORD>(node->processId())
        );
        if (!nodeProcess) {
            return false;
        }

        const bool assigned =
            AssignProcessToJobObject(nodeJob, nodeProcess);
        CloseHandle(nodeProcess);
        return assigned;
    }
#endif

    QWidget *createNode(const QJsonObject &data)
    {
        QString type = data["type"].toString();

        QJsonObject props = data["props"].toObject();
        QJsonObject events = data["events"].toObject();

        if (type == "Text") {
            QLabel *label = new QLabel(props["text"].toString());

            applyStyle(
                label,
                props["style"].toObject(),
                props,
                true
            );

            return label;
        }

        if (type == "TextInput") {
            QLineEdit *input = new QLineEdit();
            const QString inputType = props["type"].toString("text");

            if (props["autoFocus"].toBool()) {
                autoFocusInput = input;
            }

            input->setText(props["value"].toVariant().toString());
            input->setPlaceholderText(props["placeholder"].toString());

            if (inputType == "password") {
                input->setEchoMode(QLineEdit::Password);
            } else if (inputType == "number") {
                auto *validator = new QDoubleValidator(input);
                validator->setNotation(QDoubleValidator::StandardNotation);
                input->setValidator(validator);
            } else if (inputType == "email") {
                input->setInputMethodHints(
                    Qt::ImhEmailCharactersOnly | Qt::ImhNoAutoUppercase
                );
            }

            applyStyle(input, props["style"].toObject(), {}, true);

            const QString eventId = events["change"].toString();
            connect(input, &QLineEdit::textEdited,
                this, [this, eventId](const QString &value) {
                    if (eventId.isEmpty()) return;

                    QJsonObject message;
                    message["type"] = "change";
                    message["id"] = eventId;
                    message["value"] = value;

                    const QByteArray data =
                        QJsonDocument(message)
                            .toJson(QJsonDocument::Compact);

                    node->write(data + "\n");
                }
            );

            return input;
        }

        if (type == "Button" || type == "FloatingActionButton") {
            const bool isFloatingActionButton =
                type == "FloatingActionButton";
            QPushButton *button = new QPushButton(
                isFloatingActionButton
                    ? props["icon"].toString("+")
                    : props["text"].toString()
            );

            if (isFloatingActionButton) {
                button->setAccessibleName(
                    props["accessibilityLabel"].toString()
                );
            }

            applyStyle(button, props["style"].toObject(), {}, true);

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

        if (type == "Row") {
            return createRow(data);
        }

        if (type == "Column") {
            return createColumn(data);
        }

        if (type == "View") {
            return createView(data);
        }

        if (type == "Table") {
            return createTable(data);
        }

        if (type == "Modal") {
            createModal(data);
            return nullptr;
        }

        return nullptr;
    }

    void applyTableItemStyle(
        QTableWidgetItem *item,
        const QJsonObject &style
    ) {
        const QString foreground = style["color"].toString();
        if (!foreground.isEmpty()) {
            item->setForeground(QBrush(QColor(foreground)));
        }

        const QString background = style["backgroundColor"].toString(
            style["background-color"].toString()
        );
        if (!background.isEmpty()) {
            item->setBackground(QBrush(QColor(background)));
        }

        QFont font = item->font();
        if (style["fontWeight"].toString() == "bold") {
            font.setBold(true);
        }
        if (style["fontSize"].isDouble()) {
            font.setPointSizeF(style["fontSize"].toDouble());
        }
        item->setFont(font);

        const QString alignment = style["textAlign"].toString();
        if (alignment == "center") {
            item->setTextAlignment(Qt::AlignCenter);
        } else if (alignment == "right") {
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        } else {
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    QWidget *createTable(const QJsonObject &data)
    {
        const QJsonObject props = data["props"].toObject();
        const QJsonObject events = data["events"].toObject();
        const QJsonArray columns = props["columns"].toArray();
        const QJsonArray children = data["children"].toArray();
        const int rowCount = props["rowCount"].toInt();
        const bool hasEmptyContent = props["empty"].toBool(false) &&
            !children.isEmpty();
        const int nativeRows = rowCount > 0 ? rowCount : hasEmptyContent ? 1 : 0;
        const int nativeColumns = columns.size();

        auto *table = new QTableWidget(nativeRows, nativeColumns);
        table->setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Expanding
        );
        table->setAlternatingRowColors(true);
        table->setShowGrid(true);
        table->setWordWrap(false);
        table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::ExtendedSelection);
        table->verticalHeader()->setVisible(false);
        table->verticalHeader()->setDefaultSectionSize(32);

        applyStyle(table, props["style"].toObject());

        const QJsonObject headerStyle = props["headerStyle"].toObject();
        QHeaderView *header = table->horizontalHeader();
        header->setSectionsClickable(true);
        header->setSectionsMovable(false);
        header->setSortIndicatorShown(true);
        header->setSectionResizeMode(QHeaderView::Interactive);
        header->setStretchLastSection(true);
        header->setDefaultSectionSize(140);
        if (!headerStyle.isEmpty()) {
            applyStyle(header, headerStyle);
        }

        QList<int> columnMinimumWidths;
        for (int columnIndex = 0; columnIndex < nativeColumns; ++columnIndex) {
            const QJsonObject column = columns[columnIndex].toObject();
            const QString title = column["title"].toString();
            auto *headerItem = new QTableWidgetItem(title);
            applyTableItemStyle(headerItem, props["headerTextStyle"].toObject());
            table->setHorizontalHeaderItem(columnIndex, headerItem);

            const int minWidth = styleLengthPixels(
                column["minWidth"],
                40
            );
            columnMinimumWidths.append(qMax(24, minWidth));

            const int width = styleLengthPixels(column["width"]);
            if (width >= 0) {
                header->resizeSection(
                    columnIndex,
                    qMax(columnMinimumWidths[columnIndex], width)
                );
            }
        }

        int childIndex = 0;
        for (int row = 0; row < rowCount; ++row) {
            for (int column = 0; column < nativeColumns; ++column) {
                const QJsonObject cell =
                    childIndex < children.size()
                        ? children[childIndex++].toObject()
                        : QJsonObject();
                const QJsonObject cellProps = cell["props"].toObject();
                const QString text = cell["type"].toString() == "Text"
                    ? cellProps["text"].toString()
                    : QString();
                auto *item = new QTableWidgetItem(text);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                applyTableItemStyle(item, props["rowStyle"].toObject());
                applyTableItemStyle(item, props["cellStyle"].toObject());
                if (cell["type"].toString() == "Text") {
                    applyTableItemStyle(item, cellProps["style"].toObject());
                }
                table->setItem(row, column, item);

                if (cell["type"].toString() != "Text") {
                    if (QWidget *cellWidget = createNode(cell)) {
                        table->setCellWidget(row, column, cellWidget);
                    }
                }
            }
        }

        if (hasEmptyContent && nativeColumns > 0) {
            QWidget *emptyWidget = createNode(children[0].toObject());
            if (emptyWidget) {
                table->setCellWidget(0, 0, emptyWidget);
                if (nativeColumns > 1) {
                    table->setSpan(0, 0, 1, nativeColumns);
                }
            }
        }

        int sortColumn = props["sortColumn"].toInt(-1);
        bool sortAscending = props["sortAscending"].toBool(true);
        if (sortColumn >= 0 && sortColumn < nativeColumns) {
            header->setSortIndicator(
                sortColumn,
                sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder
            );
        } else {
            header->setSortIndicator(-1, Qt::AscendingOrder);
        }

        const QString sortEventId = events["sort"].toString();
        const QString resizeEventId = events["resize"].toString();
        connect(header, &QHeaderView::sectionClicked, this,
            [this, table, columns, sortEventId](int columnIndex) {
                if (columnIndex < 0 || columnIndex >= columns.size()) return;
                if (!columns[columnIndex].toObject()["sortable"].toBool(true)) {
                    return;
                }

                const int previousColumn =
                    table->property("juliaSortColumn").toInt();
                const bool ascending =
                    previousColumn != columnIndex ||
                    !table->property("juliaSortAscending").toBool();
                table->setProperty("juliaSortColumn", columnIndex);
                table->setProperty("juliaSortAscending", ascending);

                table->horizontalHeader()->setSortIndicator(
                    columnIndex,
                    ascending ? Qt::AscendingOrder : Qt::DescendingOrder
                );
                if (!sortEventId.isEmpty()) {
                    QJsonObject message;
                    message["type"] = "tableSort";
                    message["id"] = sortEventId;
                    message["columnIndex"] = columnIndex;
                    message["ascending"] = ascending;
                    node->write(
                        QJsonDocument(message)
                            .toJson(QJsonDocument::Compact) + "\n"
                    );
                }
            }
        );

        table->setProperty("juliaSortColumn", sortColumn);
        table->setProperty("juliaSortAscending", sortAscending);

        QWidget *headerViewport = header->viewport();
        headerViewport->setProperty("juliaTableResizeEvent", resizeEventId);
        headerViewport->setProperty("juliaPendingResizeColumn", -1);
        headerViewport->setProperty("juliaResizeInProgress", false);
        headerViewport->installEventFilter(this);
        connect(header, &QHeaderView::sectionResized, this,
            [table, header, headerViewport, columnMinimumWidths](
                int section,
                int,
                int width
            ) {
                if (section < 0 || section >= columnMinimumWidths.size()) return;
                const int minimum = columnMinimumWidths[section];
                if (width < minimum) {
                    const QSignalBlocker blocker(header);
                    header->resizeSection(section, minimum);
                    width = minimum;
                }
                int otherSectionsWidth = 0;
                for (int column = 0; column < header->count(); ++column) {
                    if (column != section) {
                        otherSectionsWidth += header->sectionSize(column);
                    }
                }
                const int maximum = qMax(
                    minimum,
                    table->viewport()->width() - otherSectionsWidth
                );
                if (width > maximum) {
                    const QSignalBlocker blocker(header);
                    header->resizeSection(section, maximum);
                    width = maximum;
                }
                if (!headerViewport->property(
                        "juliaResizeInProgress"
                    ).toBool()) {
                    return;
                }
                headerViewport->setProperty(
                    "juliaPendingResizeColumn",
                    section
                );
                headerViewport->setProperty(
                    "juliaPendingResizeWidth",
                    width
                );
            }
        );
        return table;
    }

    void clearModalDialogs()
    {
        for (QDialog *dialog : modalDialogs) {
            if (!dialog) continue;
            QObject::disconnect(dialog, nullptr, this, nullptr);
            dialog->close();
            delete dialog;
        }
        modalDialogs.clear();
    }

    void clearFloatingActionButtons()
    {
        for (QWidget *button : floatingActionButtons) {
            delete button;
        }
        floatingActionButtons.clear();
    }

    void positionFloatingActionButtons()
    {
        for (QWidget *button : floatingActionButtons) {
            if (!button) continue;

            const int margin = qMax(
                0,
                button->property("juliaFabMargin").toInt()
            );
            const QString placement =
                button->property("juliaFabPlacement").toString();
            const bool right = placement.endsWith("right");
            const bool bottom = placement.startsWith("bottom");
            const int x = right
                ? qMax(margin, window->width() - button->width() - margin)
                : margin;
            const int y = bottom
                ? qMax(margin, window->height() - button->height() - margin)
                : margin;

            button->move(x, y);
            button->raise();
        }
    }

    void createModal(const QJsonObject &data)
    {
        const QJsonObject props = data["props"].toObject();
        if (!props["visible"].toBool(false)) return;

        const QJsonObject style = props["style"].toObject();
        const QJsonObject events = data["events"].toObject();
        auto *dialog = new QDialog(window);
        dialog->setWindowTitle(props["title"].toString());
        dialog->setWindowModality(Qt::ApplicationModal);

        const int width = styleLengthPixels(
            styleValue(style, "width", "width"),
            props["width"].toInt(420)
        );
        const int height = styleLengthPixels(
            styleValue(style, "height", "height"),
            props["height"].toInt(280)
        );
        dialog->setFixedSize(width, height);

        auto *layout = new QVBoxLayout(dialog);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        QWidget *content = createFlexContainer(data, "column");
        layout->addWidget(content);
        modalDialogs.append(dialog);

        const QString closeEventId = events["close"].toString();
        if (!closeEventId.isEmpty()) {
            connect(dialog, &QDialog::finished, this,
                [this, closeEventId](int) {
                    QJsonObject message;
                    message["type"] = "modalClose";
                    message["id"] = closeEventId;
                    node->write(
                        QJsonDocument(message)
                            .toJson(QJsonDocument::Compact) + "\n"
                    );
                }
            );
        }

        QTimer::singleShot(0, dialog, [this, dialog]() {
            if (!dialog->isVisible()) {
                dialog->move(
                    window->geometry().center() - dialog->rect().center()
                );
                dialog->open();
            }
        });
    }

    QWidget *createView(const QJsonObject &data)
    {
        QJsonObject props = data["props"].toObject();
        QString direction = props["style"].toObject()["flexDirection"]
            .toString(props["direction"].toString("column"));

        QWidget *content = createFlexContainer(data, direction);
        if (!props["scrollable"].toBool(false)) {
            return content;
        }

        auto *scrollArea = new QScrollArea();
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollArea->setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Expanding
        );
        scrollArea->setWidget(content);

        const QString scrollDirection =
            props["scrollDirection"].toString("auto");
        scrollArea->setHorizontalScrollBarPolicy(
            scrollDirection == "vertical"
                ? Qt::ScrollBarAlwaysOff
                : Qt::ScrollBarAsNeeded
        );
        scrollArea->setVerticalScrollBarPolicy(
            scrollDirection == "horizontal"
                ? Qt::ScrollBarAlwaysOff
                : Qt::ScrollBarAsNeeded
        );

        return scrollArea;
    }

    QWidget *createRow(const QJsonObject &data)
    {
        return createFlexContainer(data, "row");
    }

    QWidget *createColumn(const QJsonObject &data)
    {
        return createFlexContainer(data, "column");
    }
    
    QWidget *createFlexContainer(
        const QJsonObject &data,
        const QString &direction
    ) {
        QJsonObject props = data["props"].toObject();
        QJsonObject style = props["style"].toObject();
        QJsonArray children = data["children"].toArray();

        QWidget *container = new QWidget();

        // 1. PASTIKAN CONTAINER INI BISA MEMBESAR SECARA MAKSIMAL
        container->setSizePolicy(
            QSizePolicy::Expanding,
            QSizePolicy::Expanding
        );

        applyStyle(container, style);

        QBoxLayout *layout;
        bool isRow = (direction == "row");

        if (isRow) {
            layout = new QHBoxLayout(container);
        } else {
            layout = new QVBoxLayout(container);
        }

        int padding = styleLengthPixels(
            props["padding"],
            styleLengthPixels(styleValue(style, "padding", "padding"), 0)
        );
        int spacing = styleLengthPixels(
            props["spacing"],
            styleLengthPixels(styleValue(style, "gap", "gap"), 0)
        );

        QString justify = props["justifyContent"].toString(
            style["justifyContent"].toString("flex-start")
        );
        QString align = props["alignItems"].toString(
            style["alignItems"].toString("stretch")
        );

        if (justify == "flex-start") justify = "start";
        if (justify == "flex-end") justify = "end";
        if (align == "flex-start") align = "start";
        if (align == "flex-end") align = "end";

        container->installEventFilter(this);
        layout->setContentsMargins(
            padding, padding, padding, padding
        );
        layout->setSpacing(0);

        struct FlexItem {
            QWidget *widget;
            int flex;
        };

        QList<FlexItem> items;
        int totalFlex = 0;

     // Membuat children
        for (const QJsonValue &child : children) {
            QJsonObject childData = child.toObject();
            QWidget *widget = createNode(childData);

            if (!widget) continue;

            QJsonObject childProps = childData["props"].toObject();
            QJsonObject childStyle = childProps["style"].toObject();
            int flex = qMax(0, childProps["flex"].toInt(childStyle["flex"].toInt(0)));
            int margin = styleLengthPixels(
                styleValue(childStyle, "margin", "margin"),
                0
            );
            const bool fixedWidth = widget->minimumWidth() == widget->maximumWidth();
            const bool fixedHeight = widget->minimumHeight() == widget->maximumHeight();

            QWidget *layoutWidget = widget;
            if (margin > 0) {
                layoutWidget = new QWidget();
                layoutWidget->installEventFilter(this);
                QBoxLayout *marginLayout = new QBoxLayout(
                    isRow ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom,
                    layoutWidget
                );
                marginLayout->setContentsMargins(margin, margin, margin, margin);
                marginLayout->setSpacing(0);
                marginLayout->addWidget(widget);
                updatePercentSizedChildren(layoutWidget);
            }

            QSizePolicy policy = layoutWidget->sizePolicy();

            // PERBAIKAN UTAMA:
            // Jika flex > 0, kita biarkan widget meregang. 
            // TAPI jika ini adalah container (View/Row/Column) yang membungkus komponen lain 
            // dan justify/align-nya center, jangan paksa horizontal/vertical policy jadi Expanding 
            // kecuali memang diinginkan memenuhi layar.
            if (isRow) {
                if (flex > 0 && !fixedWidth) {
                    policy.setHorizontalPolicy(QSizePolicy::Expanding);
                } else if (!fixedWidth) {
                    // Jika tidak punya flex, biarkan ukurannya pas dengan isinya (Preferred/Fixed)
                    policy.setHorizontalPolicy(QSizePolicy::Preferred);
                }

                if (align == "stretch" && !fixedHeight) {
                    policy.setVerticalPolicy(QSizePolicy::Expanding);
                } else if (!fixedHeight) {
                    policy.setVerticalPolicy(QSizePolicy::Preferred);
                }
            } else {
                if (flex > 0 && !fixedHeight) {
                    policy.setVerticalPolicy(QSizePolicy::Expanding);
                } else if (!fixedHeight) {
                    policy.setVerticalPolicy(QSizePolicy::Preferred);
                }

                if (align == "stretch" && !fixedWidth) {
                    policy.setHorizontalPolicy(QSizePolicy::Expanding);
                } else if (!fixedWidth) {
                    policy.setHorizontalPolicy(QSizePolicy::Preferred);
                }
            }

            layoutWidget->setSizePolicy(policy);

            items.append({layoutWidget, flex});
            totalFlex += flex;
        }
        int count = items.size();

        // Menentukan alignment cross-axis
        Qt::Alignment alignment;
        if (align == "center") {
            alignment = isRow ? Qt::AlignVCenter : Qt::AlignHCenter;
        } else if (align == "start") {
            alignment = isRow ? Qt::AlignTop : Qt::AlignLeft;
        } else if (align == "end") {
            alignment = isRow ? Qt::AlignBottom : Qt::AlignRight;
        } else {
            alignment = {}; // stretch
        }

        const bool useJustify = totalFlex == 0;

        if (useJustify &&
            (justify == "center" || justify == "end" || justify == "space-around")) {
            layout->addStretch(1);
        }

        for (int i = 0; i < count; i++) {
            if (i > 0) {
                layout->addSpacing(spacing);

                if (useJustify && justify == "space-between") {
                    layout->addStretch(1);
                } else if (useJustify && justify == "space-around") {
                    layout->addStretch(2);
                }
            }

            layout->addWidget(
                items[i].widget,
                items[i].flex,
                alignment
            );
        }

        if (useJustify &&
            (justify == "start" || justify == "center" || justify == "space-around" ||
             (justify == "space-between" && count == 1))) {
            layout->addStretch(1);
        }

        updatePercentSizedChildren(container);
        return container;
    }

    int styleLengthPixels(const QJsonValue &value, int fallback = -1) const
    {
        if (value.isDouble()) return value.toInt();
        if (!value.isString()) return fallback;

        QString text = value.toString().trimmed();
        double scale = 1.0;
        if (text.endsWith("px")) {
            text.chop(2);
        } else if (text.endsWith("pt")) {
            text.chop(2);
            scale = 96.0 / 72.0;
        } else {
            return fallback;
        }

        bool ok = false;
        const double amount = text.toDouble(&ok);
        return ok ? qRound(amount * scale) : fallback;
    }

    QJsonValue styleValue(
        const QJsonObject &style,
        const QString &camelName,
        const QString &cssName
    ) const {
        return style.contains(camelName) ? style[camelName] : style[cssName];
    }

    void setStyledDimension(
        QWidget *widget,
        const QJsonObject &style,
        const QString &property,
        bool minimum,
        bool maximum
    ) {
        QString cssName;
        for (const QChar character : property) {
            if (character.isUpper()) {
                cssName += '-';
                cssName += character.toLower();
            } else {
                cssName += character;
            }
        }
        const QJsonValue value = styleValue(style, property, cssName);
        if (value.isString() && value.toString().trimmed().endsWith('%')) {
            QString amount = value.toString().trimmed();
            amount.chop(1);
            bool ok = false;
            const double percent = amount.toDouble(&ok);
            if (ok && percent >= 0.0) {
                const bool isWidth =
                    property.endsWith("Width") || property == "width";
                const QString dimension = isWidth ? "Width" : "Height";
                const QString constraint = minimum
                    ? "Min"
                    : maximum ? "Max" : "";
                const QString percentProperty =
                    "julia" + constraint + dimension + "Percent";
                widget->setProperty(percentProperty.toUtf8().constData(), percent);
            }
            return;
        }

        const int pixels = styleLengthPixels(value);
        if (pixels < 0) return;
        const bool isWidth = property.endsWith("Width") || property == "width";
        if (minimum) {
            if (isWidth) widget->setMinimumWidth(pixels);
            else widget->setMinimumHeight(pixels);
        } else if (maximum) {
            if (isWidth) widget->setMaximumWidth(pixels);
            else widget->setMaximumHeight(pixels);
        } else if (isWidth) {
            widget->setFixedWidth(pixels);
        } else {
            widget->setFixedHeight(pixels);
        }
    }

    void applyStyle(
        QWidget *widget,
        const QJsonObject &style,
        const QJsonObject &fallbackProps = QJsonObject(),
        bool includePadding = false
    ) {
        const QString cursor = style["cursor"].toString();
        Qt::CursorShape baseCursor;
        if (cursor == "auto") {
            widget->unsetCursor();
            widget->setProperty("juliaBaseCursor", QVariant());
        } else if (cursorShapeFor(cursor, &baseCursor)) {
            widget->setCursor(QCursor(baseCursor));
            widget->setProperty("juliaBaseCursor", static_cast<int>(baseCursor));
        }

        const QJsonObject hoverStyle = style[":hover"].toObject();
        Qt::CursorShape hoverCursor;
        const QString hoverCursorName = hoverStyle["cursor"].toString();
        if (cursorShapeFor(hoverCursorName, &hoverCursor)) {
            widget->setProperty("juliaHoverCursor", static_cast<int>(hoverCursor));
            widget->installEventFilter(this);
        } else {
            widget->setProperty("juliaHoverCursor", QVariant());
        }

        setStyledDimension(widget, style, "minWidth", true, false);
        setStyledDimension(widget, style, "minHeight", true, false);
        setStyledDimension(widget, style, "maxWidth", false, true);
        setStyledDimension(widget, style, "maxHeight", false, true);
        setStyledDimension(widget, style, "width", false, false);
        setStyledDimension(widget, style, "height", false, false);

        QJsonObject effectiveStyle = style;
        if (!effectiveStyle.contains("color") &&
            fallbackProps.contains("foregroundColor"))
        {
            effectiveStyle["color"] = fallbackProps["foregroundColor"];
        }
        if (!effectiveStyle.contains("fontSize") &&
            fallbackProps.contains("fontSize"))
        {
            effectiveStyle["fontSize"] = fallbackProps["fontSize"];
        }
        if (!effectiveStyle.contains("fontWeight") &&
            fallbackProps.contains("fontWeight"))
        {
            effectiveStyle["fontWeight"] = fallbackProps["fontWeight"];
        }

        const QString styleSheet = styleSheetFor(
            widget,
            effectiveStyle,
            includePadding
        );
        if (!styleSheet.trimmed().isEmpty()) {
            widget->setAttribute(Qt::WA_StyledBackground, true);
            widget->setStyleSheet(styleSheet);
        }
    }

    Qt::Alignment getAlignment(
    const QString &align,
    const QString &direction
    ) {
        bool isRow = direction == "row";

        if (align == "center") {
            return isRow ? Qt::AlignVCenter : Qt::AlignHCenter;
        }

        if (align == "end") {
            return isRow ? Qt::AlignBottom : Qt::AlignRight;
        }

        if (align == "start") {
            return isRow ? Qt::AlignTop : Qt::AlignLeft;
        }

        // stretch: tidak memberikan alignment khusus
        return {};
    }

    void addJustifyStart(QBoxLayout *layout)
    {
        layout->addStretch(1);
    }

    void addJustifyCenter(QBoxLayout *layout)
    {
        layout->insertStretch(0, 1);
        layout->addStretch(1);
    }

    void addJustifyEnd(QBoxLayout *layout)
    {
        layout->insertStretch(0, 1);
    }

    // Menerima JSON dari Node.js dan merender UI ke window Qt.
    void renderUI(const QByteArray &json)
    {
        // Menyiapkan pemeriksaan format JSON.
        QJsonParseError error;

        // Mengubah data JSON teks menjadi QJsonDocument.
        QJsonDocument document =
            QJsonDocument::fromJson(json, &error);

        // Menghentikan proses jika JSON tidak valid atau bukan object.
        if (error.error != QJsonParseError::NoError ||
            !document.isObject())
        {
            return;
        }

        // Mengambil object utama dan properti window.
        QJsonObject root = document.object();
        clearModalDialogs();
        QJsonObject props = root["props"].toObject();
        QJsonObject style = props["style"].toObject();
        windowEvents = root["events"].toObject();

        // Mengatur judul window dari properti Widget.
        window->setWindowTitle(
            props["title"].toString("JuliaNative")
        );

        applyingWindowProps = true;
        const QSize requestedSize(
            props["width"].toInt(500),
            props["height"].toInt(350)
        );
        const bool windowSizeChanged =
            !hasAppliedWindowSize || requestedSize != lastAppliedWindowSize;

        hasRequestedPosition = props["position"].isObject();
        if (hasRequestedPosition) {
            const QJsonObject position = props["position"].toObject();
            requestedPosition = QPoint(
                position["x"].toInt(),
                position["y"].toInt()
            );
        }

        const bool alwaysOnTop = props["alwaysOnTop"].toBool(false);
        const bool hideTitleBar = props["hideTitleBar"].toBool(false);
        const bool windowFlagsChanged =
            !hasAppliedWindowFlags ||
            windowAlwaysOnTop != alwaysOnTop ||
            windowHideTitleBar != hideTitleBar;
        if (windowFlagsChanged) {
            Qt::WindowFlags flags = window->windowFlags();
            flags.setFlag(Qt::WindowStaysOnTopHint, alwaysOnTop);
            flags.setFlag(Qt::FramelessWindowHint, hideTitleBar);
            flags.setFlag(Qt::WindowTitleHint, !hideTitleBar);
            window->setWindowFlags(flags);
            windowAlwaysOnTop = alwaysOnTop;
            windowHideTitleBar = hideTitleBar;
            hasAppliedWindowFlags = true;
        }

        const bool resizable = props["resizable"].toBool(true);
        const bool resizableChanged = windowResizable != resizable;
        if (resizableChanged && resizable) {
            window->setMinimumSize(0, 0);
            window->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
        if (resizableChanged || windowSizeChanged) {
            if (resizable) window->resize(requestedSize);
            else window->setFixedSize(requestedSize);
            windowResizable = resizable;
            lastAppliedWindowSize = requestedSize;
            hasAppliedWindowSize = true;
        }
        applyStyle(window, style);

        const bool fullscreen = props["fullscreen"].toBool(false);
        const bool leavingFullscreen = !fullscreen && window->isFullScreen();
        if (fullscreen && !window->isFullScreen()) {
            window->showFullScreen();
        } else if (leavingFullscreen) {
            window->showNormal();
        }

        const bool scrollable = props["scrollable"].toBool(false);
        if (!scrollArea) {
            auto *windowLayout = new QVBoxLayout(window);
            windowLayout->setContentsMargins(0, 0, 0, 0);
            windowLayout->setSpacing(0);

            scrollArea = new QScrollArea(window);
            scrollArea->setFrameShape(QFrame::NoFrame);
            scrollArea->setWidgetResizable(true);
            scrollContent = new QWidget(scrollArea);
            scrollContent->installEventFilter(this);
            contentLayout = new QVBoxLayout(scrollContent);
            scrollArea->setWidget(scrollContent);
            windowLayout->addWidget(scrollArea);
        }
        scrollArea->setHorizontalScrollBarPolicy(
            scrollable ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff
        );
        scrollArea->setVerticalScrollBarPolicy(
            scrollable ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff
        );

        QVBoxLayout *layout = contentLayout;
        if (!layout) {
            qWarning() << "Widget content layout is unavailable.";
            applyingWindowProps = false;
            return;
        }

        /*
         * Root children live in the scroll area's content widget so the
         * window can enable scrolling without rebuilding its native shell.
         */
        layout->setContentsMargins(0, 0, 0, 0);

        /*
         * Keep the root layout reusable across state-driven renders.
         */
       int focusedInputIndex = -1;
       int focusedCursorPosition = -1;
       if (auto *focusedInput = qobject_cast<QLineEdit *>(
               QApplication::focusWidget()
           ))
       {
           const QList<QLineEdit *> previousInputs =
               window->findChildren<QLineEdit *>();
           focusedInputIndex = previousInputs.indexOf(focusedInput);
           focusedCursorPosition = focusedInput->cursorPosition();
       }

       const int padding = style["padding"].toInt(0);
        layout->setContentsMargins(padding, padding, padding, padding);
        layout->setSpacing(style["gap"].toInt(0));

        // Menghapus seluruh widget dari tampilan sebelumnya.
        autoFocusInput = nullptr;
        clearFloatingActionButtons();
        while (QLayoutItem *item = layout->takeAt(0)) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }

            delete item;
        }

        // Mengambil children dari root JSON.
        QJsonArray children = root["children"].toArray();

        // Membuat dan menambahkan children ke window dengan dukungan flex
        for (const QJsonValue &child : children) {
            QJsonObject childData = child.toObject();
            QWidget *widget = createNode(childData);

            if (widget) {
                if (childData["type"].toString() ==
                    "FloatingActionButton")
                {
                    const QJsonObject fabProps =
                        childData["props"].toObject();
                    widget->setParent(window);
                    widget->setProperty(
                        "juliaFabPlacement",
                        fabProps["placement"].toString("bottom-right")
                    );
                    widget->setProperty(
                        "juliaFabMargin",
                        fabProps["margin"].toInt(24)
                    );
                    floatingActionButtons.append(widget);
                    widget->show();
                    continue;
                }

                // Membaca properti flex untuk elemen root-level juga
                QJsonObject childProps = childData["props"].toObject();
                int flex = qMax(0, childProps["flex"].toInt(0));

                layout->addWidget(widget, flex);
            }
        }

        // Menampilkan window setelah UI selesai dibuat.
        updatePercentSizedChildren(scrollContent);
        updatePercentSizedChildren(window);
        window->show();
        positionFloatingActionButtons();
        const bool positionChanged =
            hasRequestedPosition &&
            (!hasAppliedPosition || requestedPosition != lastAppliedPosition);
        if (hasRequestedPosition &&
            (positionChanged || windowFlagsChanged || leavingFullscreen))
        {
            window->windowHandle()->setPosition(requestedPosition);
            lastAppliedPosition = requestedPosition;
            hasAppliedPosition = true;
        }
        lastWindowState = window->windowState();
        hasWindowState = true;
        applyingWindowProps = false;
        const QList<QLineEdit *> currentInputs =
            window->findChildren<QLineEdit *>();
        if (focusedInputIndex >= 0 &&
            focusedInputIndex < currentInputs.size())
        {
            QLineEdit *focusedInput = currentInputs[focusedInputIndex];
            focusedInput->setFocus(Qt::OtherFocusReason);
            if (focusedCursorPosition >= 0) {
                focusedInput->setCursorPosition(
                    qMin(focusedCursorPosition, focusedInput->text().size())
                );
            }
        } else if (autoFocusInput) {
            autoFocusInput->setFocus(Qt::OtherFocusReason);
        }
    }

    // Destructor: menghentikan Node.js dan membersihkan window.
    ~JuliaRuntime()
    {
        // Memastikan proses Node.js berhenti sebelum runtime dihancurkan.
        if (node->state() != QProcess::NotRunning) {
            node->kill();
            node->waitForFinished();
        }

#ifdef Q_OS_WIN
        if (nodeJob) {
            CloseHandle(nodeJob);
        }
#endif

        // Menghapus window utama.
        delete window;
    }
};

// Entry point aplikasi JuliaNative.
int main(int argc, char *argv[])
{
    // Membuat aplikasi Qt.
    QApplication app(argc, argv);

    // Membuat runtime JuliaNative.
    JuliaRuntime runtime;

    // Menjalankan Node.js dan menampilkan window.
    runtime.start(app.arguments().contains("--dev"));

    // Menjalankan event loop Qt.
    return app.exec();
}