/**
    @author Kenta Suzuki
*/

#include "SwitchableWidget.h"
#include <QBoxLayout>
#include <QListWidgetItem>

using namespace cnoid;

SwitchableWidget::SwitchableWidget(QWidget* parent)
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


SwitchableWidget::~SwitchableWidget()
{

}


void SwitchableWidget::addWidget(const QString& text, QWidget* widget)
{
    auto item = new QListWidgetItem(listWidget);
    item->setIcon(QIcon());
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}


void SwitchableWidget::addWidget(const QIcon& icon, const QString& text, QWidget* widget)
{
    auto item = new QListWidgetItem(listWidget);
    item->setIcon(icon);
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}


void SwitchableWidget::addLayout(const QString& text, QLayout* layout)
{
    auto widget = new QWidget;
    widget->setLayout(layout);

    auto item = new QListWidgetItem(listWidget);
    item->setIcon(QIcon());
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}


void SwitchableWidget::addLayout(const QIcon& icon, const QString& text, QLayout* layout)
{
    auto widget = new QWidget;
    widget->setLayout(layout);

    auto item = new QListWidgetItem(listWidget);
    item->setIcon(icon);
    item->setText(text);
    listWidget->setCurrentItem(item);
    stackedWidget->addWidget(widget);
}