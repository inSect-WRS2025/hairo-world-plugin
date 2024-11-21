/**
   @author Kenta Suzuki
*/

#ifndef CNOID_BODYEDIT_PLUGIN_GENERATOR_BUTTON_BOX_H
#define CNOID_BODYEDIT_PLUGIN_GENERATOR_BUTTON_BOX_H

#include <cnoid/Signal>
#include <QDialogButtonBox>

namespace cnoid {

class GeneratorButtonBox : public QDialogButtonBox
{
public:
    GeneratorButtonBox(QWidget* parent = nullptr);
    virtual ~GeneratorButtonBox();

    SignalProxy<void()> sigResetRequested();
    SignalProxy<void(std::string)> sigSaveRequested();

private:
    class Impl;
    Impl* impl;
};

}

#endif // CNOID_BODYEDIT_PLUGIN_GENERATOR_BUTTON_BOX_H