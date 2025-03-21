/**
    Spot Liedown Controller
    @author Kenta Suzuki
*/

#include <cnoid/EigenUtil>
#include <cnoid/SimpleController>
#include <cnoid/SharedJoystick>
#include <vector>

using namespace std;
using namespace cnoid;

namespace {

const double down[] = {
    0.0, 96.2, -143.4, 0.0, 96.2, -143.4,
    0.0, 96.2, -143.4, 0.0, 96.2, -143.4
};

const double up[] = {
    0.0, 60.4, -96.2, 0.0, 60.4, -96.2,
    0.0, 60.4, -96.2, 0.0, 60.4, -96.2
};

}

class SpotLiedownController : public SimpleController
{
    SimpleControllerIO* io;
    int legActuationMode;
    Link* legJoint[12];
    double qref[12];
    double qprev[12];
    double dt;

    struct StateInfo {
        int buttonId;
        bool prevButtonState;
        bool stateChanged;
        const double* angleMap;
        StateInfo(int buttonId, const double* angleMap)
            : buttonId(buttonId),
              prevButtonState(false),
              stateChanged(false),
              angleMap(angleMap)
        { }
    };
    vector<StateInfo> states;
    int currentMap;

    SharedJoystickPtr joystick;
    int targetMode;

public:

    virtual bool initialize(SimpleControllerIO* io) override
    {
        this->io = io;
        ostream& os = io->os();
        Body* body = io->body();

        legActuationMode = Link::JointEffort;
        for(auto opt : io->options()) {
            if(opt == "position") {
                legActuationMode = Link::JointDisplacement;
                os << "The joint-position command mode is used." << endl;
            } else if(opt == "velocity") {
                legActuationMode = Link::JointVelocity;
                os << "The joint-velocity command mode is used." << endl;
            }
        }

        for(int i = 0; i < 12; ++i) {
            Link* joint = legJoint[i] = body->joint(i);
            if(!joint) {
                os << "Turret joint " << i << " is not found." << endl;
                return false;
            }
            qref[i] = qprev[i] = joint->q();
            joint->setActuationMode(legActuationMode);
            io->enableIO(joint);
        }

        dt = io->timeStep();

        states = {
            { Joystick::B_BUTTON, down },
            { Joystick::X_BUTTON,   up }
        };
        currentMap = 1;

        joystick = io->getOrCreateSharedObject<SharedJoystick>("joystick");
        targetMode = joystick->addMode();

        return true;
    }

    virtual bool control() override
    {
        joystick->updateState(targetMode);

        for(size_t i = 0; i < states.size(); ++i) {
            StateInfo info = states[i];
            bool stateChanged = false;
            bool buttonState = joystick->getButtonState(targetMode, info.buttonId);
            if(buttonState && !info.prevButtonState) {
                stateChanged = true;
            }
            info.prevButtonState = buttonState;
            if(stateChanged) {
                currentMap = i == 0 ? 0 : 1;
            }
        }

        static const double P = 150.0;
        static const double D = 15.0;

        for(int i = 0; i < 12; ++i) {
            Link* joint = legJoint[i];

            double qe = radian(states[currentMap].angleMap[i]);
            double q = joint->q();
            double dq = (q - qprev[i]) / dt;
            double dqref = 0.0;
            double deltaq = (qe - q) * dt;
            qref[i] += deltaq;
            dqref = deltaq / dt;

            if(legActuationMode == Link::JointDisplacement) {
                joint->q_target() = joint->q() + deltaq;
            } else if(legActuationMode == Link::JointVelocity) {
                joint->dq_target() = dqref;
            } else if(legActuationMode == Link::JointEffort) {
                joint->u() = P * (qref[i] - q) + D * (dqref - dq);
            }
            qprev[i] = q;
        }

        return true;
    }
};

CNOID_IMPLEMENT_SIMPLE_CONTROLLER_FACTORY(SpotLiedownController)