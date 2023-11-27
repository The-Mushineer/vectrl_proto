#include "thread_utils.h"

BooleanCondition::BooleanCondition()
    : m_value(false), m_condition(m_mutex)
{
}

void BooleanCondition::Signal()
{
    wxMutexLocker locker(m_mutex);
    m_value = true;
    m_condition.Signal();
}

void BooleanCondition::Reset()
{
    wxMutexLocker locker(m_mutex);
    m_value = false;
}

bool BooleanCondition::Get()
{
    wxMutexLocker locker(m_mutex);
    return m_value;
}

int BooleanCondition::Wait() {
    wxMutexLocker locker(m_mutex);
    if (!m_value) {
        return m_condition.Wait();
    }
    return 0;
}

int BooleanCondition::WaitTimeout(int timeout) {
    wxMutexLocker locker(m_mutex);
    if (!m_value) {
        return m_condition.WaitTimeout(timeout);
    }
    return 0;
}