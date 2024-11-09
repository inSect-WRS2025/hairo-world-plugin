/**
   @author Kenta Suzuki
*/

#ifndef CNOID_BODYEDIT_PLUGIN_GENERATOR_WINDOW_H
#define CNOID_BODYEDIT_PLUGIN_GENERATOR_WINDOW_H

#include <QMainWindow>

namespace cnoid {

class GeneratorWindow : public QMainWindow
{
public:
    GeneratorWindow(QWidget* parent = nullptr);
    virtual ~GeneratorWindow();

protected:
    virtual void saveFile(const std::string& filename);

private:
    class Impl;
    Impl* impl;
};

}

#endif // CNOID_BODYEDIT_PLUGIN_GENERATOR_WINDOW_H