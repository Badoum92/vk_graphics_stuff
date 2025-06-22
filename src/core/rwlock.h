#pragma once

enum rwlock_state
{
    rwlock_state_unlocked,
    rwlock_state_read,
    rwlock_state_write,
};

struct rwlock
{
#if defined(_WIN32)
    void* handle;
#endif
    rwlock_state state;
};

rwlock rwlock_create();
void rwlock_read(rwlock* lock);
void rwlock_write(rwlock* lock);
bool rwlock_try_read(rwlock* lock);
bool rwlock_try_write(rwlock* lock);
void rwlock_unlock(rwlock* lock);