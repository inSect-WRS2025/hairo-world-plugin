/**
    @author Kenta Suzuki
*/

#ifndef CNOID_MINIO_CLIENT_PLUGIN_MINIO_CLIENT_H
#define CNOID_MINIO_CLIENT_PLUGIN_MINIO_CLIENT_H

#include <cnoid/Referenced>
#include <cnoid/Signal>
#include <QStringList>
#include <vector>
#include "exportdecl.h"

namespace cnoid {

class ExtensionManager;

class CNOID_EXPORT MinIOClient : public Referenced
{
public:
    static void initializeClass(ExtensionManager* ext);

    MinIOClient();
    MinIOClient(const QString& aliasName, const QString& bucketName);
    virtual ~MinIOClient();

    QString alias() const;
    void setAlias(const QString& aliasName);
    QString bucket() const;
    void setBucket(const QString& bucketName);

    void createBucket(const QString& bucketName);
    void deleteBucket(const QString& bucketName);
    void putObject(const QString& fileName, const QString& newPath);
    void getObject(const QString& fileName, const QString& objectKey);
    void deleteObject(const QString& objectKey);
    void listObjects();

    SignalProxy<void(const std::string& bucket_name)> sigBucketCreated();
    SignalProxy<void(const std::string& bucket_name)> sigBucketDeleted();
    SignalProxy<void(const std::string& object_name)> sigObjectUploaded();
    SignalProxy<void(const std::string& object_name)> sigObjectDownloaded();
    SignalProxy<void(const std::string& object_name)> sigObjectDeleted();
    SignalProxy<void(std::vector<std::string> object_names)> sigObjectListed();

private:
    class Impl;
    Impl* impl;
};

typedef ref_ptr<MinIOClient> MinIOClientPtr;

}

#endif // CNOID_MINIO_CLIENT_PLUGIN_MINIO_CLIENT_H