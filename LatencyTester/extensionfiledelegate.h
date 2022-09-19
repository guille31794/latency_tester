#ifndef EXTENSIONFILEDELEGATE_H
#define EXTENSIONFILEDELEGATE_H

#include <QFileInfo>
#include <QStyledItemDelegate>

class ExtensionFileDelegate: public QStyledItemDelegate
{
    Q_OBJECT
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        QFileInfo fileinfo(opt.text);
        opt.text = fileinfo.completeBaseName();
        QStyle* style = opt.widget->style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QSize result = QStyledItemDelegate::sizeHint(option, index);
        return result;
    }
};

#endif // EXTENSIONFILEDELEGATE_H
