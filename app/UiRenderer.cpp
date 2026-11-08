#include "JuliaRuntime.h"

QWidget *JuliaRuntime::createNode(const QJsonObject &data)
{
    QWidget *widget = createNodeImpl(data);
    if (widget) {
        widget->setProperty(
            "juliaNodeId",
            data["nodeId"].toString()
        );
    }
    return widget;
}

QWidget *JuliaRuntime::createNodeImpl(const QJsonObject &data)
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

    if (type == "Image") {
        return createImage(props);
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
            const QString imageSource =
                props["imageSource"].toString();
            if (!imageSource.isEmpty()) {
                const QJsonObject style = props["style"].toObject();
                const int width = styleLengthPixels(style["width"], 56);
                const int height = styleLengthPixels(style["height"], 56);
                QPointer<QPushButton> safeButton(button);
                loadImageSource(
                    imageSource,
                    QSize(qMax(1, width - 20), qMax(1, height - 20)),
                    [safeButton, width, height](const QPixmap &image) {
                        if (!safeButton) return;
                        safeButton->setIcon(QIcon(image));
                        safeButton->setText(QString());
                        safeButton->setIconSize(QSize(
                            qMax(1, width - 20),
                            qMax(1, height - 20)
                        ));
                    }
                );
            }
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

    if (type == "Video") {
        return createVideo(data);
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

void JuliaRuntime::applyTableItemStyle(QTableWidgetItem *item, const QJsonObject &style)
{
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

QWidget *JuliaRuntime::createTable(const QJsonObject &data)
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

void JuliaRuntime::clearModalDialogs()
{
    for (QDialog *dialog : modalDialogs) {
        if (!dialog) continue;
        QObject::disconnect(dialog, nullptr, this, nullptr);
        dialog->close();
        delete dialog;
    }
    modalDialogs.clear();
}

void JuliaRuntime::clearFloatingActionButtons()
{
    for (QWidget *button : floatingActionButtons) {
        delete button;
    }
    floatingActionButtons.clear();
}

void JuliaRuntime::positionFloatingActionButtons()
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

void JuliaRuntime::applyLeafPatches(const QJsonObject &message)
{
    const QJsonArray updates = message["updates"].toArray();
    const QList<QWidget *> widgets = window->findChildren<QWidget *>();

    for (const QJsonValue &value : updates) {
        const QJsonObject update = value.toObject();
        const QString nodeId = update["nodeId"].toString();
        const QString type = update["type"].toString();
        QWidget *target = nullptr;

        for (QWidget *widget : widgets) {
            if (widget->property("juliaNodeId").toString() == nodeId) {
                target = widget;
                break;
            }
        }
        if (!target) {
            qWarning() << "Could not find widget for UI patch:" << nodeId;
            continue;
        }

        const QString text = update["value"].toString();
        if (type == "Text") {
            auto *label = qobject_cast<QLabel *>(target);
            if (!label) {
                qWarning() << "UI text patch target is not a label:"
                           << nodeId;
                continue;
            }
            if (label->text() != text) label->setText(text);
        } else if (type == "TextInput") {
            auto *input = qobject_cast<QLineEdit *>(target);
            if (!input) {
                qWarning() << "UI input patch target is not a line edit:"
                           << nodeId;
                continue;
            }
            if (input->text() == text) continue;

            const int cursorPosition = input->cursorPosition();
            const QSignalBlocker blocker(input);
            input->setText(text);
            input->setCursorPosition(
                qMin(cursorPosition, input->text().size())
            );
        } else if (type == "Button") {
            auto *button = qobject_cast<QPushButton *>(target);
            if (!button) {
                qWarning() << "UI button patch target is not a button:"
                           << nodeId;
                continue;
            }
            if (button->text() != text) button->setText(text);
        }
    }
}

void JuliaRuntime::createModal(const QJsonObject &data)
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

QWidget *JuliaRuntime::createView(const QJsonObject &data)
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

QWidget *JuliaRuntime::createRow(const QJsonObject &data)
{
    return createFlexContainer(data, "row");
}

QWidget *JuliaRuntime::createColumn(const QJsonObject &data)
{
    return createFlexContainer(data, "column");
}

QWidget *JuliaRuntime::createFlexContainer(const QJsonObject &data, const QString &direction)
{
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

// Menerima JSON dari Node.js dan merender UI ke window Qt.
void JuliaRuntime::renderUI(const QByteArray &json)
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

