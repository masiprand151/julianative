
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
    QLineEdit *autoFocusInput = nullptr;

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

    // Constructor: menyiapkan window dan koneksi komunikasi Node.js.
    JuliaRuntime()
    {
        node = new QProcess(this);
        window = new QWidget();
        // Memisahkan output stdout dan stderr Node.js.
        node->setProcessChannelMode(QProcess::SeparateChannels);

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

        if (type == "Button") {
            QPushButton *button = new QPushButton(
                props["text"].toString()
            );

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

        return nullptr;
    }

    QWidget *createView(const QJsonObject &data)
    {
        QJsonObject props = data["props"].toObject();
        QString direction = props["style"].toObject()["flexDirection"]
            .toString(props["direction"].toString("column"));

        return createFlexContainer(data, direction);
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

        int padding = props["padding"].toInt(style["padding"].toInt(0));
        int spacing = props["spacing"].toInt(style["gap"].toInt(0));

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
            int margin = childStyle["margin"].toInt(0);
            const bool fixedWidth = widget->minimumWidth() == widget->maximumWidth();
            const bool fixedHeight = widget->minimumHeight() == widget->maximumHeight();

            QWidget *layoutWidget = widget;
            if (margin > 0) {
                layoutWidget = new QWidget();
                QBoxLayout *marginLayout = new QBoxLayout(
                    isRow ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom,
                    layoutWidget
                );
                marginLayout->setContentsMargins(margin, margin, margin, margin);
                marginLayout->setSpacing(0);
                marginLayout->addWidget(widget);
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

        return container;
    }

    void applyStyle(
        QWidget *widget,
        const QJsonObject &style,
        const QJsonObject &fallbackProps = QJsonObject(),
        bool includePadding = false
    ) {
        const int minWidth = style["minWidth"].toInt(-1);
        const int minHeight = style["minHeight"].toInt(-1);
        const int maxWidth = style["maxWidth"].toInt(-1);
        const int maxHeight = style["maxHeight"].toInt(-1);
        const int width = style["width"].toInt(-1);
        const int height = style["height"].toInt(-1);

        if (minWidth >= 0) widget->setMinimumWidth(minWidth);
        if (minHeight >= 0) widget->setMinimumHeight(minHeight);
        if (maxWidth >= 0) widget->setMaximumWidth(maxWidth);
        if (maxHeight >= 0) widget->setMaximumHeight(maxHeight);
        if (width >= 0) widget->setFixedWidth(width);
        if (height >= 0) widget->setFixedHeight(height);

        QString styleSheet = widget->styleSheet();
        const QString backgroundColor = style["backgroundColor"].toString();
        const QString color = style["color"].toString(
            fallbackProps["foregroundColor"].toString()
        );
        const int borderWidth = style["borderWidth"].toInt(0);
        const int borderRadius = style["borderRadius"].toInt(0);
        const int fontSize = style["fontSize"].toInt(
            fallbackProps["fontSize"].toInt(-1)
        );
        const QString fontWeight = style["fontWeight"].toString(
            fallbackProps["fontWeight"].toString()
        );
        const QString textAlign = style["textAlign"].toString();
        const int padding = style["padding"].toInt(-1);

        if (!backgroundColor.isEmpty()) {
            styleSheet += QString("background-color: %1;").arg(backgroundColor);
        }
        if (!color.isEmpty()) {
            styleSheet += QString("color: %1;").arg(color);
        }
        if (borderWidth > 0) {
            styleSheet += QString("border: %1px solid %2;")
                .arg(borderWidth)
                .arg(style["borderColor"].toString("#000000"));
        }
        if (borderRadius > 0) {
            styleSheet += QString("border-radius: %1px;").arg(borderRadius);
        }
        if (fontSize >= 0) {
            styleSheet += QString("font-size: %1px;").arg(fontSize);
        }
        if (!fontWeight.isEmpty()) {
            styleSheet += QString("font-weight: %1;").arg(fontWeight);
        }
        if (!textAlign.isEmpty()) {
            styleSheet += QString("text-align: %1;").arg(textAlign);
        }
        if (includePadding && padding >= 0) {
            styleSheet += QString("padding: %1px;").arg(padding);
        }
        if (!styleSheet.isEmpty()) {
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
        QJsonObject props = root["props"].toObject();
        QJsonObject style = props["style"].toObject();

        // Mengatur judul window dari properti Widget.
        window->setWindowTitle(
            props["title"].toString("JuliaNative")
        );

        // Mengatur ukuran window dari properti Widget.
        window->resize(
            props["width"].toInt(500),
            props["height"].toInt(350)
        );
        applyStyle(window, style);

        // Mengambil layout utama window jika sudah tersedia.
        QVBoxLayout *layout =
            qobject_cast<QVBoxLayout *>(window->layout());

        // Membuat layout utama jika belum tersedia.
       if (!layout) {
            layout = new QVBoxLayout(window);

        }

        const int padding = style["padding"].toInt(0);
        layout->setContentsMargins(padding, padding, padding, padding);
        layout->setSpacing(style["gap"].toInt(0));

        // Menghapus seluruh widget dari tampilan sebelumnya.
        autoFocusInput = nullptr;
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
                // Membaca properti flex untuk elemen root-level juga
                QJsonObject childProps = childData["props"].toObject();
                int flex = qMax(0, childProps["flex"].toInt(0));

                layout->addWidget(widget, flex);
            }
        }

        // Menampilkan window setelah UI selesai dibuat.
        window->show();
        if (autoFocusInput) {
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