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
#include "LogPublisherThread.h"
#include "IDeviceDiagnostics.h"
#include "DeviceRuntimeMonitorThread.h"

#include "esp_heap_caps.h"
#include <cstdio>


/* @Component */
class IotApp final : public IIotApp {

    Public IotApp() {
    }

    Public Virtual ~IotApp() = default;

    Public Virtual Void Start() override {
        logger->Info(Tag::Untagged, StdString("[ArduinoSpringBootApp] Starting app..."));
        deviceDiagnostics->CheckAndLogPreviousCrash();
        AddStartupThreads();
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

    Private Void AddStartupThreads() {
        AddStartupThread<WiFiHealthCheckerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_4);
        AddStartupThread<InternetHealthCheckerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_3);
        AddStartupThread<TcpServerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_7);
        AddStartupThread<MqttClientThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_14);
        AddStartupThread<RequestManagerThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_7);
        AddStartupThread<DeviceManagerThread>(ThreadPoolCore::Application, ThreadPoolStackSize::KB_10);
        AddStartupThread<LogPublisherThread>(ThreadPoolCore::Application, ThreadPoolStackSize::KB_6);
        AddStartupThread<DeviceRuntimeMonitorThread>(ThreadPoolCore::System, ThreadPoolStackSize::KB_4);
    }

     /* @Autowired */
     Private IThreadPoolPtr threadPool;

     /* @Autowired */
     Private ILoggerPtr logger;

     /* @Autowired */
     Private IDeviceDiagnosticsPtr deviceDiagnostics;

     Private StdVector<IRunnablePtr> startupThreads;
     Private StdVector<ThreadPoolCore> startupThreadCores;
     Private StdVector<ThreadPoolStackSize> startupThreadStackSize;
};

#endif // IOTAPP_INTERNAL_H