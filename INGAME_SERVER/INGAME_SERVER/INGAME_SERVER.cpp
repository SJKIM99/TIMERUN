#include "pch.h"
#include "IngameMain.h"

int main()
{
    IngameMain ingameMain;

    if (!ingameMain.InitSocket()) {
        return -1;
    }
    ingameMain.ServeurRun();
}
