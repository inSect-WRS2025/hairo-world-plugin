/**
   @author Kenta Suzuki
*/

#include "PipeGenerator.h"
#include <cnoid/Action>
#include <cnoid/Button>
#include <cnoid/Dialog>
#include <cnoid/EigenArchive>
#include <cnoid/EigenTypes>
#include <cnoid/EigenUtil>
#include <cnoid/ExtensionManager>
#include <cnoid/Menu>
#include <cnoid/MenuManager>
#include <cnoid/Separator>
#include <cnoid/SpinBox>
#include <cnoid/UTF8>
#include <cnoid/YAMLWriter>
#include <cnoid/stdx/filesystem>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QContextMenuEvent>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QtMath>
#include "ColorButton.h"
#include "GeneratorButtonBox.h"
#include "WidgetInfo.h"
#include "gettext.h"

using namespace std;
using namespace cnoid;
namespace filesystem = cnoid::stdx::filesystem;

namespace {

PipeGenerator* pipeInstance = nullptr;

DoubleSpinInfo doubleSpinInfo[] = {
    { 0, 1, 0.01, 1000.0, 0.01, 3, 1.00,           "mass", nullptr },
    { 0, 3, 0.01, 1000.0, 0.01, 3, 1.00,         "length", nullptr },
    { 1, 1, 0.01, 1000.0, 0.01, 3, 0.03, "inner_diameter", nullptr },
    { 1, 3, 0.01, 1000.0, 0.01, 3, 0.05, "outer_diameter", nullptr }
};

SpinInfo spinInfo[] = {
    { 3, 3, 0, 359, 1,  0,      "angle", nullptr },
    { 2, 1, 1, 120, 1, 30, "inter_step", nullptr },
    { 2, 3, 1, 120, 1, 30, "outer_step", nullptr }
};

}

namespace cnoid {

class SquarePipeGenerator : public Dialog
{
public:
    SquarePipeGenerator(QWidget* parent = nullptr);

    void setWidth(double width) { widthSpin->setValue(width); }
    double width() const { return widthSpin->value(); }

    void setRadius(double radius) { radiusSpin->setValue(radius); }
    double  radius() const { return radiusSpin->value(); }

    void setLength(double length) { lengthSpin->setValue(length); }
    double length() const { return lengthSpin->value(); }

private:
    DoubleSpinBox* widthSpin;
    DoubleSpinBox* heightSpin;
    DoubleSpinBox* radiusSpin;
    DoubleSpinBox* lengthSpin;
    QDialogButtonBox* buttonBox;
};

class PipeGenerator::Impl : public Dialog
{
public:

    enum DoubleSpinId { MASS, LENGTH, IN_DIA, OUT_DIA, NUM_DSPINS };
    enum SpinId { ANGLE, IN_STEP, OUT_STEP, NUM_SPINS };

    DoubleSpinBox* dspins[NUM_DSPINS];
    SpinBox* spins[NUM_SPINS];
    ColorButton* colorButton;
    GeneratorButtonBox* buttonBox;
    YAMLWriter yamlWriter;
    Action* configureAct;

    Impl();

    virtual void contextMenuEvent(QContextMenuEvent* event) override;

    void configure();

    bool save(const string& filename);
    void onInnerDiameterChanged(const double& diameter);
    void onOuterDiameterChanged(const double& diameter);
    MappingPtr writeBody(const string& filename);
    MappingPtr writeLink();
    void writeLinkShape(Listing* elementsNode);
    VectorXd calcInertia();
};

}


void PipeGenerator::initializeClass(ExtensionManager* ext)
{
    if(!pipeInstance) {
        pipeInstance = ext->manage(new PipeGenerator);

        MenuManager& mm = ext->menuManager().setPath("/Tools").setPath(_("Make a body file"));
        mm.addItem(_("Pipe"))->sigTriggered().connect(
                    [&](){ pipeInstance->impl->show(); });
    }
}


PipeGenerator::PipeGenerator()
{
    impl = new Impl;
}


PipeGenerator::Impl::Impl()
    : Dialog()
{
    setWindowTitle(_("Pipe Generator"));
    yamlWriter.setKeyOrderPreservationMode(true);

    auto gridLayout = new QGridLayout;

    const QStringList list = { _("Mass [kg]"), _("Length [m]"),
                              _("Inner diameter [m]"), _("Outer diameter [m]")
                            };

    for(int i = 0; i < NUM_DSPINS; ++i) {
        DoubleSpinInfo info = doubleSpinInfo[i];
        info.spin = dspins[i] = new DoubleSpinBox;
        info.spin->setRange(info.min, info.max);
        info.spin->setSingleStep(info.step);
        info.spin->setDecimals(info.decimals);
        info.spin->setValue(info.value);
        gridLayout->addWidget(new QLabel(list[i]), info.row, info.column - 1);
        gridLayout->addWidget(info.spin, info.row, info.column);
    }

    const QStringList list2 = { _("Opening angle [deg]"), _("Inner step angle [deg]"), _("Outer step angle [deg]") };

    for(int i = 0; i < NUM_SPINS; ++i) {
        SpinInfo info = spinInfo[i];
        info.spin = spins[i] = new SpinBox;
        info.spin->setRange(info.min, info.max);
        info.spin->setValue(info.value);
        gridLayout->addWidget(new QLabel(list2[i]), info.row, info.column - 1);
        gridLayout->addWidget(info.spin, info.row, info.column);
    }

    colorButton = new ColorButton;
    colorButton->setColor(Vector3(0.5, 0.5, 0.5));
    gridLayout->addWidget(new QLabel(_("Color [-]")), 3, 0);
    gridLayout->addWidget(colorButton, 3, 1);

    buttonBox = new GeneratorButtonBox;

    configureAct = new Action;
    configureAct->setText(_("Advanced settings"));
    configureAct->sigTriggered().connect([this](){ configure(); });

    auto vbox = new QVBoxLayout;
    vbox->addLayout(gridLayout);
    vbox->addStretch();
    vbox->addWidget(new HSeparator);
    vbox->addWidget(buttonBox);
    setLayout(vbox);

    dspins[IN_DIA]->sigValueChanged().connect([&](double value){ onInnerDiameterChanged(value); });
    dspins[OUT_DIA]->sigValueChanged().connect([&](double value){ onOuterDiameterChanged(value); });
    buttonBox->sigSaveTriggered().connect([&](string filename){ save(filename); });
}


PipeGenerator::~PipeGenerator()
{
    delete impl;
}


void PipeGenerator::Impl::contextMenuEvent(QContextMenuEvent* event)
{
    Menu menu(this);
    menu.addAction(configureAct);
    menu.exec(event->globalPos());
}


void PipeGenerator::Impl::configure()
{
    SquarePipeGenerator dialog;
    dialog.setWidth(dspins[OUT_DIA]->value() / qSqrt(2.0));
    dialog.setRadius(dspins[IN_DIA]->value() / 2.0);
    dialog.setLength(dspins[LENGTH]->value());

    if(dialog.exec() == QDialog::Accepted) {
        dspins[LENGTH]->setValue(dialog.length());
        dspins[OUT_DIA]->setValue(dialog.width() * qSqrt(2.0));
        dspins[IN_DIA]->setValue(dialog.radius() * 2.0);
        spins[OUT_STEP]->setValue(90);
        spins[IN_STEP]->setValue(90);
    }
}


bool PipeGenerator::Impl::save(const string& filename)
{
    if(!filename.empty()) {
        auto topNode = writeBody(filename);
        if(yamlWriter.openFile(filename)) {
            yamlWriter.putNode(topNode);
            yamlWriter.closeFile();
        }
    }

    return true;
}


void PipeGenerator::Impl::onInnerDiameterChanged(const double& diameter)
{
    double d_out = dspins[OUT_DIA]->value();
    if(diameter >= d_out) {
        double d_in = d_out - 0.01;
        dspins[IN_DIA]->setValue(d_in);
    }
}


void PipeGenerator::Impl::onOuterDiameterChanged(const double& diameter)
{
    double d_in = dspins[IN_DIA]->value();
    if(diameter <= d_in) {
        double d_out = d_in + 0.01;
        dspins[OUT_DIA]->setValue(d_out);
    }
}


MappingPtr PipeGenerator::Impl::writeBody(const string& filename)
{
    MappingPtr node = new Mapping;

    filesystem::path path(fromUTF8(filename));
    string name = path.stem().string();

    node->write("format", "ChoreonoidBody");
    node->write("format_version", "2.0");
    node->write("angle_unit", "degree");
    node->write("name", name);

    ListingPtr linksNode = new Listing;
    linksNode->append(writeLink());
    if(!linksNode->empty()) {
        node->insert("links", linksNode);
    }

    return node;
}


MappingPtr PipeGenerator::Impl::writeLink()
{
    MappingPtr node = new Mapping;

    double mass = dspins[MASS]->value();

    node->write("name", "Root");
    node->write("joint_type", "free");
    write(node, "center_of_mass", Vector3(0.0, 0.0, 0.0));
    node->write("mass", mass);
    write(node, "inertia", calcInertia());

    ListingPtr elementsNode = new Listing;
    writeLinkShape(elementsNode);
    if(!elementsNode->empty()) {
        node->insert("elements", elementsNode);
    }

    return node;
}


void PipeGenerator::Impl::writeLinkShape(Listing* elementsNode)
{
    MappingPtr node = new Mapping;

    double length = dspins[LENGTH]->value();
    double d_in = dspins[IN_DIA]->value();
    double d_out = dspins[OUT_DIA]->value();
    double r_in = d_in / 2.0;
    double r_out = d_out / 2.0;

    int angle = spins[ANGLE]->value();
    int step_in = spins[IN_STEP]->value();
    int step_out = spins[OUT_STEP]->value();

    node->write("type", "Shape");

    MappingPtr geometryNode = new Mapping;
    geometryNode->write("type", "Extrusion");
    Listing& crossSectionList = *geometryNode->createFlowStyleListing("cross_section");

    int range = 360 - angle;
    int n = ((360 - angle) / step_in + 1) + ((360 - angle) / step_out + 1) + 1;
    double sx;
    double sy;
    for(int i = 0; i <= range; i += step_out) {
        double x = r_out * cos(i * TO_RADIAN);
        double y = r_out * sin(i * TO_RADIAN);
        if(i == 0) {
            sx = x;
            sy = y;
        }
        crossSectionList.append(x, 2, n);
        crossSectionList.append(y, 2, n);
    }
    for(int i = 0; i <= range; i += step_in) {
        double x = r_in * cos((range - i) * TO_RADIAN);
        double y = r_in * sin((range - i) * TO_RADIAN);
        crossSectionList.append(x, 2, n);
        crossSectionList.append(y, 2, n);
    }
    crossSectionList.append(sx, 2, n);
    crossSectionList.append(sy, 2, n);

    VectorXd spine(6);
    spine << 0.0, -length / 2.0, 0.0, 0.0, length / 2.0, 0.0;
    write(geometryNode, "spine", spine);

    node->insert("geometry", geometryNode);

    MappingPtr appearanceNode = node->createFlowStyleMapping("appearance");
    MappingPtr materialNode = appearanceNode->createFlowStyleMapping("material");
    Listing& diffuseColorList = *materialNode->createFlowStyleListing("diffuse");
    Vector3 c = colorButton->color();
    for(int i = 0; i < 3; ++i) {
        diffuseColorList.append(c[i], 3, 3);
    }

    elementsNode->append(node);
}


VectorXd PipeGenerator::Impl::calcInertia()
{
    VectorXd innerInertia, outerInertia;
    innerInertia.resize(9);
    outerInertia.resize(9);

    double length = dspins[LENGTH]->value();
    double d_in = dspins[IN_DIA]->value();
    double d_out = dspins[OUT_DIA]->value();
    double r_in = d_in / 2.0;
    double r_out = d_out / 2.0;

    double innerRate = r_in * r_in / r_out * r_out;
    double outerRate = 1.0 - innerRate;

    {
        double mass = dspins[MASS]->value() * innerRate;
        double radius = r_in;
        double mainInertia = mass * radius * radius / 2.0;
        double subInertia = mass * (3.0 * radius * radius + length * length) / 12.0;
        double ix, iy, iz;
        ix = iz = subInertia;
        iy = mainInertia;
        innerInertia << ix, 0.0, 0.0, 0.0, iy, 0.0, 0.0, 0.0, iz;
    }

    {
        double mass = dspins[MASS]->value() * outerRate;
        double radius = r_out;
        double mainInertia = mass * radius * radius / 2.0;
        double subInertia = mass * (3.0 * radius * radius + length * length) / 12.0;
        double ix, iy, iz;
        ix = iz = subInertia;
        iy = mainInertia;
        outerInertia << ix, 0.0, 0.0, 0.0, iy, 0.0, 0.0, 0.0, iz;
    }

    return outerInertia - innerInertia;
}


SquarePipeGenerator::SquarePipeGenerator(QWidget* parent)
    : Dialog(parent)
{
    setWindowTitle(_("SquarePipe Generator"));

    widthSpin = new DoubleSpinBox;
    widthSpin->setRange(0.01, 1000.0);
    widthSpin->setSingleStep(0.01);
    widthSpin->setDecimals(3);
    widthSpin->sigValueChanged().connect([&](double value){ heightSpin->setValue(value); });

    heightSpin = new DoubleSpinBox;
    heightSpin->setRange(0.01, 1000.0);
    heightSpin->setSingleStep(0.01);
    heightSpin->setDecimals(3);
    heightSpin->setEnabled(false);

    radiusSpin = new DoubleSpinBox;
    radiusSpin->setRange(0.01, 1000.0);
    radiusSpin->setSingleStep(0.01);
    radiusSpin->setDecimals(3);

    lengthSpin = new DoubleSpinBox;
    lengthSpin->setRange(0.01, 1000.0);
    lengthSpin->setSingleStep(0.01);
    lengthSpin->setDecimals(3);

    auto formLayout = new QFormLayout;
    formLayout->addRow(_("Width [m]"), widthSpin);
    formLayout->addRow(_("Height [m]"), heightSpin);
    formLayout->addRow(_("Radius [m]"), radiusSpin);
    formLayout->addRow(_("Length [m]"), lengthSpin);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto vbox = new QVBoxLayout;
    vbox->addLayout(formLayout);
    vbox->addWidget(new HSeparator);
    vbox->addWidget(buttonBox);
    setLayout(vbox);
}