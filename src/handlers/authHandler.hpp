#pragma once

#ifdef __linux__
#include <uuid/uuid.h>
#else
#include <windows.h>
#endif

#include <string>
#include <vector>

#include "civetweb.h"
#include <nlohmann/json.hpp>
#include "../components/base64.h"
#include "../components/log.hpp"

using namespace std;
using json = nlohmann::json;

struct RestSession
{
    string userName = "";
    string psw = "";
    string token = "";
    string remoteAdr = "";
    time_t tokenCreated;
    time_t timeOfLogin;
};

extern vector<RestSession> RestSessions;

class AuthHandler
{
public:
    AuthHandler(/* args */);
    ~AuthHandler();

    static int login(mg_connection *conn, void *cbdata);
};
