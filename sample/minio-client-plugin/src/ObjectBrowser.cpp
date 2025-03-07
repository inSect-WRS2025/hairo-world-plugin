/**
    @author Kenta Suzuki
*/

#include "ObjectBrowser.h"
#include <cnoid/Buttons>
#include <cnoid/ComboBox>
#include <cnoid/Dialog>
#include <cnoid/ExtensionManager>
#include <cnoid/LineEdit>
#include <cnoid/MainMenu>
#include <cnoid/Separator>
#include <cnoid/TreeWidget>
#include <cnoid/UTF8>
#include <cnoid/stdx/filesystem>
#include <cnoid/MinIOClient>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTreeWidgetItem>
#include <vector>
#include "gettext.h"

using namespace std;
using namespace cnoid;
namespace filesystem = stdx::filesystem;

namespace {

ObjectBrowser* browserInstance = nullptr;

}

namespace cnoid {

class ObjectBrowser::Impl : public Dialog
{
public:

    Impl();
    ~Impl();

    void onTextEdited(const QString& text);
    void onBucketListed(vector<string> bucket_names);
    void onUpdateButtonClicked();
    void onDownloadButtonClicked();
    void onObjectListed(vector<string> object_names);

    LineEdit* aliasLineEdit;
    ComboBox* bucketComboBox;
    TreeWidget* treeWidget;

    Signal<void(const string& object_name)> sigObjectDownloaded_;

    QDialogButtonBox* buttonBox;

    MinIOClientPtr mc0;
    MinIOClientPtr mc1;
    vector<MinIOClientPtr> clients;
};

}


void ObjectBrowser::initializeClass(ExtensionManager* ext)
{
    if(!browserInstance) {
        browserInstance = ext->manage(new ObjectBrowser);

        MainMenu::instance()->add_Tools_Item(
            _("Object Browser"), [](){ browserInstance->impl->show(); });
    }
}


ObjectBrowser* ObjectBrowser::instance()
{
    return browserInstance;
}


ObjectBrowser::ObjectBrowser()
{
    impl = new Impl;
}


ObjectBrowser::Impl::Impl()
    : Dialog()
{
    clients.clear();

    aliasLineEdit = new LineEdit;
    aliasLineEdit->sigTextEdited().connect([&](const QString& text){ onTextEdited(text); });

    bucketComboBox = new ComboBox;
    bucketComboBox->sigCurrentIndexChanged().connect([&](int index){ onUpdateButtonClicked(); });;

    const QIcon icon1 = QIcon::fromTheme("view-refresh");
    auto button1 = new PushButton;
    button1->setIcon(icon1);
    button1->sigClicked().connect([&](){ onUpdateButtonClicked(); });

    const QIcon icon2 = QIcon::fromTheme("emblem-downloads");
    auto button2 = new PushButton;
    button2->setIcon(icon2);
    button2->sigClicked().connect([&](){ onDownloadButtonClicked(); });

    treeWidget = new TreeWidget(this);
    treeWidget->setHeaderLabels(QStringList() << _("Download") << _("Object"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, [&](){ accept(); });

    auto layout = new QHBoxLayout;
    layout->addWidget(new QLabel(_("Alias")));
    layout->addWidget(aliasLineEdit);
    layout->addWidget(bucketComboBox);
    layout->addWidget(button1);
    layout->addWidget(button2);
    // layout->addStretch();

    auto mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout);
    mainLayout->addWidget(treeWidget);
    mainLayout->addWidget(new HSeparator);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(_("Object Browser"));
}


ObjectBrowser::~ObjectBrowser()
{
    delete impl;
}


ObjectBrowser::Impl::~Impl()
{

}


SignalProxy<void(const string& object_name)> ObjectBrowser::sigObjectDownloaded()
{
    return impl->sigObjectDownloaded_;
}


void ObjectBrowser::Impl::onTextEdited(const QString& text)
{
    bucketComboBox->clear();

    mc0 = new MinIOClient;
    mc0->setAlias(text);
    if(!text.isEmpty()) {
        mc0->sigBucketListed().connect([&](vector<string> bucket_names){ onBucketListed(bucket_names); });
        mc0->listBuckets();
    }
}


void ObjectBrowser::Impl::onBucketListed(vector<string> bucket_names)
{
    for(auto& bucket_name : bucket_names) {
        bucketComboBox->addItem(bucket_name.c_str());
    }
}


void ObjectBrowser::Impl::onUpdateButtonClicked()
{
    treeWidget->clear();

    QString aliasName = aliasLineEdit->text();
    QString bucketName = bucketComboBox->currentText();

    if(!bucketName.isEmpty()) {
        mc1 = new MinIOClient;
        mc1->setAlias(aliasName);
        mc1->setBucket(bucketName);
        mc1->sigObjectListed().connect([&](vector<string> object_names){ onObjectListed(object_names); });
        mc1->listObjects();
    }
}


void ObjectBrowser::Impl::onDownloadButtonClicked()
{
    clients.clear();

    QString aliasName = aliasLineEdit->text();
    QString bucketName = bucketComboBox->currentText();

    QStringList items;
    for(int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        auto item = treeWidget->topLevelItem(i);
        if(item->checkState(0) == Qt::Checked) {
            items << item->text(1);
        }
    }

    for(auto& objectName : items) {
        filesystem::path objectPath(fromUTF8(aliasName.toStdString() + "/" + bucketName.toStdString()));
        string object_key = toUTF8((objectPath / filesystem::path(fromUTF8(objectName.toStdString()))).string());
        QString fileName = QString("./minio_ws/%1")
            .arg(objectName);

        auto client = new MinIOClient(aliasName, bucketName);
        client->sigObjectDownloaded().connect([&](string object_name){ sigObjectDownloaded_(object_name); });
        clients.push_back(client);
        client->getObject(fileName, object_key.c_str());
    }
}


void ObjectBrowser::Impl::onObjectListed(vector<string> object_names)
{
    for(auto& object_name : object_names) {
        auto item = new QTreeWidgetItem(treeWidget);
        item->setCheckState(0, Qt::Unchecked);
        item->setText(1, object_name.c_str());
    }

    if(treeWidget->topLevelItemCount() > 0) {
        auto item = treeWidget->topLevelItem(0);
        treeWidget->setCurrentItem(item);
    }
}