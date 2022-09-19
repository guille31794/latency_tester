#ifndef EXTENSIONFILEDELEGATE_H
#define EXTENSIONFILEDELEGATE_H

#include <QFileInfo>
#include <QStyledItemDelegate>

class NoIconOrExtensionFileDelegate: public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(NoIconOrExtensionFileDelegate)
public:
    explicit NoIconOrExtensionFileDelegate(QObject* parent=Q_NULLPTR) : QStyledItemDelegate(parent){}

protected:

    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        QVariant value = index.data(Qt::FontRole);

        if (value.isValid() && !value.isNull())
        {
            option->font = qvariant_cast<QFont>(value).resolve(option->font);
            option->fontMetrics = QFontMetrics(option->font);
        }

        value = index.data(Qt::TextAlignmentRole);
        if (value.isValid() && !value.isNull())
        {
            option->displayAlignment = Qt::Alignment(value.toInt());
        }

        value = index.data(Qt::ForegroundRole);
        if (value.canConvert<QBrush>())
        {
            option->palette.setBrush(QPalette::Text, qvariant_cast<QBrush>(value));
        }

        option->index = index;
        value = index.data(Qt::CheckStateRole);

        if (value.isValid() && !value.isNull())
        {
            option->features |= QStyleOptionViewItem::HasCheckIndicator;
            option->checkState = static_cast<Qt::CheckState>(value.toInt());
        }

        value = index.data(Qt::DisplayRole);
        if (value.isValid() && !value.isNull())
        {
            option->features |= QStyleOptionViewItem::HasDisplay;
            option->text = displayText(value, option->locale);
        }
        option->backgroundBrush = qvariant_cast<QBrush>(index.data(Qt::BackgroundRole));
        option->styleObject = 0;

        // Extension removal
        QFileInfo fileinfo(option->text);
        option->text = fileinfo.completeBaseName();
    }
};

#endif // EXTENSIONFILEDELEGATE_H
