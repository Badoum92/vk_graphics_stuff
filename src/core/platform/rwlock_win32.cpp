#include "core/rwlock.h"
#include "core/core.h"

#include <windows.h>

static_assert(sizeof(rwlock::handle) == sizeof(SRWLOCK));

rwlock rwlock_create()
{
    rwlock lock;
    InitializeSRWLock((PSRWLOCK)&lock.handle);
    lock.state = rwlock_state_unlocked;
    return lock;
}

void rwlock_read(rwlock* lock)
{
    ASSERT(lock->state == rwlock_state_unlocked);
    AcquireSRWLockShared((PSRWLOCK)&lock->handle);
    lock->state = rwlock_state_read;
}

void rwlock_write(rwlock* lock)
{
    ASSERT(lock->state == rwlock_state_unlocked);
    AcquireSRWLockExclusive((PSRWLOCK)&lock->handle);
    lock->state = rwlock_state_write;
}

bool rwlock_try_read(rwlock* lock)
{
    ASSERT(lock->state == rwlock_state_unlocked);
    if (TryAcquireSRWLockShared((PSRWLOCK)&lock->handle))
    {
        lock->state = rwlock_state_read;
        return true;
    }
    return false;
}

bool rwlock_try_write(rwlock* lock)
{
    ASSERT(lock->state == rwlock_state_unlocked);
    if (TryAcquireSRWLockExclusive((PSRWLOCK)&lock->handle))
    {
        lock->state = rwlock_state_write;
        return true;
    }
    return false;
}

void rwlock_unlock(rwlock* lock)
{
    ASSERT(lock->state != rwlock_state_unlocked);
    if (lock->state == rwlock_state_read)
        ReleaseSRWLockShared((PSRWLOCK)&lock->handle);
    else if (lock->state == rwlock_state_write)
        ReleaseSRWLockExclusive((PSRWLOCK)&lock->handle);
    lock->state = rwlock_state_unlocked;
}