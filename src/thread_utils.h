#ifndef __VECTRL_THREAD_UTILS_H__
#define __VECTRL_THREAD_UTILS_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/thread.h"
#endif

#if !wxUSE_THREADS
    #error "This program requires thread support!"
#endif // wxUSE_THREADS

class BooleanCondition {
public:
    BooleanCondition();

    void Signal();
    void Reset();
    bool Get();

    int Wait();
    int WaitTimeout(int timeout);

    operator bool() { return Get(); }
    bool operator!() { return !Get(); }
private:
    wxMutex m_mutex;
    wxCondition m_condition;
    bool m_value;
};

#endif