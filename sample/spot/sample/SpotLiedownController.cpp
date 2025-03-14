/**
    Spot Liedown Controller
    @author Kenta Suzuki
*/

#include <cnoid/EigenUtil>
#include <cnoid/SimpleController>
#include <cnoid/SharedJoystick>

using namespace std;
using namespace cnoid;

const double down[] = {
    20.0, 95.0, -143.0, -20.0, 95.0, -143.0,
    20.0, 95.0, -143.0, -20.0, 95.0, -143.0
};

const double up[] = {
    25.0, 60.0, -95.0, -25.0, 60.0, -95.0,
    25.0, 60.0, -95.0, -25.0, 60.0, -95.0
};

class SpotLiedownController : public SimpleController
{
    SimpleControllerIO* io;
    int jointActuationMode;
    Body* ioBody;
    VectorXd qref, qold, qref_old;
    double time;
    double timeStep;

    SharedJoystickPtr joystick;
    int targetMode;

public:

    virtual bool initialize(SimpleControllerIO* io) override
    {
        this->io = io;
        ostream& os = io->os();
        ioBody = io->body();

        jointActuationMode = Link::JointVelocity;
        for(auto opt : io->options()) {
            if(opt == "position") {
                jointActuationMode = Link::JointDisplacement;
                os << "The joint-position command mode is used." << endl;
            } else if(opt == "velocity") {
                jointActuationMode = Link::JointVelocity;
                os << "The joint-velocity command mode is used." << endl;
            }
        }

        const int nj = ioBody->numJoints();
        qold.resize(nj);
        for(int i = 0; i < nj; ++i) {
            Link* joint = ioBody->joint(i);
            joint->setActuationMode(jointActuationMode);
            io->enableIO(joint);
            double q = joint->q();
            qold[i] = q;
        }

        qref = qold;
        qref_old = qold;

        time = 0.0;
        timeStep = io->timeStep();

        joystick = io->getOrCreateSharedObject<SharedJoystick>("joystick");
        targetMode = joystick->addMode();

        return true;
    }

    virtual bool control() override
    {
        joystick->updateState(targetMode);

        double pos[2];
        for(int i = 0; i < 2; ++i) {
            pos[i] = joystick->getPosition(targetMode,
                i == 0 ? Joystick::L_TRIGGER_AXIS : Joystick::R_TRIGGER_AXIS);
            if(fabs(pos[i]) < 0.2) {
                pos[i] = 0.0;
            }
        }

        for(int i = 0; i < ioBody->numJoints(); ++i) {
            double q = ioBody->joint(i)->q();
            double q_target = pos[1] > 0 ? radian(down[i]) : radian(up[i]);
            double deltaq = q_target - q;
            qref[i] += deltaq * timeStep;
        }

        for(int i = 0; i < ioBody->numJoints(); ++i) {
            Link* joint = ioBody->joint(i);
            if(jointActuationMode == Link::JointDisplacement) {
                joint->q_target() = qref[i];
            } else if(jointActuationMode == Link::JointVelocity) {
                double q = joint->q();
                double dq = (q - qold[i]) / timeStep;
                double dq_ref = (qref[i] - qref_old[i]) / timeStep;
                joint->dq_target() = dq_ref;
                qold[i] = q;
            }
        }
        qref_old = qref;
        time += timeStep;

        return true;
    }
};

CNOID_IMPLEMENT_SIMPLE_CONTROLLER_FACTORY(SpotLiedownController)