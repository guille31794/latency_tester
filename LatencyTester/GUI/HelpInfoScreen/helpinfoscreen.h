#ifndef HELPINFOSCREEN_H
#define HELPINFOSCREEN_H

#include <QWidget>
#include <QUrl>

namespace Ui {
class HelpInfoScreen;
}

/**
 * @brief The HelpInfoScreen class displays read-only information.
 * Can show plain text or HTML content from embedded resources.
 */
class HelpInfoScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HelpInfoScreen(QWidget *parent = nullptr);
    ~HelpInfoScreen();
    void changeEvent(QEvent *event) override;

    /**
     * @brief setContent sets plain text content (for general info, etc.)
     */
    void setContent(const QString& text);

    /**
     * @brief setHtmlResource loads an HTML file from Qt resources (qrc).
     * @param resourcePath e.g. "qrc:/help/help/manual_es.html"
     */
    void setHtmlResource(const QString& resourcePath);

signals:
    void backRequested();

private slots:
    void on_backButton_released();

private:
    Ui::HelpInfoScreen *ui;
};

#endif // HELPINFOSCREEN_H
