#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QWidget>

class VirtualKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit VirtualKeyboard(QWidget *parent = Q_NULLPTR);

public slots:
    void showKeyboard(int x, int y);
    void hideKeyboard();
    bool keyboardVisible() const;

signals:
    void specialKeyClicked(int key);
    void keyClicked(const QString &text);

private slots:
    void buttonClicked(int key);
};

#endif // VIRTUALKEYBOARD_H
