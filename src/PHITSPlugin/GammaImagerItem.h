/**
   @author Kenta Suzuki
*/

#ifndef CNOID_PHITS_PLUGIN_GAMMA_IMAGER_ITEM_H
#define CNOID_PHITS_PLUGIN_GAMMA_IMAGER_ITEM_H

#include <cnoid/Item>
#include "exportdecl.h"

namespace cnoid {

class CNOID_EXPORT GammaImagerItem : public Item
{
public:
    static void initializeClass(ExtensionManager* ext);

    GammaImagerItem();
    GammaImagerItem(const GammaImagerItem& org);
    virtual ~GammaImagerItem();

    std::string defaultNuclideTableFile() const;
    std::string defaultElementTableFile() const;
    void setDefaultEnergyFilterFile(const std::string& filename);
    std::string defaultEnergyFilterFile() const;

protected:
    virtual Item* doCloneItem(CloneMap* cloneMap) const override;
    virtual void doPutProperties(PutPropertyFunction& putProperty) override;
    virtual void onPositionChanged() override;
    virtual void onDisconnectedFromRoot() override;
    virtual bool store(Archive& archive) override;
    virtual bool restore(const Archive& archive) override;


private:
    class Impl;
    Impl* impl;
};

typedef ref_ptr<GammaImagerItem> GammaImagerItemPtr;

}

#endif // CNOID_PHITS_PLUGIN_GAMMA_IMAGER_ITEM_H
