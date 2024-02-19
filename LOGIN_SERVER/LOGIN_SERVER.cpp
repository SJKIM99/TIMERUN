#include "pch.h"
#include "LoginMain.h"

int main()
{
	LoginMain loginMain;

	if (!loginMain.InitSocket()) {
		return -1;
	}
	loginMain.ServerRun();
}

