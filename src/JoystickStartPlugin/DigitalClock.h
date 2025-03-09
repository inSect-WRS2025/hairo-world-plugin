/**
    @author Kenta Suzuki
*/

#ifndef CNOID_JOYSTICKSTART_PLUGIN_DIGITAL_CLOCK_H
#define CNOID_JOYSTICKSTART_PLUGIN_DIGITAL_CLOCK_H

namespace cnoid {

class ExtensionManager;

class DigitalClock
{
public:
    static void initializeClass(ExtensionManager* ext);

    DigitalClock();
    virtual ~DigitalClock();
};

}

#endif // CNOID_JOYSTICKSTART_PLUGIN_DIGITAL_CLOCK_H