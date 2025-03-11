/**
    @author Kenta Suzuki
*/

#ifndef CNOID_BOOKMARK_PLUGIN_LISTED_WIDGET_H
#define CNOID_BOOKMARK_PLUGIN_LISTED_WIDGET_H

#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>
#include "exportdecl.h"

namespace cnoid {

class CNOID_EXPORT ListedWidget : public QWidget
{
public:
    ListedWidget(QWidget* parent = nullptr);
    virtual ~ListedWidget();

    void addWidget(const QString& text, QWidget* widget);
    void addWidget(const QIcon& icon, const QString& text, QWidget* widget);
    void addLayout(const QString& text, QLayout* layout);
    void addLayout(const QIcon& icon, const QString& text, QLayout* layout);

private:
    QListWidget* listWidget;
    QStackedWidget* stackedWidget;
};

}

#endif // CNOID_BOOKMARK_PLUGIN_LISTED_WIDGET_H