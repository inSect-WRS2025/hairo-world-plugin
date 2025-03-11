/**
    @author Kenta Suzuki
*/

#include "ProjectListedDialog.h"
#include <cnoid/ExtensionManager>
#include <cnoid/MainMenu>
#include "gettext.h"

using namespace cnoid;

void ProjectListedDialog::initializeClass(ExtensionManager* ext)
{
    MainMenu::instance()->add_Tools_Item(
        _("Show the manager list"), [](){ ProjectListedDialog::instance()->show(); });
}


ProjectListedDialog* ProjectListedDialog::instance()
{
    static ProjectListedDialog* dialog = new ProjectListedDialog;
    return dialog;
}


ProjectListedDialog::ProjectListedDialog(QWidget* parent)
    : ListedWidget(parent)
{
    setWindowTitle(_(" "));
}


ProjectListedDialog::~ProjectListedDialog()
{

}