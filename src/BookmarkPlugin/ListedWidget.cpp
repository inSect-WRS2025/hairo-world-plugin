/**
    @author Kenta Suzuki
*/

#include "ListedWidget.h"
#include <QBoxLayout>
#include <QListWidgetItem>

using namespace cnoid;

ListedWidget::ListedWidget(QWidget* parent)
    : QWidget(parent)
{
    stackedWidget = new QStackedWidget(this);

    listWidget = new QListWidget(this);
    listWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(listWidget, &QListWidget::currentRowChanged,
        [&](int currentRow){ stackedWidget->setCurrentIndex(currentRow); });

    auto layout = new QHBoxLayout;
    layout->addWidget(listWidget);
    layout->addWidget(stackedWidget);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout);
    setLayout(mainLayout);

    setWindowTitle("");
}


ListedWidget::~ListedWidget()
{

}


void ListedWidget::addWidget(const QString& text, QWidget* widget)
{
    auto item = new QListWidgetItem(listWidget);
    item->setIcon(QIcon());
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}


void ListedWidget::addWidget(const QIcon& icon, const QString& text, QWidget* widget)
{
    auto item = new QListWidgetItem(listWidget);
    item->setIcon(icon);
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}


void ListedWidget::addLayout(const QString& text, QLayout* layout)
{
    auto widget = new QWidget;
    widget->setLayout(layout);

    auto item = new QListWidgetItem(listWidget);
    item->setIcon(QIcon());
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}


void ListedWidget::addLayout(const QIcon& icon, const QString& text, QLayout* layout)
{
    auto widget = new QWidget;
    widget->setLayout(layout);

    auto item = new QListWidgetItem(listWidget);
    item->setIcon(icon);
    item->setText(text);
    // auto firstItem = listWidget->item(0);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}