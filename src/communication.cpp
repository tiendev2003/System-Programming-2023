#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "../include/user.h"
#include "../include/message.h"
#include "../include/communication.h"

using namespace std;

Message APIRegister(const char* username, const char* password) {
  char user[32];
    char pass[32];
    strcpy(user, username);
    strcpy(pass, password);
    char data[64];
    memset(data, 0, sizeof(data));
    sprintf(data, "%s:%s", user, pass);
    Message message;
    
}