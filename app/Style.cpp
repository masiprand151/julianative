#include "JuliaRuntime.h"

bool JuliaRuntime::cursorShapeFor(const QString &name, Qt::CursorShape *shape) const
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

QString JuliaRuntime::cssPropertyName(const QString &name) const
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

QString JuliaRuntime::cssValue(const QString &property, const QJsonValue &value) const
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

QString JuliaRuntime::cssDeclarations(const QJsonObject &style, bool includePadding, bool includeGeometry) const
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

QString JuliaRuntime::styleSheetFor(QWidget *widget, const QJsonObject &style, bool includePadding)
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

int JuliaRuntime::styleLengthPixels(const QJsonValue &value, int fallback) const
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

QJsonValue JuliaRuntime::styleValue(const QJsonObject &style, const QString &camelName, const QString &cssName) const
{
    return style.contains(camelName) ? style[camelName] : style[cssName];
}

void JuliaRuntime::setStyledDimension(QWidget *widget, const QJsonObject &style, const QString &property, bool minimum, bool maximum)
{
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

void JuliaRuntime::applyStyle(QWidget *widget, const QJsonObject &style, const QJsonObject &fallbackProps, bool includePadding)
{
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

Qt::Alignment JuliaRuntime::getAlignment(const QString &align, const QString &direction)
{
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

void JuliaRuntime::addJustifyStart(QBoxLayout *layout)
{
    layout->addStretch(1);
}

void JuliaRuntime::addJustifyCenter(QBoxLayout *layout)
{
    layout->insertStretch(0, 1);
    layout->addStretch(1);
}

void JuliaRuntime::addJustifyEnd(QBoxLayout *layout)
{
    layout->insertStretch(0, 1);
}

