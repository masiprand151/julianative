#pragma once

#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QWindow>
#include <QScrollArea>
#include <QScrollBar>
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
#include <QSaveFile>
#include <QFileSystemWatcher>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSvgRenderer>
#include <QTimer>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QSizePolicy>
#include <QMargins>
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
#include <QAudioOutput>
#include <QBrush>
#include <QColor>
#include <QIcon>
#include <QMediaPlayer>
#include <QSlider>
#include <QSignalBlocker>
#include <QUrl>
#include <QVideoWidget>
#include <functional>
#include <limits>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif


class JuliaRuntime : public QObject
{

public:
    // Proses Node.js yang menjalankan kode JavaScript.
    QProcess *node;
    QNetworkAccessManager *networkManager = nullptr;

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
    QString lastAppliedPositionMode;
    QString requestedPositionMode;
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

    JuliaRuntime();
    void start(bool developmentMode = false);
    void restartNode();
    bool eventFilter(QObject *watched, QEvent *event) override;
    void sendWindowEvent(
        const QString &id,
        const QString &event,
        int width = 0,
        int height = 0
    );
    void updatePercentSizedChildren(QWidget *parent);
    bool cursorShapeFor(const QString &name, Qt::CursorShape *shape) const;
    QString cssPropertyName(const QString &name) const;
    QString cssValue(const QString &property, const QJsonValue &value) const;
    QString cssDeclarations(
        const QJsonObject &style,
        bool includePadding,
        bool includeGeometry
    ) const;
    QString styleSheetFor(QWidget *widget, const QJsonObject &style, bool includePadding);

#ifdef Q_OS_WIN
    bool attachNodeToJob();
#endif
    QSize requestedImageSize(const QJsonObject &style) const;
    QPixmap decodeImageData(
        const QByteArray &data,
        const QString &source,
        const QSize &requestedSize
    ) const;
    void loadImageSource(
        const QString &source,
        const QSize &requestedSize,
        std::function<void(const QPixmap &)> onLoaded
    );
    QWidget *createImage(const QJsonObject &props);
    QWidget *createNode(const QJsonObject &data);
    QWidget *createNodeImpl(const QJsonObject &data);
    QWidget *createVirtualList(const QJsonObject &data);
    void updateVirtualList(
        QScrollArea *list,
        const QJsonObject &data,
        bool reuseItems
    );
    void sendVirtualListViewport(QWidget *viewport);
    QWidget *createVideo(const QJsonObject &data);
    void applyTableItemStyle(
        QTableWidgetItem *item,
        const QJsonObject &style
    );
    QWidget *createTable(const QJsonObject &data);
    void clearModalDialogs();
    void clearFloatingActionButtons();
    void positionFloatingActionButtons();
    void applyLeafPatches(const QJsonObject &message);
    void applyVirtualListPatches(const QJsonObject &message);
    void handleStorageRequest(const QJsonObject &message);
    void createModal(const QJsonObject &data);
    QWidget *createView(const QJsonObject &data);
    QWidget *createRow(const QJsonObject &data);
    QWidget *createColumn(const QJsonObject &data);
    QWidget *createFlexContainer(
        const QJsonObject &data,
        const QString &direction
    );
    int styleLengthPixels(const QJsonValue &value, int fallback = -1) const;
    QMargins styleBoxSpacing(const QJsonValue &value, int fallback = 0) const;
    QJsonValue styleValue(
        const QJsonObject &style,
        const QString &camelName,
        const QString &cssName
    ) const;
    void setStyledDimension(
        QWidget *widget,
        const QJsonObject &style,
        const QString &property,
        bool minimum,
        bool maximum
    );
    void applyStyle(
        QWidget *widget,
        const QJsonObject &style,
        const QJsonObject &fallbackProps = QJsonObject(),
        bool includePadding = false
    );
    Qt::Alignment getAlignment( const QString &align, const QString &direction );
    void addJustifyStart(QBoxLayout *layout);
    void addJustifyCenter(QBoxLayout *layout);
    void addJustifyEnd(QBoxLayout *layout);
    void renderUI(const QByteArray &json);
    ~JuliaRuntime();
};
