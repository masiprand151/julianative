#include "JuliaRuntime.h"

class NativeImageLabel : public QLabel
{
public:
    using QLabel::QLabel;

    void setImage(const QPixmap &value)
    {
        image = value;
        update();
    }

    void setResizeMode(const QString &value)
    {
        resizeMode = value;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QLabel::paintEvent(event);
        if (image.isNull()) return;

        const QRect area = contentsRect();
        if (area.isEmpty()) return;

        QPainter painter(this);
        if (resizeMode == "fill") {
            painter.drawPixmap(area, image);
            return;
        }

        const Qt::AspectRatioMode aspectMode =
            resizeMode == "cover"
                ? Qt::KeepAspectRatioByExpanding
                : Qt::KeepAspectRatio;
        const QSize scaledSize = image.size().scaled(area.size(), aspectMode);
        const QRect target(
            area.center().x() - scaledSize.width() / 2,
            area.center().y() - scaledSize.height() / 2,
            scaledSize.width(),
            scaledSize.height()
        );
        if (resizeMode == "cover") painter.setClipRect(area);
        painter.drawPixmap(target, image);
    }

private:
    QPixmap image;
    QString resizeMode = "contain";
};

QSize JuliaRuntime::requestedImageSize(const QJsonObject &style) const
{
    return QSize(
        styleLengthPixels(style["width"]),
        styleLengthPixels(style["height"])
    );
}

QPixmap JuliaRuntime::decodeImageData(const QByteArray &data, const QString &source, const QSize &requestedSize) const
{
    const QByteArray prefix = data.left(4096).trimmed().toLower();
    const bool isSvg =
        source.endsWith(".svg", Qt::CaseInsensitive) ||
        prefix.contains("<svg");
    if (!isSvg) {
        QPixmap image;
        image.loadFromData(data);
        return image;
    }

    QSvgRenderer renderer(data);
    if (!renderer.isValid()) return {};

    QSize renderSize = renderer.defaultSize();
    if (!renderSize.isValid() || renderSize.isEmpty()) {
        renderSize = requestedSize.isValid() && !requestedSize.isEmpty()
            ? requestedSize
            : QSize(256, 256);
    } else if (requestedSize.width() > 0 && requestedSize.height() > 0) {
        renderSize = requestedSize;
    } else if (requestedSize.width() > 0) {
        renderSize.setHeight(qMax(
            1,
            qRound(
                requestedSize.width() *
                renderSize.height() /
                static_cast<double>(renderSize.width())
            )
        ));
        renderSize.setWidth(requestedSize.width());
    } else if (requestedSize.height() > 0) {
        renderSize.setWidth(qMax(
            1,
            qRound(
                requestedSize.height() *
                renderSize.width() /
                static_cast<double>(renderSize.height())
            )
        ));
        renderSize.setHeight(requestedSize.height());
    }

    renderSize = renderSize.scaled(4096, 4096, Qt::KeepAspectRatio);
    QImage image(renderSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    renderer.render(
        &painter,
        QRectF(QPointF(0, 0), QSizeF(renderSize))
    );
    painter.end();
    return QPixmap::fromImage(image);
}

void JuliaRuntime::loadImageSource(const QString &source, const QSize &requestedSize, std::function<void(const QPixmap &)> onLoaded)
{
    if (source.trimmed().isEmpty()) {
        qWarning() << "Image requires a non-empty source.";
        return;
    }

    const QUrl url(source);
    const QString scheme = url.scheme().toLower();
    if (scheme == "http" || scheme == "https") {
        QNetworkRequest request(url);
        request.setTransferTimeout(15000);
        request.setAttribute(
            QNetworkRequest::RedirectPolicyAttribute,
            QNetworkRequest::NoLessSafeRedirectPolicy
        );
        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this,
            [this, reply, source, requestedSize, onLoaded]() {
                if (reply->error() != QNetworkReply::NoError) {
                    qWarning() << "Could not download image from"
                               << reply->url().host()
                               << reply->errorString();
                    reply->deleteLater();
                    return;
                }

                const QPixmap image = decodeImageData(
                    reply->readAll(),
                    source,
                    requestedSize
                );
                reply->deleteLater();
                if (image.isNull()) {
                    qWarning() << "Could not decode downloaded image.";
                    return;
                }
                onLoaded(image);
            }
        );
        return;
    }

    QString path;
    if (url.isLocalFile()) {
        path = url.toLocalFile();
    } else if (QFileInfo(source).isAbsolute() ||
               source.startsWith(":/"))
    {
        path = source;
    } else if (scheme.isEmpty()) {
        path = QDir(workingDir).absoluteFilePath(source);
    } else {
        qWarning() << "Unsupported image source scheme:" << scheme;
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open image file:"
                   << path << file.errorString();
        return;
    }

    const QPixmap image = decodeImageData(
        file.readAll(),
        path,
        requestedSize
    );
    if (image.isNull()) {
        qWarning() << "Could not decode image file:" << path;
        return;
    }
    onLoaded(image);
}

QWidget *JuliaRuntime::createImage(const QJsonObject &props)
{
    const QJsonObject style = props["style"].toObject();
    auto *label = new NativeImageLabel();
    label->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    label->setAccessibleName(props["alt"].toString());
    label->setResizeMode(props["resizeMode"].toString("contain"));
    applyStyle(label, style);

    QPointer<NativeImageLabel> safeLabel(label);
    loadImageSource(
        props["source"].toString(),
        requestedImageSize(style),
        [safeLabel](const QPixmap &image) {
            if (safeLabel) safeLabel->setImage(image);
        }
    );
    return label;
}

QWidget *JuliaRuntime::createVideo(const QJsonObject &data)
{
    const QJsonObject props = data["props"].toObject();
    const QJsonObject style = props["style"].toObject();
    const QString source = props["source"].toString();
    if (source.isEmpty()) {
        qWarning() << "Video requires a non-empty source.";
        return nullptr;
    }

    auto *container = new QFrame();
    container->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    container->setMinimumHeight(180);
    applyStyle(container, style);

    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto *videoWidget = new QVideoWidget(container);
    videoWidget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );
    layout->addWidget(videoWidget, 1);

    auto *controls = new QWidget(container);
    auto *controlsLayout = new QHBoxLayout(controls);
    controlsLayout->setContentsMargins(0, 0, 0, 0);

    auto *playButton = new QPushButton("Play", controls);
    auto *positionSlider = new QSlider(Qt::Horizontal, controls);
    positionSlider->setRange(0, 0);
    auto *timeLabel = new QLabel("00:00 / 00:00", controls);
    auto *errorLabel = new QLabel(controls);
    errorLabel->setWordWrap(true);
    errorLabel->hide();

    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(positionSlider, 1);
    controlsLayout->addWidget(timeLabel);
    controls->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );
    controls->setFixedHeight(32);
    layout->addWidget(controls);
    layout->addWidget(errorLabel);

    auto *player = new QMediaPlayer(container);
    auto *audioOutput = new QAudioOutput(player);
    audioOutput->setMuted(props["muted"].toBool(false));
    player->setAudioOutput(audioOutput);
    player->setVideoOutput(videoWidget);

    const auto formatTime = [](qint64 milliseconds) {
        const qint64 seconds = qMax<qint64>(0, milliseconds / 1000);
        return QString("%1:%2")
            .arg(seconds / 60, 2, 10, QChar('0'))
            .arg(seconds % 60, 2, 10, QChar('0'));
    };
    const auto updateTimeLabel =
        [player, timeLabel, formatTime]() {
            timeLabel->setText(
                formatTime(player->position()) + " / " +
                formatTime(player->duration())
            );
        };

    connect(playButton, &QPushButton::clicked, player, [player]() {
        if (player->playbackState() == QMediaPlayer::PlayingState) {
            player->pause();
        } else {
            player->play();
        }
    });
    connect(player, &QMediaPlayer::playbackStateChanged,
        playButton, [playButton](QMediaPlayer::PlaybackState state) {
            playButton->setText(
                state == QMediaPlayer::PlayingState ? "Pause" : "Play"
            );
        }
    );
    connect(player, &QMediaPlayer::durationChanged,
        positionSlider, [positionSlider, updateTimeLabel](qint64 duration) {
            positionSlider->setRange(0, static_cast<int>(
                qMin<qint64>(duration, std::numeric_limits<int>::max())
            ));
            updateTimeLabel();
        }
    );
    connect(player, &QMediaPlayer::positionChanged,
        positionSlider, [positionSlider, updateTimeLabel](qint64 position) {
            const QSignalBlocker blocker(positionSlider);
            positionSlider->setValue(static_cast<int>(position));
            updateTimeLabel();
        }
    );
    connect(positionSlider, &QSlider::sliderMoved,
        player, [player](int position) {
            player->setPosition(position);
        }
    );
    connect(player, &QMediaPlayer::errorOccurred, errorLabel,
        [errorLabel, source](
            QMediaPlayer::Error,
            const QString &errorString
        ) {
            const QString message = errorString.isEmpty()
                ? QString("Could not play video: %1").arg(source)
                : errorString;
            errorLabel->setText(message);
            errorLabel->show();
            qWarning() << "Video playback error:" << message;
        }
    );
    if (props["loop"].toBool(false)) {
        connect(player, &QMediaPlayer::mediaStatusChanged, player,
            [player](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia) {
                    player->setPosition(0);
                    player->play();
                }
            }
        );
    }

    player->setSource(QUrl::fromUserInput(source));
    if (props["autoplay"].toBool(false)) {
        player->play();
    }
    return container;
}
