#pragma once

#include <string>
#include "globalsForHandlers.hpp"

using namespace std;
using json = nlohmann::json;

class AuthHandler
{
public:
    static int login(mg_connection *conn, void *cbdata);
    static int logout(mg_connection *conn, void *cbdata);
    static int ping(mg_connection *conn, void *cbdata);
};
