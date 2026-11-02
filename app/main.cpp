
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
#include <QCoreApplication>

// Mengelola komunikasi antara runtime C++ Qt dan aplikasi JavaScript Node.js.
class JuliaRuntime : public QObject
{
public:
    // Proses Node.js yang menjalankan kode JavaScript.
    QProcess *node;

    // Window utama aplikasi JuliaNative.
    QWidget *window;

    // Menampung data output Node.js sebelum diproses per baris.
    QByteArray buffer;

    // Constructor: menyiapkan window dan koneksi komunikasi Node.js.
    JuliaRuntime()
    {
        node = new QProcess(this);
        window = new QWidget();

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

                    // Jika data memiliki penanda UI, teruskan ke renderer.
                    if (line.startsWith(prefix)) {
                        renderUI(line.mid(prefix.size()));
                    }
                }
            }
        );
    }

    // Menjalankan Node.js dan menentukan lokasi file JavaScript.
    void start()
    {
        // Mengambil lokasi folder tempat executable JuliaNative berada.
        QString appDir =
            QCoreApplication::applicationDirPath();

        // Lokasi Node.js untuk versi portable.
        QString nodePath =
            QDir(appDir).filePath("runtime/node/node.exe");

        // Lokasi file utama JavaScript untuk versi portable.
        QString mainFile =
            QDir(appDir).filePath("runtime/app/src/main.js");

        // Folder kerja Node.js untuk versi portable.
        QString workingDir =
            QDir(appDir).filePath("runtime/app");

        // Jika file portable tidak ditemukan, gunakan struktur project development.
        if (!QFileInfo::exists(nodePath) ||
            !QFileInfo::exists(mainFile))
        {
            // Mengambil lokasi root project dari folder executable.
            QString projectDir =
                QDir(appDir).absoluteFilePath("../..");

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

        // Menampilkan window utama.
        window->show();

        // Memulai proses Node.js.
        node->start();

        // Menunggu sampai proses Node.js berhasil dijalankan.
        if (!node->waitForStarted(5000)) {
            window->close();
        }
    }

    // Mengubah satu node UI JSON menjadi widget native Qt.
    // Fungsi ini berjalan secara rekursif untuk menangani children.
    QWidget *createNode(const QJsonObject &data)
    {
        // Mengambil tipe komponen dari JSON.
        QString type = data["type"].toString();

        // Mengambil properti komponen.
        QJsonObject props = data["props"].toObject();

        // Mengambil daftar event komponen.
        QJsonObject events = data["events"].toObject();

        // Mengambil children atau komponen di dalamnya.
        QJsonArray children = data["children"].toArray();

        // Membuat komponen Text menggunakan QLabel.
        if (type == "Text") {
            QLabel *label = new QLabel(
                props["text"].toString()
            );

            // Mengatur ukuran dan ketebalan font.
            label->setStyleSheet(
                QString("font-size:%1px; font-weight:%2;")
                    .arg(props["fontSize"].toInt(14))
                    .arg(
                        props["fontWeight"].toString() == "bold"
                            ? "bold"
                            : "normal"
                    )
            );

            return label;
        }

        // Membuat komponen Button menggunakan QPushButton.
        if (type == "Button") {
            QPushButton *button = new QPushButton(
                props["text"].toString()
            );

            // Mengambil ID event click dari JSON.
            QString eventId = events["click"].toString();

            // Menghubungkan klik tombol native dengan event JavaScript.
            connect(button, &QPushButton::clicked,
                this, [this, eventId]() {

                    if (eventId.isEmpty()) return;

                    // Membuat pesan event untuk dikirim ke Node.js.
                    QJsonObject message;
                    message["type"] = "click";
                    message["id"] = eventId;

                    // Mengubah pesan JSON menjadi format teks.
                    QByteArray data =
                        QJsonDocument(message)
                            .toJson(QJsonDocument::Compact);

                    // Mengirim event melalui stdin proses Node.js.
                    node->write(data + "\n");
                }
            );

            return button;
        }

        // Membuat container untuk komponen layout.
        QWidget *container = new QWidget();

        // Row menggunakan layout horizontal.
        // Selain Row, layout menggunakan susunan vertikal.
        QBoxLayout *layout =
            type == "Row"
                ? static_cast<QBoxLayout *>(
                    new QHBoxLayout(container))
                : static_cast<QBoxLayout *>(
                    new QVBoxLayout(container));

        // Mengatur jarak antar-komponen.
        layout->setSpacing(props["spacing"].toInt(10));

        // Mengambil nilai padding dari properti.
        int padding = props["padding"].toInt(0);

        // Mengatur jarak bagian dalam layout.
        layout->setContentsMargins(
            padding, padding, padding, padding
        );

        // Membuat seluruh children dan memasukkannya ke layout.
        for (const QJsonValue &child : children) {
            QWidget *widget = createNode(child.toObject());

            if (widget) {
                layout->addWidget(widget);
            }
        }

        return container;
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

        // Mengatur judul window dari properti Widget.
        window->setWindowTitle(
            props["title"].toString("JuliaNative")
        );

        // Mengatur ukuran window dari properti Widget.
        window->resize(
            props["width"].toInt(500),
            props["height"].toInt(350)
        );

        // Mengambil layout utama window jika sudah tersedia.
        QVBoxLayout *layout =
            qobject_cast<QVBoxLayout *>(window->layout());

        // Membuat layout utama jika belum tersedia.
        if (!layout) {
            layout = new QVBoxLayout(window);
            layout->setContentsMargins(0, 0, 0, 0);
        }

        // Menghapus seluruh widget dari tampilan sebelumnya.
        while (QLayoutItem *item = layout->takeAt(0)) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }

            delete item;
        }

        // Mengambil children dari root JSON.
        QJsonArray children = root["children"].toArray();

        // Membuat dan menambahkan children ke window.
        for (const QJsonValue &child : children) {
            QWidget *widget = createNode(child.toObject());

            if (widget) {
                layout->addWidget(widget);
            }
        }

        // Menampilkan window setelah UI selesai dibuat.
        window->show();
    }

    // Destructor: menghentikan Node.js dan membersihkan window.
    ~JuliaRuntime()
    {
        // Memastikan proses Node.js berhenti sebelum runtime dihancurkan.
        if (node->state() != QProcess::NotRunning) {
            node->kill();
            node->waitForFinished();
        }

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
    runtime.start();

    // Menjalankan event loop Qt.
    return app.exec();
}
