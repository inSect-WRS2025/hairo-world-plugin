/**
    @author Kenta Suzuki
*/

#include "DigitalClock.h"
#include <cnoid/Dialog>
#include <cnoid/ExtensionManager>
#include <cnoid/MainMenu>
#include <cnoid/Separator>
#include <cnoid/TimeBar>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLCDNumber>
#include <QTime>
#include "gettext.h"

using namespace cnoid;

namespace {

class ClockDialog : public QDialog
{
public:
    ClockDialog(QWidget* parent = nullptr);

private:
    void onTimeChanged(double time);

    QLCDNumber* lcdNumber;
    QDialogButtonBox* buttonBox;
};

}


void DigitalClock::initializeClass(ExtensionManager* ext)
{
    static ClockDialog* dialog = nullptr;

    if(!dialog) {
        dialog = ext->manage(new ClockDialog);

        MainMenu::instance()->add_Tools_Item(
            _("Digital Clock"), [](){ dialog->show(); });
    }
}


DigitalClock::DigitalClock()
{

}


DigitalClock::~DigitalClock()
{

}


ClockDialog::ClockDialog(QWidget* parent)
    : QDialog(parent)
{
    lcdNumber = new QLCDNumber(this);
    lcdNumber->setSegmentStyle(QLCDNumber::Filled);
    lcdNumber->setWindowFlags(Qt::WindowStaysOnTopHint);
    lcdNumber->resize(150, 60);

    TimeBar::instance()->sigTimeChanged().connect(
        [&](double time){ onTimeChanged(time); return true; });

    onTimeChanged(0.0);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, [&](){ accept(); });

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(lcdNumber);
    mainLayout->addWidget(new HSeparator);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(_("Digital Clock"));
}


void ClockDialog::onTimeChanged(double time)
{
    int minute = time / 60.0;
    int second = time - minute * 60.0;
    QTime currentTime(0, minute, second, 0);
    QString text = currentTime.toString("mm:ss");
    // if((currentTime.second() % 2) == 0) {
    //     text[2] = ' ';
    // }
    lcdNumber->display(text);
}