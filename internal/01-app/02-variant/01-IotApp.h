#ifndef IOTAPP_INTERNAL_H
#define IOTAPP_INTERNAL_H

#include <StandardDefines.h>
#include "../01-interface/01-IIotApp.h"

#include "Thread.h"

#include "logger/ILogger.h"
#include "threading/IThreadPool.h"

#include "WiFiHealthCheckerThread.h"
#include "InternetHealthCheckerThread.h"
#include "TcpServerThread.h"
#include "MqttClientThread.h"
#include "RequestManagerThread.h"
#include "DeviceManagerThread.h"


/* @Component */
class IotApp final : public IIotApp {

    Public IotApp() {
        AddStartupThread<WiFiHealthCheckerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_4);
        AddStartupThread<InternetHealthCheckerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_4);
        AddStartupThread<TcpServerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_8);
        AddStartupThread<MqttClientThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_16);
        AddStartupThread<RequestManagerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_8);
        AddStartupThread<DeviceManagerThread>(ThreadPoolCore::Application, ThreadPoolStackSize::KB_12);
    }

    Public Virtual ~IotApp() = default;

    Public Virtual Void Start() override {
        logger->Info(Tag::Untagged, StdString("[ArduinoSpringBootApp] Starting app..."));
        /*if (deviceDiagnostics->HadPreviousCrash()) {
            logger->Info(Tag::Untagged, StdString("[ArduinoSpringBootApp] Previous run: crashed (core dump/panic)."));
        } else {
            logger->Info(Tag::Untagged, StdString("[ArduinoSpringBootApp] Previous run: normal."));
        }*/
        for (Size i = 0; i < startupThreads.size(); ++i) {
            if (startupThreads[i]) {
                threadPool->Execute(startupThreads[i], startupThreadCores[i], startupThreadStackSize[i]);
            }
        }
    }

    Public Virtual Void Stop() override {
    }

    Public Virtual Void Restart() override {
        Stop();
        Start();
    }

    Protected Void AddStartupThreadImpl(IRunnablePtr thread, ThreadPoolCore core, ThreadPoolStackSize stackSize) override {
        if (thread) {
            startupThreads.push_back(thread);
            startupThreadCores.push_back(core);
            startupThreadStackSize.push_back(stackSize);
        }
    }

     /* @Autowired */
     Private IThreadPoolPtr threadPool;
 
     /* @Autowired */
     Private ILoggerPtr logger;

     Private StdVector<IRunnablePtr> startupThreads;
     Private StdVector<ThreadPoolCore> startupThreadCores;
     Private StdVector<ThreadPoolStackSize> startupThreadStackSize;
};

#endif // IOTAPP_INTERNAL_H