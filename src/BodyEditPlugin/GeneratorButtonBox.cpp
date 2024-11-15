/**
   @author Kenta Suzuki
*/

#include "GeneratorButtonBox.h"
#include <cnoid/BodyItem>
#include <cnoid/RootItem>
#include <QBoxLayout>
#include <QPushButton>
#include <cnoid/UTF8>
#include <cnoid/stdx/filesystem>
#include <cnoid/HamburgerMenu>
#include "gettext.h"

using namespace std;
using namespace cnoid;
namespace filesystem = cnoid::stdx::filesystem;

namespace cnoid {

class GeneratorButtonBox::Impl
{
public:
    GeneratorButtonBox* self;

    Impl(GeneratorButtonBox* self);

    void save();
    void saveAs();

    void saveFile(const QString& fileName);
    void reloadItem();

    QString fileName;

    Signal<void(string)> sigSaveTriggered_;
    SignalProxy<void(string)> sigSaveTriggered() {
        return sigSaveTriggered_;
    }
};

}


GeneratorButtonBox::GeneratorButtonBox(QWidget* parent)
    : QDialogButtonBox(parent)
{
    impl = new Impl(this);
}


GeneratorButtonBox::Impl::Impl(GeneratorButtonBox* self)
    : self(self)
{
    const QIcon saveIcon = QIcon::fromTheme("document-save");
    QPushButton* saveButton = new QPushButton(saveIcon, _("&Save"), self);
    self->connect(saveButton, &QPushButton::clicked, [&](){ save(); });

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QPushButton* saveAsButton = new QPushButton(saveAsIcon, _("Save &As..."), self);
    self->connect(saveAsButton, &QPushButton::clicked, [&](){ saveAs(); });

    self->addButton(saveButton, QDialogButtonBox::ActionRole);
    self->addButton(saveAsButton, QDialogButtonBox::ActionRole);
}


GeneratorButtonBox::~GeneratorButtonBox()
{
    delete impl;
}


SignalProxy<void(string)> GeneratorButtonBox::sigSaveTriggered()
{
    return impl->sigSaveTriggered();
}


void GeneratorButtonBox::Impl::save()
{
    if(!fileName.isEmpty()) {
        saveFile(fileName);
    } else {
        saveAs();
    }
}


void GeneratorButtonBox::Impl::saveAs()
{
    fileName = getSaveFileName(_("Body"), "body").c_str();
    saveFile(fileName);
}


void GeneratorButtonBox::Impl::saveFile(const QString& fileName)
{
    string filename = fileName.toStdString();

    if(!filename.empty()) {
        filesystem::path path(fromUTF8(filename));
        string ext = path.extension().string();
        if(ext != ".body") {
           filename += ".body";
        }
        this->fileName = filename.c_str();
        sigSaveTriggered_(filename);
    }

    reloadItem();
}


void GeneratorButtonBox::Impl::reloadItem()
{
    RootItem* rootItem = RootItem::instance();
    ItemList<BodyItem> bodyItems = rootItem->checkedItems<BodyItem>();
    for(auto& bodyItem : bodyItems) {
        string filename0 = fileName.toStdString();
        string filename1 = bodyItem->filePath().c_str();
        if(filename0 == filename1) {
            bodyItem->reload();
        }
    }
}