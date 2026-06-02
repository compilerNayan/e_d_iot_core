#ifndef IIOTAPP_INTERNAL_H
#define IIOTAPP_INTERNAL_H

#include <StandardDefines.h>
#include "threading/IRunnable.h"
#include <threading/ThreadPoolCore.h>
#include <threading/ThreadPoolStackSize.h>
#include <memory>
#include <type_traits>

DefineStandardPointers(IIotApp)
class IIotApp {

    Public Virtual ~IIotApp() = default;

    Public Virtual Void Start() = 0;
    Public Virtual Void Stop() = 0;
    Public Virtual Void Restart() = 0;

    /** Register a thread (by type T) to be started when the app starts (Start()). Creates T via default constructor. */
    Public
    template<typename T>
    Void AddStartupThread(ThreadPoolCore core = ThreadPoolCore::System,
                          ThreadPoolStackSize stackSize = ThreadPoolStackSize::KB_8) {
        static_assert(std::is_base_of_v<IRunnable, T>, "T must derive from IRunnable");
        AddStartupThreadImpl(std::make_shared<T>(), core, stackSize);
    }

    /** Implement in variant: register the thread for startup on the specified core with the requested stack size. */
    Protected Virtual Void AddStartupThreadImpl(IRunnablePtr thread,
                                                ThreadPoolCore core,
                                                ThreadPoolStackSize stackSize) = 0;
};

#endif // IIOTAPP_INTERNAL_H 