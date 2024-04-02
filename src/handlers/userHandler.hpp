#pragma once

#include "globalsForHandlers.hpp"

class UserHandler
{
public:
    // UserHandler(/* args */);
    // ~UserHandler();

    // create new user only for admin user
    static int CreateNewUser(mg_connection *conn, void *cbdata);
    // get info about session owner
    static int GetUserInfo(mg_connection *conn, void *cbdata);
    // update user wich is session owner
    static int UpdateCurrentUser(mg_connection *conn, void *cbdata);
    // delete only for admin user
    static int DeleteUserById(mg_connection *conn, void *cbdata);
};
