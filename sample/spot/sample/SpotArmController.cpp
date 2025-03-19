/**
    SpotArm Controller
    @author Kenta Suzuki
*/

#include <cnoid/SimpleController>
#include <cnoid/JointPath>
#include <cnoid/EigenUtil>
#include <cnoid/SharedJoystick>

using namespace std;
using namespace cnoid;

class SpotArmController : public SimpleController
{
    SimpleControllerIO* io;
    int jointActuationMode;
    Body* ioBody;
    Link* ioFinger;
    BodyPtr ikBody;
    Link* ikWrist;
    shared_ptr<JointPath> baseToWrist;
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
    
        // jointActuationMode = Link::JointEffort;
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

        ioFinger = ioBody->link("arm_gripper");

        ikBody = ioBody->clone();
        ikWrist = ikBody->link("arm_joint6");
        Link* base = ikBody->link("base_arm_joint");
        baseToWrist = JointPath::getCustomPath(base, ikWrist);
        base->p().setZero();
        base->R().setIdentity();
    
        const int nj = ioBody->numJoints();
        qold.resize(nj);
        for(int i=0; i < nj; ++i) {
            Link* joint = ioBody->joint(i);
            joint->setActuationMode(jointActuationMode);
            io->enableIO(joint);
            double q = joint->q();
            ikBody->joint(i)->q() = q;
            qold[i] = q;
        }
    
        baseToWrist->calcForwardKinematics();
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

        static const int axisID[] = {
            Joystick::L_STICK_H_AXIS, Joystick::R_STICK_V_AXIS, Joystick::L_STICK_V_AXIS,
            Joystick::DIRECTIONAL_PAD_V_AXIS, Joystick::R_STICK_H_AXIS, Joystick::DIRECTIONAL_PAD_H_AXIS
        };
    
        double pos[6];
        for(int i = 0; i < 6; ++i) {
            pos[i] = joystick->getPosition(targetMode, axisID[i]);
            if(fabs(pos[i]) < 0.2) {
                pos[i] = 0.0;
            }
        }
    
        VectorXd p(6);
        double v = 0.1;
        double w = 2.0;
    
        p.head<3>() = ikWrist->p() + ikWrist->R() * Vector3(-pos[0], -pos[1], pos[2]) * v * timeStep;
        p.tail<3>() = rpyFromRot(ikWrist->R() * rotFromRpy(Vector3(pos[3], pos[4], pos[5]) * w * timeStep));
        Isometry3 T;
        T.linear() = rotFromRpy(Vector3(p.tail<3>()));
        T.translation() = p.head<3>();
        if(baseToWrist->calcInverseKinematics(T)) {
            for(int i = 0; i < baseToWrist->numJoints(); ++i) {
                Link* joint = baseToWrist->joint(i);
                qref[joint->jointId()] = joint->q();
            }
        }

        bool buttonState[2];
        for(int i = 0; i < 2; ++i) {
            buttonState[i] = joystick->getButtonState(targetMode,
                i == 0 ? Joystick::A_BUTTON : Joystick::B_BUTTON);
        }
    
        double q = ioFinger->q();
        double q_upper = ioFinger->q_upper();
        double q_lower = ioFinger->q_lower();
        double deltaq = buttonState[0] ? 1.0 : 0.0;
        if(deltaq < 1.0) {
            deltaq = buttonState[1] ? -1.0 : 0.0;
        }
        qref[ioFinger->jointId()] += deltaq * timeStep;
        if(q > q_upper && deltaq > 0.0) {
            qref[ioFinger->jointId()] = q_upper;
        } else if(q < q_lower && deltaq < 0.0) {
            qref[ioFinger->jointId()] = q_lower;
        }

        static const double P = 0.0;
        static const double D = 0.0;

        for(int i = 0; i < ioBody->numJoints(); ++i) {
            Link* joint = ioBody->joint(i);

            double q = joint->q();
            double dq = (q - qold[i]) / timeStep;
            double dq_ref = 0.0;
            double deltaq = qref[i] - qref_old[i];
            dq_ref = deltaq / timeStep;

            if(jointActuationMode == Link::JointDisplacement) {
                joint->q_target() = joint->q() + deltaq;
            } else if(jointActuationMode == Link::JointVelocity) {
                joint->dq_target() = dq_ref;
            } else if(jointActuationMode == Link::JointEffort) {
                joint->u() = P * (qref[i] - q) + D * (dq_ref - dq);
            }
            qold[i] = q;
        }
        qref_old = qref;
        time += timeStep;
    
        return true;
    }
};

CNOID_IMPLEMENT_SIMPLE_CONTROLLER_FACTORY(SpotArmController)