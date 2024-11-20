/**
   @author Kenta Suzuki
*/

#include "NetworkEmulator.h"
#include <cnoid/Action>
#include <cnoid/Button>
#include <cnoid/ComboBox>
#include <cnoid/Dialog>
#include <cnoid/ExtensionManager>
#include <cnoid/MainMenu>
#include <cnoid/Separator>
#include <cnoid/SpinBox>
#include <cnoid/HamburgerMenu>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include "gettext.h"
#include "NetEm.h"

using namespace cnoid;

namespace {

NetworkEmulator* emulatorInstance = nullptr;

}

namespace cnoid {

class NetworkEmulator::Impl : public Dialog
{
public:

    Impl();

    void onStartButtonToggled(bool checked);

    enum { In, Out, NumInterfaces };

    NetEmPtr emulator;
    ComboBox* interfaceComboBox;
    ComboBox* ifbdeviceComboBox;
    DoubleSpinBox* delaySpinBoxes[NumInterfaces];
    DoubleSpinBox* rateSpinBoxes[NumInterfaces];
    DoubleSpinBox* lossSpinBoxes[NumInterfaces];
    PushButton* startButton;
};

}


void NetworkEmulator::initializeClass(ExtensionManager* ext)
{
    if(!emulatorInstance) {
        emulatorInstance = ext->manage(new NetworkEmulator);

        const QIcon icon = QIcon(":/GoogleMaterialSymbols/icon/network_manage_24dp_5F6368_FILL1_wght400_GRAD0_opsz24.svg");
        auto action = new Action;
        action->setText(_("Network Emulator"));
        action->setIcon(icon);
        action->setToolTip(_("Show the network emulator"));
        action->sigTriggered().connect([&](){ emulatorInstance->impl->show(); });
        HamburgerMenu::instance()->addAction(action);
    }
}


NetworkEmulator::NetworkEmulator()
{
    impl = new Impl;
}


NetworkEmulator::Impl::Impl()
    : Dialog()
{
    setWindowTitle(_("Network Emulator"));

    emulator = new NetEm;

    interfaceComboBox = new ComboBox;
    for(int i = 0; i < emulator->interfaces().size(); ++i) {
        interfaceComboBox->addItem(emulator->interfaces()[i].c_str());
    }

    ifbdeviceComboBox = new ComboBox;
    ifbdeviceComboBox->addItems(QStringList() << "ifb0" << "ifb1");
    ifbdeviceComboBox->setCurrentIndex(1);

    for(int i = 0; i < NumInterfaces; ++i) {
        delaySpinBoxes[i] = new DoubleSpinBox;
        delaySpinBoxes[i]->setRange(0.0, 100000.0);
        rateSpinBoxes[i] = new DoubleSpinBox;
        rateSpinBoxes[i]->setRange(0.0, 11000000.0);
        lossSpinBoxes[i] = new DoubleSpinBox;
        lossSpinBoxes[i]->setRange(0.0, 100.0);
    }

    auto formLayout = new QFormLayout;
    formLayout->addRow(_("Interface"), interfaceComboBox);
    formLayout->addRow(_("IFB Device"), ifbdeviceComboBox);
    formLayout->addRow(_("Inbound Delay [ms]"), delaySpinBoxes[In]);
    formLayout->addRow(_("Inbound Rate [kbit/s]"), rateSpinBoxes[In]);
    formLayout->addRow(_("Inbound Loss [%]"), lossSpinBoxes[In]);
    formLayout->addRow(_("Outbound Delay [ms]"), delaySpinBoxes[Out]);
    formLayout->addRow(_("Outbound Rate [kbit/s]"), rateSpinBoxes[Out]);
    formLayout->addRow(_("Outbound Loss [%]"), lossSpinBoxes[Out]);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    startButton = new PushButton(_("&Start"));
    startButton->setCheckable(true);
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    startButton->sigToggled().connect([&](bool checked){ onStartButtonToggled(checked); });

    auto vbox = new QVBoxLayout;
    vbox->addLayout(formLayout);
    vbox->addStretch();
    vbox->addWidget(new HSeparator);
    vbox->addWidget(buttonBox);
    setLayout(vbox);
}


NetworkEmulator::~NetworkEmulator()
{
    delete impl;
}


void NetworkEmulator::Impl::onStartButtonToggled(bool checked)
{
    if(checked) {
        startButton->setText(_("&Stop"));
        emulator->start(interfaceComboBox->currentIndex(), ifbdeviceComboBox->currentIndex());
        for(int i = 0; i < NumInterfaces; ++i) {
            emulator->setDelay(i, delaySpinBoxes[i]->value());
            emulator->setRate(i, rateSpinBoxes[i]->value());
            emulator->setLoss(i, lossSpinBoxes[i]->value());
        }
        emulator->update();
    } else {
        startButton->setText(_("&Start"));
        emulator->stop();
    }
}
