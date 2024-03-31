#include "authHandler.hpp"

int AuthHandler::login(mg_connection *conn, void *cbdata)
{
    json answJson;
    string S = "";
    try
    {
        UserModel user;

        const char *header = mg_get_header(conn, "Authorization");
        if (header != NULL)
        {
            // get username and password
            const string forDecode = header;
            size_t pos = forDecode.find(" ");
            vector<BYTE> decodeData = base64_decode(forDecode.substr(pos + 1));
            string decode(decodeData.begin(), decodeData.end());
            pos = decode.find(":");
            user.username = decode.substr(0, pos);
            user.password = decode.substr(pos + 1);
        }

        const mg_request_info *info = mg_get_request_info(conn);
        // check user address
        if (info->remote_addr != NULL)
            user.ip_addr = info->remote_addr;
        else
            user.ip_addr = "Unknown";

        // get user ID, check if he exist
        user.id = UserService::GetUserId(user);
        if (user.id < 0)
            throw runtime_error("User does not exist");

        if (UserService::CheckUserPassword(user))
        {
            SessionModel session;
            session.userId = user.id;
            // create new session for user
            session.token = SessionsService::CreateSession(session, user.ip_addr);

            logger->info("New session by {}, token: {}", user.username, session.token);

            answJson["data"]["token"] = session.token;
            answJson["data"]["name"] = user.username;
            answJson["result"] = "OK";

            S = "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                "Cache-Control: no-cache\r\n"
                "Content-Length: " +
                to_string(answJson.dump().length()) +
                "\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n";

            S.append(answJson.dump());
        }
        else
        {
            logger->info("Incorrect username or password! {}, ACCESS DENIED!", user.username);

            answJson["result"] = "Error";
            answJson["data"]["info"] = "Incorrect password or username, ACCESS DENIED";

            S = "HTTP/1.1 401\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                "Cache-Control: no-cache\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n";

            S.append(answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::login: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Cache-Control: no-cache\r\n"
            "Content-Length: " +
            to_string(answJson.dump().length()) +
            "\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n"
            "\r\n";

        S.append(answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}

int AuthHandler::logout(mg_connection *conn, void *cbdata)
{
    json answJson;
    string S = "";
    try
    {
        UserModel user;

        string inToken = "";

        const char *header = mg_get_header(conn, "Pragma");
        if (header != NULL)
        {
            const string Pragma = header;
            size_t pos = Pragma.find_first_of("=");
            if (pos != string::npos)
                inToken = Pragma.substr(pos + 1);
        }

        if (inToken.empty())
            throw std::runtime_error("Empty Pragma (token)");

        // get user ID, check if he exist
        user = UserService::GetUserByToken(inToken);
        if (user.id < 0)
            throw runtime_error("User does not exist");

        // check user address
        const mg_request_info *info = mg_get_request_info(conn);
        if (info->remote_addr != NULL)
            user.ip_addr = info->remote_addr;

        if (SessionsService::CheckSession(inToken, user.ip_addr))
        {
            SessionsService::DeleteSession(inToken);

            logger->info("The session was closed by {}, token: {}", user.username, inToken);

            answJson["data"]["info"] = "The session was closed";
            answJson["result"] = "OK";

            S = "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                "Cache-Control: no-cache\r\n"
                "Content-Length: " +
                to_string(answJson.dump().length()) +
                "\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n";

            S.append(answJson.dump());
        }
        else
        {
            logger->info("Incorrect username or password! {}, ACCESS DENIED!", user.username);

            answJson["result"] = "Error";
            answJson["data"]["info"] = "Bad password or username";

            S = "HTTP/1.1 401\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                "Cache-Control: no-cache\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n";

            S.append(answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::logout: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Cache-Control: no-cache\r\n"
            "Content-Length: " +
            to_string(answJson.dump().length()) +
            "\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n"
            "\r\n";

        S.append(answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}

int AuthHandler::ping(mg_connection *conn, void *cbdata)
{
    json answJson;
    string S = "";
    try
    {
        UserModel user;

        string inToken = "";

        const char *header = mg_get_header(conn, "Pragma");
        if (header != NULL)
        {
            const string Pragma = header;
            size_t pos = Pragma.find_first_of("=");
            if (pos != string::npos)
                inToken = Pragma.substr(pos + 1);
        }

        if (!inToken.empty())
        {
            // get user ID, check if he exist
            user = UserService::GetUserByToken(inToken);
            if (user.id < 0)
                throw runtime_error("User does not exist");

            // check user address
            const mg_request_info *info = mg_get_request_info(conn);
            if (info->remote_addr != NULL)
                user.ip_addr = info->remote_addr;
        }

        if (SessionsService::CheckSession(inToken, user.ip_addr))
        {
            // get current time
            time_t now = time(0);
            tm nowTm = *localtime(&now);
            char nowStr[31];
            strftime(nowStr, 30, "%Y-%m-%dT%H:%M:%S.000", &nowTm);
            
            answJson["data"]["info"] = "OK";
            answJson["data"]["time"] = nowStr;
            answJson["result"] = "OK";

            S = "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                "Cache-Control: no-cache\r\n"
                "Content-Length: " +
                to_string(answJson.dump().length()) +
                "\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n";

            S.append(answJson.dump());
        }
        else
        {
            answJson["result"] = "Error";
            answJson["data"]["info"] = "Unauthorized";

            S = "HTTP/1.1 200\r\n"
                "Content-Type: application/json; charset=UTF-8\r\n"
                "Cache-Control: no-cache\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n"
                "\r\n";

            S.append(answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::ping: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Cache-Control: no-cache\r\n"
            "Content-Length: " +
            to_string(answJson.dump().length()) +
            "\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n"
            "\r\n";

        S.append(answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}