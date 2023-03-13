#pragma once

#include <cstdio>

struct St
{
    St(int val_)
        : val{val_}
    {
        printf("ctor\n");
    }

    ~St()
    {
        printf("dtor\n");
        val = -1;
    }

    St(const St& other)
    {
        printf("copy\n");
        val = other.val;
    }

    St(St&& other)
    {
        printf("move\n");
        val = other.val;
        other.val = 0xdeadbeef;
    }

    St& operator=(const St& other)
    {
        printf("assign\n");
        val = other.val;
        return *this;
    }

    St& operator=(St&& other)
    {
        printf("move assign\n");
        val = other.val;
        other.val = -2;
        return *this;
    }

    int val = 0;
};
