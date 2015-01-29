#include "BCI/bciService.h"
#include "BCI/BCIStateMachine.h"
#include "debug.h"

#include <QKeyEvent>
#include <QMutex>

BCIService *BCIService::bciServiceInstance = NULL;
QMutex BCIService::createLock;

BCIService *BCIService::getInstance() {
    if (createLock.locked()) {
        assert(0);
        return NULL;
    }
    QMutexLocker lock(&createLock);
    if (!bciServiceInstance) {
        DBGA("Building a BCIService")
        bciServiceInstance = new BCIService();
        QApplication::instance()->installEventFilter(bciServiceInstance);
    }

    return bciServiceInstance;
}

BCIService::BCIService() {
    DBGA("Built BCIService");
//rosServer = NULL;
    rosServer = new RosRPCZClient();
}

void BCIService::init(BCIControlWindow *bciControlWindow) {
    //builds and starts a qtStateMachine
    BCIStateMachine *bciStateMachine = new BCIStateMachine(bciControlWindow, this);
    connect(this, SIGNAL(plannerUpdated()), bciControlWindow, SLOT(redraw()));
    connect(OnlinePlannerController::getInstance(), SIGNAL(render()), bciControlWindow, SLOT(redraw()));
    bciStateMachine->start();
}

bool BCIService::eventFilter(QObject *obj, QEvent *evt) {
    if (evt->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
        if (keyEvent->key() == Qt::Key::Key_Z)
            runObjectRecognition(NULL, NULL);

        if (keyEvent->key() == Qt::Key::Key_N) {
            if (OnlinePlannerController::getInstance() && world_element_tools::getWorld()->getCurrentPlanner()) {
                OnlinePlannerController::getInstance()->incrementGraspIndex();
                OnlinePlannerController::getInstance()->getCurrentGrasp()->execute(
                        OnlinePlannerController::getInstance()->getGraspDemoHand());
                DBGA("set next graspfrom key press");
                return true;
            }
        }
        if (keyEvent->key() == Qt::Key::Key_C) {
            if (OnlinePlannerController::getInstance()) {
                if (OnlinePlannerController::getInstance()->getCurrentGrasp()) {
                    DBGA("emitted check grasp from key press");
                    checkGraspReachability(OnlinePlannerController::getInstance()->getCurrentGrasp(), NULL, NULL);
                }
                else {
                    DBGA("couldn't find grasp to check");
                }
            }

        }
        if (keyEvent->key() == Qt::Key::Key_T) {
            if (OnlinePlannerController::getInstance()) {
                OnlinePlannerController::getInstance()->toggleTimedUpdate();
                DBGA("Planner timed update running: " << OnlinePlannerController::getInstance()->timedUpdateRunning);
            }
        }
        if (keyEvent->key() == Qt::Key::Key_J) {
            std::vector<float> interestLevel;
            interestLevel.push_back(.5);
            emitOptionChoice(1, .5, interestLevel);
            DBGA("BCIService");
        }
        if (keyEvent->modifiers().testFlag(Qt::KeyboardModifier::AltModifier)) {
            bool isNumOk;
            unsigned int choice = keyEvent->text().toInt(&isNumOk);
            if (isNumOk) {
                //Output Option Choice
                std::vector<float> interest_level;
                emit optionChoice(choice, 1.0, interest_level);
                std::cout << "Option Choice:" << choice << std::endl;
            }

        }
        return true;
    }
    else {
        return QObject::eventFilter(obj, evt);
    }
}

bool BCIService::runObjectRetreival(QObject *callbackReceiver,
        const char *slot) {
    if (!rosServer) {
        DBGA("invalid ros server");
        return false;
    }
    return rosServer->runObjectRetrieval(callbackReceiver, slot);
}

bool BCIService::runObjectRecognition(QObject *callbackReceiver,
        const char *slot) {
    if (!rosServer) {
        DBGA("invalid ros server");
        return false;
    }
    return rosServer->runObjectRecognition(callbackReceiver, slot);
}

bool BCIService::getCameraOrigin(QObject *callbackReceiver, const char *slot) {
    if (!rosServer) {
        DBGA("invalid ros server");
        return false;
    }
    return rosServer->getCameraOrigin(callbackReceiver, slot);
}


bool BCIService::checkGraspReachability(const GraspPlanningState *state,
        QObject *callbackReceiver,
        const char *slot) {
    if (!rosServer) {
        DBGA("invalid ros server");
        return false;
    }
    return rosServer->checkGraspReachability(state, callbackReceiver, slot);
}

bool BCIService::executeGrasp(const GraspPlanningState *gps,
        QObject *callbackReceiver,
        const char *slot) {
    if (!rosServer) {
        DBGA("invalid ros server");
        return false;
    }
    return rosServer->executeGrasp(gps, callbackReceiver, slot);
}

bool BCIService::sendOptionChoices(std::vector<QImage *> &images,
        std::vector<QString> &optionDescriptions, std::vector<float> &imageCosts,
        float minimumConfidence) {
    if (!rosServer) {
        DBGA("invalid ros server");
        return false;
    }
    //return rosServer->sendOptionChoices(images, optionDescriptions, imageCosts, minimumConfidence);
}

void BCIService::emitOptionChoice(unsigned int option, float confidence,
        std::vector<float> &interestLevel) {
    emit optionChoice(option, confidence, interestLevel);
}
