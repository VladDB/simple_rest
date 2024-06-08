#pragma once

#include "globalsForHandlers.hpp"

class HtmlHandler
{
private:
    static std::string PrepareHtmlPage(const std::string &body, const std::string &pageTitle, bool startLink = false);
public:
    static int AllUsersPage(mg_connection *conn, void *cbdata);
    static int UserSessionsPage(mg_connection *conn, void *cbdata);
};
