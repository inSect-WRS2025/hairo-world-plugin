/**
   @author Kenta Suzuki
*/

#include "InertiaCalculator.h"
#include <cnoid/Action>
#include <cnoid/Buttons>
#include <cnoid/ComboBox>
#include <cnoid/Dialog>
#include <cnoid/EigenTypes>
#include <cnoid/ExtensionManager>
#include <cnoid/Format>
#include <cnoid/MainMenu>
#include <cnoid/MessageView>
#include <cnoid/Separator>
#include <cnoid/SpinBox>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QStackedLayout>
#include "gettext.h"

using namespace std;
using namespace cnoid;

namespace {

struct DoubleSpinInfo {
    int page;
    QDoubleSpinBox* spin;
};

DoubleSpinInfo doubleSpinInfo[] = {
    { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr },
    { 1, nullptr }, { 1, nullptr },
    { 2, nullptr }, { 2, nullptr }, { 2, nullptr },
    { 3, nullptr }, { 3, nullptr }, { 3, nullptr }
};

class CalculatorDialog : public QDialog
{
public:
    CalculatorDialog(QWidget* parent = nullptr);

private:
    void calc();

    enum {
        BOX_MAS, BOX_X, BOX_Y, BOX_Z,
        SPR_MAS, SPR_RAD,
        CLD_MAS, CLD_RAD, CLD_HGT,
        CON_MAS, CON_RAD, CON_HGT,
        NumDoubleSpinBoxes
    };
    enum { Page_Box, Page_Sphere, Page_Cylinder, Page_Cone, NumPages };
    enum Axis { XAxis, YAxis, ZAxis };

    QComboBox* shapeComboBox;
    QComboBox* axisComboBox;
    QComboBox* axisComboBox2;
    QDoubleSpinBox* doubleSpinBoxes[NumDoubleSpinBoxes];
    QDialogButtonBox* buttonBox;
};

}


void InertiaCalculator::initializeClass(ExtensionManager* ext)
{
    static CalculatorDialog* dialog = nullptr;

    if(!dialog) {
        dialog = ext->manage(new CalculatorDialog);

        MainMenu::instance()->add_Tools_Item(
            _("Inertia Calculator"), [](){ dialog->show(); });
    }
}


InertiaCalculator::InertiaCalculator()
{

}


InertiaCalculator::~InertiaCalculator()
{

}


CalculatorDialog::CalculatorDialog(QWidget* parent)
    : QDialog(parent)
{
    auto stackedLayout = new QStackedLayout;

    const QStringList texts = { _("Box"), _("Sphere"), _("Cylinder"), _("Cone") };
    shapeComboBox = new QComboBox;
    shapeComboBox->addItems(texts);
    connect(shapeComboBox, QOverload<int>::of(&QComboBox::activated),
        [&, stackedLayout](int index){ stackedLayout->setCurrentIndex(index); });

    const QStringList texts2 = { "X", "Y", "Z" };
    axisComboBox = new QComboBox;
    axisComboBox->addItems(texts2);

    axisComboBox2 = new QComboBox;
    axisComboBox2->addItems(texts2);

    QFormLayout* formLayout[NumPages];
    for(int i = 0; i < NumPages; ++i) {
        QWidget* pageWidget = new QWidget;
        formLayout[i] = new QFormLayout;
        auto layout = new QVBoxLayout;
        layout->addLayout(formLayout[i]);
        layout->addStretch();
        pageWidget->setLayout(layout);
        stackedLayout->addWidget(pageWidget);
    }

    const QStringList list = {
        _("mass [kg]"), _("x [m]"), _("y [m]"), _("z [m]"),
        _("mass [kg]"), _("radius [m]"),
        _("mass [kg]"), _("radius [m]"), _("height [m]"),
        _("mass [kg]"), _("radius [m]"), _("height [m]")
    };

    for(int i = 0; i < NumDoubleSpinBoxes; ++i) {
        DoubleSpinInfo info = doubleSpinInfo[i];
        info.spin = doubleSpinBoxes[i] = new QDoubleSpinBox;
        info.spin->setDecimals(7);
        info.spin->setSingleStep(0.01);
        info.spin->setRange(0.0, 9999.999);
        formLayout[info.page]->addRow(list[i], info.spin);
    }

    formLayout[Page_Cylinder]->addRow(_("axis [-]"), axisComboBox);
    formLayout[Page_Cone]->addRow(_("axis [-]"), axisComboBox2);

    const QIcon calcIcon = QIcon::fromTheme("accessories-calculator");
    QPushButton* calcButton = new QPushButton(calcIcon, _("&Calc"), this);
    connect(calcButton, &QPushButton::clicked, [&](){ calc(); });

    buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton(calcButton, QDialogButtonBox::ActionRole);

    auto layout = new QHBoxLayout;
    layout->addWidget(new QLabel(_("Shape")));
    layout->addWidget(shapeComboBox);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout);
    mainLayout->addLayout(stackedLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(new HSeparator);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(_("Inertia Calculator"));
}


void CalculatorDialog::calc()
{
    MessageView* mv = MessageView::instance();

    double ix, iy, iz = 0.0;
    int index = shapeComboBox->currentIndex();
    if(index == Page_Box) {
        double m = doubleSpinBoxes[BOX_MAS]->value();
        double x = doubleSpinBoxes[BOX_X]->value();
        double y = doubleSpinBoxes[BOX_Y]->value();
        double z = doubleSpinBoxes[BOX_Z]->value();

        ix = m / 12.0 * (y * y + z * z);
        iy = m / 12.0 * (z * z + x * x);
        iz = m / 12.0 * (x * x + y * y);

        mv->putln(formatR(_("shape: Box, mass: {0} [kg], x: {1} [m], y: {2} [m], z: {3} [m]"),
                                    m, x, y, z));
    } else if(index == Page_Sphere) {
        double m = doubleSpinBoxes[SPR_MAS]->value();
        double r = doubleSpinBoxes[SPR_RAD]->value();

        ix = iy = iz = m * r * r / 5.0 * 2.0;

        mv->putln(formatR(_("shape: Sphere, mass: {0} [kg], radius: {1} [m]"),
                                    m, r));
    } else if(index == Page_Cylinder) {
        double m = doubleSpinBoxes[CLD_MAS]->value();
        double r = doubleSpinBoxes[CLD_RAD]->value();
        double h = doubleSpinBoxes[CLD_HGT]->value();
        int index = axisComboBox->currentIndex();

        double main_inertia = m * r * r / 2.0;
        double sub_inertia = m * (3.0 * r * r + h * h) / 12.0;

        if(index == XAxis) {
            ix = main_inertia;
            iy = iz = sub_inertia;
        } else if(index == YAxis) {
            iy = main_inertia;
            iz = ix = sub_inertia;
        } else if(index == ZAxis) {
            iz = main_inertia;
            ix = iy = sub_inertia;
        }

        mv->putln(formatR(_("shape: Cylinder, mass: {0} [kg], radius: {1} [m], height: {2} [m], axis: {3} [-]"),
                                    m, r, h, axisComboBox->currentText().toStdString()));
    } else if(index == Page_Cone) {
        double m = doubleSpinBoxes[CON_MAS]->value();
        double r = doubleSpinBoxes[CON_RAD]->value();
        double h = doubleSpinBoxes[CON_HGT]->value();
        int index = axisComboBox2->currentIndex();

        double main_inertia = m * r * r * 3.0 / 10.0;
        double sub_inertia = m * 3.0 / 80.0 * (4.0 * r * r + h * h);

        if(index == XAxis) {
            ix = main_inertia;
            iy = iz = sub_inertia;
        } else if(index == YAxis) {
            iy = main_inertia;
            iz = ix = sub_inertia;
        } else if(index == ZAxis) {
            iz = main_inertia;
            ix = iy = sub_inertia;
        }

        mv->putln(formatR(_("shape: Cone, mass: {0} [kg], radius: {1} [m], height: {2} [m], axis: {3} [-]"),
                                    m, r, h, axisComboBox2->currentText().toStdString()));
    }

    Vector3 inertia(ix, iy, iz);
    mv->putln(formatR(_("inertia: [ {0}, 0, 0, 0, {1}, 0, 0, 0, {2} ]\n"),
                          inertia[0], inertia[1], inertia[2]));
}