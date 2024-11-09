/**
   @author Kenta Suzuki
*/

#include "GeneratorWindow.h"
#include <QAction>
#include <cnoid/BodyItem>
#include <cnoid/RootItem>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <cnoid/UTF8>
#include <cnoid/stdx/filesystem>
#include <cnoid/HamburgerMenu>
#include "gettext.h"

using namespace std;
using namespace cnoid;
namespace filesystem = cnoid::stdx::filesystem;

namespace cnoid {

class GeneratorWindow::Impl
{
public:
    GeneratorWindow* self;

    Impl(GeneratorWindow* self);

    void newFile();
    void save();
    void saveAs();
    void exportFile();
    void importFile();

    void createActions();
    void createMenus();
    void createToolBars();
    void saveFile(const QString& fileName);
    void reloadItem();

    QMenu* fileMenu;
    QToolBar* fileToolBar;
    QAction* newAct;
    QAction* saveAct;
    QAction* saveAsAct;
    QAction* exportAct;
    QAction* importAct;
    QAction* exitAct;

    QLineEdit* fileLine;
};

}


GeneratorWindow::GeneratorWindow(QWidget* parent)
    : QMainWindow(parent)
{
    impl = new Impl(this);
}


GeneratorWindow::Impl::Impl(GeneratorWindow* self)
    : self(self)
{
    self->setWindowFlag(Qt::WindowStaysOnTopHint);

    createActions();
    // createMenus();
    createToolBars();
}


GeneratorWindow::~GeneratorWindow()
{
    delete impl;
}


void GeneratorWindow::saveFile(const string& filename)
{

}


void GeneratorWindow::Impl::newFile()
{

}


void GeneratorWindow::Impl::save()
{
    saveAs();
}


void GeneratorWindow::Impl::saveAs()
{
    string filename = fileLine->text().toStdString();

    if(filename.empty()) {
        filename = getSaveFileName("Save a body", "body");
        fileLine->setText(filename.c_str());
    }

    if(!filename.empty()) {
        filesystem::path path(fromUTF8(filename));
        string ext = path.extension().string();
        if(ext != ".body") {
           filename += ".body";
        }
        fileLine->setText(filename.c_str());
        saveFile(fileLine->text());
    }

    reloadItem();
}


void GeneratorWindow::Impl::exportFile()
{

}


void GeneratorWindow::Impl::importFile()
{

}


void GeneratorWindow::Impl::createActions()
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

    const QIcon exportIcon = QIcon::fromTheme("");
    exportAct = new QAction(exportIcon, _("&Export"), self);
    exportAct->setStatusTip(_("Export the configurations"));
    self->connect(exportAct, &QAction::triggered, [&](){ exportFile(); });

    const QIcon importIcon = QIcon::fromTheme("");
    importAct = new QAction(importIcon, _("&Import"), self);
    importAct->setStatusTip(_("Import the configurations"));
    self->connect(importAct, &QAction::triggered, [&](){ importFile(); });

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    exitAct = new QAction(exitIcon, ("E&xit"), self);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    self->connect(exitAct, &QAction::triggered, [&](){ self->close(); });
}


void GeneratorWindow::Impl::createMenus()
{
    fileMenu = self->menuBar()->addMenu(_("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(exportAct);
    fileMenu->addAction(importAct);
    fileMenu->addAction(exitAct);
    fileMenu->addSeparator();
}


void GeneratorWindow::Impl::createToolBars()
{
    fileToolBar = self->addToolBar(_("File"));
    fileLine = new QLineEdit;
    fileToolBar->addWidget(fileLine);
    // fileToolBar->addAction(newAct);
    fileToolBar->addAction(saveAct);
    // fileToolBar->addAction(saveAsAct);
    fileToolBar->addSeparator();
}


void GeneratorWindow::Impl::saveFile(const QString& fileName)
{
    self->saveFile(fileName.toStdString());
}


void GeneratorWindow::Impl::reloadItem()
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