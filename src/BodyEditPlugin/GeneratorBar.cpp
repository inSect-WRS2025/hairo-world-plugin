/**
   @author Kenta Suzuki
*/

#include "GeneratorBar.h"
#include <QAction>
#include <cnoid/BodyItem>
#include <cnoid/RootItem>
#include <QLineEdit>
#include <cnoid/UTF8>
#include <cnoid/stdx/filesystem>
#include <cnoid/HamburgerMenu>
#include "gettext.h"

using namespace std;
using namespace cnoid;
namespace filesystem = cnoid::stdx::filesystem;

namespace cnoid {

class GeneratorBar::Impl
{
public:
    GeneratorBar* self;

    Impl(GeneratorBar* self);

    void newFile();
    void save();
    void saveAs();

    void createActions();
    void saveFile(const QString& fileName);
    void reloadItem();

    QAction* newAct;
    QAction* saveAct;
    QAction* saveAsAct;
    QLineEdit* fileLine;

    Signal<void(string)> sigSaveTriggered_;
    SignalProxy<void(string)> sigSaveTriggered() {
        return sigSaveTriggered_;
    }
};

}


GeneratorBar::GeneratorBar(QWidget* parent)
    : QToolBar("File", parent)
{
    impl = new Impl(this);
}


GeneratorBar::Impl::Impl(GeneratorBar* self)
    : self(self)
{
    createActions();

    fileLine = new QLineEdit;
    self->addWidget(fileLine);
    // self->addAction(newAct);
    self->addAction(saveAct);
    // self->addAction(saveAsAct);
}


GeneratorBar::~GeneratorBar()
{
    delete impl;
}


SignalProxy<void(string)> GeneratorBar::sigSaveTriggered()
{
    return impl->sigSaveTriggered();
}


void GeneratorBar::Impl::newFile()
{

}


void GeneratorBar::Impl::save()
{
    QString fileName = fileLine->text();
    if(!fileName.isEmpty()) {
        saveFile(fileName);
    } else {
        saveAs();
    }
}


void GeneratorBar::Impl::saveAs()
{
    QString fileName = getSaveFileName(_("Body"), "body").c_str();
    fileLine->setText(fileName);
    saveFile(fileName);
}


void GeneratorBar::Impl::createActions()
{
    const QIcon newIcon = QIcon::fromTheme("document-new");
    newAct = new QAction(newIcon, _("&New"), self);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(_("Create a new file"));
    self->connect(newAct, &QAction::triggered, [&](){ newFile(); });

    const QIcon saveIcon = QIcon::fromTheme("document-save");
    saveAct = new QAction(saveIcon, _("&Save"), self);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(_("Save the document to disk"));
    self->connect(saveAct, &QAction::triggered, [&](){ save(); });

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, _("Save &As..."), self);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(("Save the document under a new name"));
    self->connect(saveAsAct, &QAction::triggered, [&](){ saveAs(); });
}


void GeneratorBar::Impl::saveFile(const QString& fileName)
{
    string filename = fileName.toStdString();

    if(!filename.empty()) {
        filesystem::path path(fromUTF8(filename));
        string ext = path.extension().string();
        if(ext != ".body") {
           filename += ".body";
        }
        fileLine->setText(filename.c_str());
        sigSaveTriggered_(filename);
    }

    reloadItem();
}


void GeneratorBar::Impl::reloadItem()
{
    RootItem* rootItem = RootItem::instance();
    ItemList<BodyItem> bodyItems = rootItem->checkedItems<BodyItem>();
    for(auto& bodyItem : bodyItems) {
        string filename0 = fileLine->text().toStdString();
        string filename1 = bodyItem->filePath().c_str();
        if(filename0 == filename1) {
            bodyItem->reload();
        }
    }
}