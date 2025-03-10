/**
   @author Kenta Suzuki
*/

#ifndef CNOID_BODYEDIT_PLUGIN_STAIRS_GENERATOR_H
#define CNOID_BODYEDIT_PLUGIN_STAIRS_GENERATOR_H

namespace cnoid {

class ExtensionManager;

class StairsGenerator
{
public:
    static void initializeClass(ExtensionManager* ext);

    StairsGenerator();
    virtual ~StairsGenerator();
};

}

#endif // CNOID_BODYEDIT_PLUGIN_STAIRS_GENERATOR_H