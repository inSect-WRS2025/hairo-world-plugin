/**
   @author Kenta Suzuki
*/

#ifndef CNOID_FORMATEDIT_PLUGIN_FORMAT_CONVERTER_H
#define CNOID_FORMATEDIT_PLUGIN_FORMAT_CONVERTER_H

namespace cnoid {

class ExtensionManager;

class FormatConverter
{
public:
    static void initializeClass(ExtensionManager* ext);

    FormatConverter();
    virtual ~FormatConverter();
};

}

#endif // CNOID_FORMATEDIT_PLUGIN_FORMAT_CONVERTER_H