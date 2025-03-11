/**
   @author Kenta Suzuki
*/

#include <cnoid/Format>
#include <cnoid/Plugin>
#include "BodyCreator.h"
#include "BodyCreatorDialog.h"
#include "FormatConverter.h"
#include "InertiaCalculator.h"

using namespace cnoid;

class BodyEditPlugin : public Plugin
{
public:

    BodyEditPlugin() : Plugin("BodyEdit")
    {
        require("Body");
        require("Bookmark");
    }

    virtual bool initialize() override
    {
        BodyCreatorDialog::initializeClass(this);
        BentPipeCreator::initializeClass(this);
        CrawlerCreator::initializeClass(this);
        GratingCreator::initializeClass(this);
        PipeCreator::initializeClass(this);
        SlopeCreator::initializeClass(this);
        StairsCreator::initializeClass(this);
        TerrainCreator::initializeClass(this);
        InertiaCalculator::initializeClass(this);
        FormatConverter::initializeClass(this);
        return true;
    }

    virtual const char* description() const override
    {
        static std::string text =
            formatC("BodyEdit Plugin Version {}\n", CNOID_FULL_VERSION_STRING) +
            "\n" +
            "Copyright (c) 2023 Japan Atomic Energy Agency.\n"
            "\n" +
            MITLicenseText();
        return text.c_str();
    }
};

CNOID_IMPLEMENT_PLUGIN_ENTRY(BodyEditPlugin)