#pragma once

#include "globalsForHandlers.hpp"

using namespace std;

class HtmlHandler
{
private:
    static string PrepareHtmlPage(string body, string pageTitle, bool startLink = false);
public:
    static int AllUsersPage(mg_connection *conn, void *cbdata);
    static int UserSessionsPage(mg_connection *conn, void *cbdata);
};
