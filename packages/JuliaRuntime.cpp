#include "JuliaRuntime.h"
#include <QGuiApplication>
#include <QScreen>
#include <QSysInfo>
#include <QThread>
#include <QSettings>
#include <QLocale>
#include <QTimeZone>
#include <QClipboard>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif


// Constructor: menyiapkan window dan koneksi komunikasi Node.js.
JuliaRuntime::JuliaRuntime()
{
    node = new QProcess(this);
    networkManager = new QNetworkAccessManager(this);
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
                        // Jika perintah terkait clipboard, lakukan operasi yang sesuai.
                        else if (command.startsWith("clipboard.")) {

                            QJsonValue result;
                            QString error;
                            bool success = true;

                            QClipboard *clipboard = QGuiApplication::clipboard();

                            if (command == "clipboard.getText") {

                                result = clipboard->text();

                            } else if (command == "clipboard.setText") {

                                clipboard->setText(message["text"].toString());
                                result = true;

                            } else if (command == "clipboard.clear") {

                                clipboard->clear();
                                result = true;

                            } else {

                                success = false;
                                error = "Unknown clipboard command: " + command;
                            }

                            QJsonObject response;
                            response["type"] = "nativeResponse";
                            response["requestId"] = message["requestId"];
                            response["success"] = success;

                            if (success) {
                                response["data"] = result;
                            } else {
                                response["error"] = error;
                            }

                            node->write(
                                QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n"
                            );
                        }
                        // get a information screens from the host machine
                        else if (command == "screens") {

                            QJsonArray screensArray;

                            const auto screens = QGuiApplication::screens();

                            QScreen *primaryScreen =
                                QGuiApplication::primaryScreen();

                            for (int i = 0; i < screens.size(); i++) {

                                QScreen *screen = screens[i];

                                QRect geometry = screen->geometry();
                                QRect available = screen->availableGeometry();

                                QJsonObject screenObj;

                                screenObj["id"] = i;
                                screenObj["name"] = screen->name();

                                screenObj["width"] = geometry.width();
                                screenObj["height"] = geometry.height();

                                screenObj["x"] = geometry.x();
                                screenObj["y"] = geometry.y();

                                screenObj["availableWidth"] = available.width();
                                screenObj["availableHeight"] = available.height();

                                screenObj["primary"] =
                                    (screen == primaryScreen);

                                screenObj["scaleFactor"] =
                                    screen->devicePixelRatio();

                                screensArray.append(screenObj);
                            }

                            QJsonObject response;

                            response["type"] = "screensResponse";

                            response["requestId"] =
                                message["requestId"];

                            response["success"] = true;

                            response["screens"] = screensArray;

                            node->write(
                                QJsonDocument(response)
                                    .toJson(QJsonDocument::Compact) + "\n"
                            );
                        } else
                        // get a system information from the host machine
                            if (command.startsWith("system.")) {

                                QJsonValue result;
                                QString error;
                                bool success = true;

                                if (command == "system.getPlatform") {

                            #ifdef Q_OS_WIN
                                    result = "windows";
                            #elif defined(Q_OS_MACOS)
                                    result = "macos";
                            #elif defined(Q_OS_LINUX)
                                    result = "linux";
                            #else
                                    result = "unknown";
                            #endif

                                } else if (command == "system.getOSVersion") {

                                    result = QSysInfo::prettyProductName();

                                } else if (command == "system.getHostname") {

                                    result = QSysInfo::machineHostName();

                                } else if (command == "system.getLocale") {

                                    result = QLocale().name();

                                } else if (command == "system.getTimeZone") {

                                    result = QString::fromUtf8(
                                        QTimeZone::systemTimeZoneId()
                                    );

                                } else if (command == "system.getCPUInfo") {

                                    QString model = QSysInfo::currentCpuArchitecture();

                            #ifdef Q_OS_WIN
                                    QSettings cpuRegistry(
                                        "HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                                        QSettings::NativeFormat
                                    );

                                    const QString registryModel =
                                        cpuRegistry.value("ProcessorNameString").toString().trimmed();

                                    if (!registryModel.isEmpty()) {
                                        model = registryModel;
                                    }
                            #endif

                                    QJsonObject cpu;
                                    cpu["model"] = model;
                                    cpu["cores"] = QThread::idealThreadCount();
                                    cpu["architecture"] = QSysInfo::currentCpuArchitecture();

                                    result = cpu;

                                } else if (command == "system.getMemoryInfo") {

                            #ifdef Q_OS_WIN
                                    MEMORYSTATUSEX memoryStatus = {};
                                    memoryStatus.dwLength = sizeof(memoryStatus);

                                    if (GlobalMemoryStatusEx(&memoryStatus)) {

                                        const quint64 total = memoryStatus.ullTotalPhys;
                                        const quint64 free = memoryStatus.ullAvailPhys;

                                        QJsonObject memory;
                                        memory["total"] = static_cast<double>(total);
                                        memory["free"] = static_cast<double>(free);
                                        memory["used"] = static_cast<double>(total - free);

                                        result = memory;

                                    } else {
                                        success = false;
                                        error = "Failed to get memory information.";
                                    }
                            #else
                                    success = false;
                                    error = "Memory information is not implemented for this platform.";
                            #endif

                                } else if (command == "system.getSystemInfo") {

                                    QJsonObject system;

                            #ifdef Q_OS_WIN
                                    system["platform"] = "windows";
                            #elif defined(Q_OS_MACOS)
                                    system["platform"] = "macos";
                            #elif defined(Q_OS_LINUX)
                                    system["platform"] = "linux";
                            #else
                                    system["platform"] = "unknown";
                            #endif

                                    system["osVersion"] = QSysInfo::prettyProductName();
                                    system["hostname"] = QSysInfo::machineHostName();
                                    system["architecture"] = QSysInfo::currentCpuArchitecture();
                                    system["locale"] = QLocale().name();
                                    system["timeZone"] = QString::fromUtf8(
                                        QTimeZone::systemTimeZoneId()
                                    );

                                    result = system;

                                } else {

                                    success = false;
                                    error = "Unknown system command: " + command;
                                }

                                QJsonObject response;
                                response["type"] = "nativeResponse";
                                response["requestId"] = message["requestId"];
                                response["success"] = success;

                                if (success) {
                                    response["data"] = result;
                                } else {
                                    response["error"] = error;
                                }

                                node->write(
                                    QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n"
                                );
                            }


                    } else if (type == "patch") {
                        applyLeafPatches(message);
                    } else if (type == "listPatch") {
                        applyVirtualListPatches(message);
                    } else if (type == "storageRequest") {
                        handleStorageRequest(message);
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
void JuliaRuntime::handleStorageRequest(const QJsonObject &message)
{
    const QString requestId = message["requestId"].toString();
    if (requestId.isEmpty()) {
        qWarning() << "Ignoring local storage request without a request ID.";
        return;
    }

    QJsonObject response;
    response["type"] = "storageResponse";
    response["requestId"] = requestId;

    const QString directory =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (directory.isEmpty()) {
        response["success"] = false;
        response["error"] = "Could not determine the application data directory.";
    } else {
        const QString filePath =
            QDir(directory).filePath("local-storage.json");
        QJsonObject storage;
        QFile source(filePath);
        bool requestFailed = false;

        if (source.exists()) {
            if (!source.open(QIODevice::ReadOnly)) {
                response["success"] = false;
                response["error"] = source.errorString();
                requestFailed = true;
            } else {
                QJsonParseError parseError;
                const QByteArray contents = source.readAll();
                source.close();
                const QJsonDocument document =
                    QJsonDocument::fromJson(contents, &parseError);
                if (parseError.error != QJsonParseError::NoError ||
                    !document.isObject())
                {
                    response["success"] = false;
                    response["error"] =
                        "The local storage file is not a valid JSON object.";
                    requestFailed = true;
                } else {
                    storage = document.object();
                }
            }
        }

        const QString operation = message["operation"].toString();
        const QString key = message["key"].toString();
        bool shouldSave = false;
        if (!requestFailed && operation == "get") {
            if (!message.contains("key")) {
                response["success"] = false;
                response["error"] = "Local storage get requires a key.";
            } else {
                const bool found = storage.contains(key);
                response["success"] = true;
                response["found"] = found;
                if (found) response["value"] = storage.value(key);
            }
        } else if (!requestFailed && operation == "set") {
            if (!message.contains("key") || !message.contains("value")) {
                response["success"] = false;
                response["error"] =
                    "Local storage set requires a key and JSON value.";
            } else {
                storage.insert(key, message["value"]);
                shouldSave = true;
            }
        } else if (!requestFailed && operation == "remove") {
            if (!message.contains("key")) {
                response["success"] = false;
                response["error"] = "Local storage remove requires a key.";
            } else {
                storage.remove(key);
                shouldSave = true;
            }
        } else if (!requestFailed && operation == "clear") {
            storage = QJsonObject();
            shouldSave = true;
        } else if (!requestFailed) {
            response["success"] = false;
            response["error"] = "Unsupported local storage operation.";
        }

        if (!requestFailed && shouldSave) {
            if (!QDir().mkpath(directory)) {
                response["success"] = false;
                response["error"] =
                    "Could not create the application data directory.";
            } else {
                QSaveFile destination(filePath);
                if (!destination.open(QIODevice::WriteOnly)) {
                    response["success"] = false;
                    response["error"] =
                        QString("Could not open local storage file '%1': %2")
                            .arg(filePath, destination.errorString());
                } else {
                    const QByteArray contents =
                        QJsonDocument(storage).toJson(QJsonDocument::Indented);
                    if (destination.write(contents) != contents.size()) {
                        response["success"] = false;
                        response["error"] = destination.errorString();
                        destination.cancelWriting();
                    } else if (!destination.commit()) {
                        response["success"] = false;
                        response["error"] =
                            QString("Could not replace local storage file '%1': %2")
                                .arg(filePath, destination.errorString());
                    } else {
                        response["success"] = true;
                    }
                }
            }
        }
    }

    node->write(
        QJsonDocument(response).toJson(QJsonDocument::Compact) + "\n"
    );
}

void JuliaRuntime::start(bool developmentMode)
{
    // Mengambil lokasi folder tempat executable JuliaNative berada.
    QString appDir =
        QCoreApplication::applicationDirPath();

    // Lokasi Node.js untuk versi portable.
    nodePath =
        QDir(appDir).filePath("runtime/node/node.exe");

    // Lokasi file utama JavaScript untuk versi portable.
    mainFile =
        QDir(appDir).filePath("runtime/packages/src/main.js");

    // Folder kerja Node.js untuk versi portable.
    workingDir =
        QDir(appDir).filePath("runtime/packages");

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
            QDir(projectDir).filePath("packages/node/node.exe");

        // Lokasi hasil compile TypeScript.
        mainFile =
            QDir(projectDir).filePath("packages/build/src/main.js");

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
            QDir(projectDir).filePath("packages/build/.dev-reload");
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

void JuliaRuntime::restartNode()
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

bool JuliaRuntime::eventFilter(QObject *watched, QEvent *event)
{
    if (auto *viewport = qobject_cast<QWidget *>(watched)) {
        if (event->type() == QEvent::Resize &&
            !viewport->property("juliaVirtualListNodeId").toString().isEmpty())
        {
            sendVirtualListViewport(viewport);
        }
    }

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

void JuliaRuntime::sendWindowEvent(const QString &id, const QString &event, int width, int height)
{
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

void JuliaRuntime::updatePercentSizedChildren(QWidget *parent)
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

#ifdef Q_OS_WIN
bool JuliaRuntime::attachNodeToJob()
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

// Destructor: menghentikan Node.js dan membersihkan window.
JuliaRuntime::~JuliaRuntime()
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
