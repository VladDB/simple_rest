#pragma once

#include <string>
#include <vector>

#include "civetweb.h"
#include <nlohmann/json.hpp>
#include "../components/base64.h"
#include "../components/log.hpp"

#include "../services/userService.hpp"
#include "../services/sessionService.hpp"

using namespace std;
using json = nlohmann::json;

class AuthHandler
{
public:
    AuthHandler();

    static int login(mg_connection *conn, void *cbdata);
    static int logout(mg_connection *conn, void *cbdata);
    static int ping(mg_connection *conn, void *cbdata);
};
