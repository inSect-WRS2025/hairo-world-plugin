/**
   @author Kenta Suzuki
*/

#ifndef CNOID_BODYEDIT_PLUGIN_TERRAIN_GENERATOR_H
#define CNOID_BODYEDIT_PLUGIN_TERRAIN_GENERATOR_H

namespace cnoid {

class ExtensionManager;

class TerrainGenerator
{
public:
    static void initializeClass(ExtensionManager* ext);

    TerrainGenerator();
    virtual ~TerrainGenerator();
};

}

#endif // CNOID_BODYEDIT_PLUGIN_TERRAIN_GENERATOR_H