/**
   @author Kenta Suzuki
*/

#include "SlopeGenerator.h"
#include <cnoid/Button>
#include <cnoid/Dialog>
#include <cnoid/EigenArchive>
#include <cnoid/EigenTypes>
#include <cnoid/EigenUtil>
#include <cnoid/ExtensionManager>
#include <cnoid/MenuManager>
#include <cnoid/Separator>
#include <cnoid/SpinBox>
#include <cnoid/UTF8>
#include <cnoid/YAMLWriter>
#include <cnoid/stdx/filesystem>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include "ColorButton.h"
#include "GeneratorButtonBox.h"
#include "WidgetInfo.h"
#include "gettext.h"

using namespace std;
using namespace cnoid;
namespace filesystem = cnoid::stdx::filesystem;

namespace {

SlopeGenerator* slopeInstance = nullptr;

DoubleSpinInfo doubleSpinInfo[] = {
    { 0, 1, 0.001, 1000.0, 0.01, 3, 1.0,   "mass", nullptr },
    { 0, 3, 0.001, 1000.0, 0.01, 3, 1.0,  "width", nullptr },
    { 1, 1, 0.001, 1000.0, 0.01, 3, 1.0, "height", nullptr },
    { 1, 3, 0.001, 1000.0, 0.01, 3, 1.0, "length", nullptr }
};

}

namespace cnoid {

class SlopeGenerator::Impl : public Dialog
{
public:

    enum {
        MASS, WIDTH, HEIGHT,
        LENGTH, NumDoubleSpinBoxes
    };

    DoubleSpinBox* doubleSpinBoxes[NumDoubleSpinBoxes];
    ColorButton* colorButton;
    GeneratorButtonBox* buttonBox;
    YAMLWriter yamlWriter;

    Impl();

    bool save(const string& filename);
    MappingPtr writeBody(const string& filename);
    MappingPtr writeLink();
    void writeLinkShape(Listing* elementsNode);
    VectorXd calcInertia();
};

}


void SlopeGenerator::initializeClass(ExtensionManager* ext)
{
    if(!slopeInstance) {
        slopeInstance = ext->manage(new SlopeGenerator);

        MenuManager& mm = ext->menuManager().setPath("/Tools").setPath(_("Make a body file"));
        mm.addItem(_("Slope"))->sigTriggered().connect(
                    [&](){ slopeInstance->impl->show(); });
    }
}


SlopeGenerator::SlopeGenerator()
{
    impl = new Impl;
}


SlopeGenerator::Impl::Impl()
    : Dialog()
{
    setWindowTitle(_("Slope Generator"));
    yamlWriter.setKeyOrderPreservationMode(true);

    auto gridLayout = new QGridLayout;

    const QStringList list = {
        _("Mass [kg]"), _("Width [m]"), _("Height [m]"), _("Length [m]")
    };

    for(int i = 0; i < NumDoubleSpinBoxes; ++i) {
        DoubleSpinInfo info = doubleSpinInfo[i];
        info.spin = doubleSpinBoxes[i] = new DoubleSpinBox;
        info.spin->setRange(info.min, info.max);
        info.spin->setSingleStep(info.step);
        info.spin->setDecimals(info.decimals);
        info.spin->setValue(info.value);
        gridLayout->addWidget(new QLabel(list[i]), info.row, info.column - 1);
        gridLayout->addWidget(info.spin, info.row, info.column);
    }

    colorButton = new ColorButton;
    colorButton->setColor(Vector3(0.5, 0.5, 0.5));
    gridLayout->addWidget(new QLabel(_("Color [-]")), 2, 0);
    gridLayout->addWidget(colorButton, 2, 1);

    buttonBox = new GeneratorButtonBox;

    auto vbox = new QVBoxLayout;
    vbox->addLayout(gridLayout);
    vbox->addStretch();
    vbox->addWidget(new HSeparator);
    vbox->addWidget(buttonBox);
    setLayout(vbox);

    buttonBox->sigSaveTriggered().connect([&](string filename){ save(filename); });
}


SlopeGenerator::~SlopeGenerator()
{
    delete impl;
}


bool SlopeGenerator::Impl::save(const string& filename)
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


MappingPtr SlopeGenerator::Impl::writeBody(const string& filename)
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


MappingPtr SlopeGenerator::Impl::writeLink()
{
    MappingPtr node = new Mapping;

    double mass = doubleSpinBoxes[MASS]->value();

    node->write("name", "Root");
    node->write("joint_type", "fixed");
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


void SlopeGenerator::Impl::writeLinkShape(Listing* elementsNode)
{
    MappingPtr node = new Mapping;

    double length = doubleSpinBoxes[LENGTH]->value();
    double width = doubleSpinBoxes[WIDTH]->value();
    double height = doubleSpinBoxes[HEIGHT]->value();

    node->write("type", "Shape");

    MappingPtr geometryNode = new Mapping;
    geometryNode->write("type", "Extrusion");
    Listing& crossSectionList = *geometryNode->createFlowStyleListing("cross_section");

    int n = 8;
    double hl = length / 2.0;
    double hw = width / 2.0;
    double hh = height / 2.0;
    crossSectionList.append(-hl, 2, n);
    crossSectionList.append(-hh, 2, n);
    crossSectionList.append( hl, 2, n);
    crossSectionList.append(-hh, 2, n);
    crossSectionList.append( hl, 2, n);
    crossSectionList.append( hh, 2, n);
    crossSectionList.append(-hl, 2, n);
    crossSectionList.append(-hh, 2, n);

    VectorXd spine(6);
    spine << 0.0, -hw, 0.0, 0.0, hw, 0.0;
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


VectorXd SlopeGenerator::Impl::calcInertia()
{
    VectorXd inertia;
    inertia.resize(9);
    inertia << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

    double mass = doubleSpinBoxes[MASS]->value();
    double x = doubleSpinBoxes[LENGTH]->value();
    double y = doubleSpinBoxes[WIDTH]->value();
    double z = doubleSpinBoxes[HEIGHT]->value();

    double ix = mass / 12.0 * (y * y + z * z);
    double iy = mass / 12.0 * (z * z + x * x);
    double iz = mass / 12.0 * (x * x + y * y);

    inertia << ix, 0.0, 0.0, 0.0, iy, 0.0, 0.0, 0.0, iz;

    return inertia;
}
