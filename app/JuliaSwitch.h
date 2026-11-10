#include <QCheckBox>
#include <QPainter>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QColor>
#include <QSize>

class JuliaSwitch : public QCheckBox
{
public:
    explicit JuliaSwitch(const QString &label, QWidget *parent = nullptr)
        : QCheckBox(label, parent)
    {
        setTristate(false);
        setCursor(Qt::PointingHandCursor);
        setMinimumHeight(32);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }

    QSize sizeHint() const override
    {
        int textWidth = fontMetrics().horizontalAdvance(text());

        return QSize(textWidth + 54, 32);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing);

        const int trackWidth = 42;
        const int trackHeight = 24;

        const int trackX = width() - trackWidth - 2;
        const int trackY = (height() - trackHeight) / 2;

        QColor trackColor = isChecked()
            ? QColor("#2196F3")
            : QColor("#BDBDBD");

        QColor thumbColor("#FFFFFF");

        if (!isEnabled()) {
            trackColor.setAlpha(100);
            thumbColor.setAlpha(180);
        }

        // Track
        painter.setPen(Qt::NoPen);
        painter.setBrush(trackColor);

        painter.drawRoundedRect(
            trackX,
            trackY,
            trackWidth,
            trackHeight,
            trackHeight / 2,
            trackHeight / 2
        );

        // Thumb
        const int thumbSize = 18;

        const int thumbX = isChecked()
            ? trackX + trackWidth - thumbSize - 3
            : trackX + 3;

        const int thumbY = trackY + 3;

        painter.setBrush(thumbColor);

        painter.drawEllipse(
            thumbX,
            thumbY,
            thumbSize,
            thumbSize
        );

        // Label
        if (!text().isEmpty()) {
            QColor textColor = palette().color(QPalette::WindowText);

            if (!isEnabled()) {
                textColor.setAlpha(120);
            }

            painter.setPen(textColor);

            painter.drawText(
                QRect(0, 0, trackX - 8, height()),
                Qt::AlignLeft | Qt::AlignVCenter,
                text()
            );
        }
    }
};
