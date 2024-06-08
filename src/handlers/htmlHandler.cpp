#include "htmlHandler.hpp"

using namespace std;

string HtmlHandler::PrepareHtmlPage(const string &body, const string &pageTitle, bool startLink)
{
    std::string S = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=windows-1251\r\n"
                    "Cache-Control: no-cache\r\n"
                    "Connection: close\r\n"
                    "\r\n";

    S += "<html>";
    // Page header
    S += "<head>";
    S += "<title>Simple Rest</title>";
    S += "</head>";
    // page body
    S += "<body>";
    if (startLink)
        S += (std::string) "<a href=\"/html/users\">Back to start page</a>";
    S += "<h2>" + pageTitle + "</h2>";
    S += "Server build date: " + (std::string)__DATE__;
    time_t now = time(0);
    tm nowTm = *localtime(&now);
    std::string currentTime = GlobalsForHandlers::TmToISO(nowTm);
    S += "<br>Server date-time: " + currentTime;
    S += "<br>";

    // add body
    S += body;

    S += "</body>";
    S += "</html>";
    return S;
}

int HtmlHandler::AllUsersPage(mg_connection *conn, void *cbdata)
{
    string table{""};
    vector<UserModel> users = UserService::GetAllUsers();
    if (users.empty())
        table = "No users in DB";
    else
    {
        // шапка таблицы со списком cтанций
        table += "<table cols=\"23\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" width=\"100%%\">";
        table += "<thead>";
        table += "<tr>";
        table += "<th rowspan=\"2\">ID</th>";
        table += "<th rowspan=\"2\">Username</th>";
        table += "<th rowspan=\"2\">IP address</th>";
        table += "<th rowspan=\"2\">Is admin</th>";
        table += "<th rowspan=\"2\">Create time</th>";
        table += "<th rowspan=\"2\">To stations</th>";
        table += "</tr>";
        table += "</thead>";
        table += "<tbody>";

        // Содержимое таблицы со списком станций
        for (UserModel user : users)
        {
            table += "<tr align=\"center\">";
            // ID
            table += "<td>" + std::to_string(user.id) + "</td>";
            // username
            table += "<td>" + user.username + "</td>";
            // IP
            table += "<td>" + user.ip_addr + "</td>";
            // is admin
            string admin = user.is_admin ? "true" : "false";
            table += "<td>" + admin + "</td>";
            // create time
            table += "<td>" + GlobalsForHandlers::TmToISO(user.create_at) + "</td>";
            // link to sessions
            table += "<td><a href=\"/html/sessions/" + std::to_string(user.id) + "\">--></a></td>";
            table += "</tr>";
        }
        table += "</tbody>";
        table += "</table>";
    }
    
    mg_printf(conn, PrepareHtmlPage(table, "USERS", false).c_str());
    return 1;
}

int HtmlHandler::UserSessionsPage(mg_connection *conn, void *cbdata)
{
    string uri = mg_get_request_info(conn)->request_uri;
    int userId = GlobalsForHandlers::GetUserIdFromUrl(uri);
    string table{""};
    vector<SessionModel> sessions = SessionsService::GetAllUserSessions(userId);
    if (sessions.empty())
        table = "<h2>User does not have sessions</h2>";
    else
    {
        // шапка таблицы со списком cтанций
        table += "<table cols=\"23\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\" width=\"100%%\">";
        table += "<thead>";
        table += "<tr>";
        table += "<th rowspan=\"2\">ID</th>";
        table += "<th rowspan=\"2\">token</th>";
        table += "<th rowspan=\"2\">Last connect time</th>";
        table += "<th rowspan=\"2\">Create time</th>";
        table += "</tr>";
        table += "</thead>";
        table += "<tbody>";

        // Содержимое таблицы со списком станций
        for (SessionModel session : sessions)
        {
            table += "<tr align=\"center\">";
            // ID
            table += "<td>" + std::to_string(session.id) + "</td>";
            // token
            table += "<td>" + session.token + "</td>";
            // last connect time
            table += "<td>" + GlobalsForHandlers::TmToISO(session.lastConnect) + "</td>";
            // create time
            table += "<td>" + GlobalsForHandlers::TmToISO(session.createAt) + "</td>";
            table += "</tr>";
        }
        table += "</tbody>";
        table += "</table>";
    }
    
    string title = "Sessions of user (id = " + to_string(userId) + ")";
    
    mg_printf(conn, PrepareHtmlPage(table, title, true).c_str());
    return 1;
}
