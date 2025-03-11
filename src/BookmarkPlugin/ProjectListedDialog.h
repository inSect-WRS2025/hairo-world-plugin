/**
    @author Kenta Suzuki
*/

#ifndef CNOID_BOOKMARK_PLUGIN_PROJECT_LISTED_DIALOG_H
#define CNOID_BOOKMARK_PLUGIN_PROJECT_LISTED_DIALOG_H

#include "ListedWidget.h"

namespace cnoid {

class ExtensionManager;

class ProjectListedDialog : public ListedWidget
{
public:
    static void initializeClass(ExtensionManager* ext);
    static ProjectListedDialog* instance();

    ProjectListedDialog(QWidget* parent = nullptr);
    virtual ~ProjectListedDialog();
};

}

#endif // CNOID_BOOKMARK_PLUGIN_PROJECT_LISTED_DIALOG_H