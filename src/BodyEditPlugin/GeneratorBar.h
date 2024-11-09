/**
   @author Kenta Suzuki
*/

#ifndef CNOID_BODYEDIT_PLUGIN_GENERATOR_BAR_H
#define CNOID_BODYEDIT_PLUGIN_GENERATOR_BAR_H

#include <cnoid/Signal>
#include <QToolBar>

namespace cnoid {

class GeneratorBar : public QToolBar
{
public:
    GeneratorBar(QWidget* parent = nullptr);
    virtual ~GeneratorBar();

    SignalProxy<void(std::string)> sigSaveTriggered();

private:
    class Impl;
    Impl* impl;
};

}

#endif // CNOID_BODYEDIT_PLUGIN_GENERATOR_BAR_H