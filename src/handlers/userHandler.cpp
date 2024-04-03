#include "userHandler.hpp"

int GetUserIdFromUrl(string url)
{
    int id = -1;
    try
    {
        size_t pos = url.find_last_of('/');
        if (pos != string::npos)
            id = stoi(url.substr(pos + 1));
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserHandler::GetUserInfo: {}", e.what());
    }
    return id;
}

int UserHandler::CreateNewUser(mg_connection *conn, void *cbdata)
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
            throw runtime_error("Empty token");

        // get user ID, check if he exist
        user = UserService::GetUserByToken(inToken);
        if (user.id < 0)
            throw runtime_error("User does not exist");

        // check user address
        const mg_request_info *info = mg_get_request_info(conn);
        if (info->remote_addr != NULL)
            user.ip_addr = info->remote_addr;

        // get request body
        int buffSize = 1024;
        header = mg_get_header(conn, "Content-Length");
        if (header != NULL)
            buffSize = atoi(header);

        char *buf = new char[buffSize];
        json reqJson;
        string res;
        for (;;)
        {
            int ret = mg_read(conn, buf, buffSize);
            if (ret <= 0)
                break;
            res.assign(buf, buffSize);
        }

        delete buf;
        reqJson = json::parse(res);

        UserModel newUser;
        if (reqJson["username"].is_null() && reqJson["password"].is_null())
            throw runtime_error("Empty username or password");

        newUser.username = reqJson["username"].get<string>();
        newUser.password = reqJson["password"].get<string>();
        newUser.is_admin = reqJson["is_admin"].get<bool>();

        if (user.is_admin && SessionsService::CheckSession(inToken, user.ip_addr))
        {
            // update user
            answJson["result"] = "OK";
            answJson["data"]["info"] = UserService::CreateUser(newUser);

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
        else
        {
            answJson["result"] = "Error";
            answJson["data"]["info"] = "Unauthorized";

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::CreateNewUser: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}

int UserHandler::GetUserInfo(mg_connection *conn, void *cbdata)
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
            throw runtime_error("Empty token");

        // get user ID, check if he exist
        user = UserService::GetUserByToken(inToken);
        if (user.id < 0)
            throw runtime_error("User does not exist");

        // check user address
        const mg_request_info *info = mg_get_request_info(conn);
        if (info->remote_addr != NULL)
            user.ip_addr = info->remote_addr;

        // get user id from url
        string uri = mg_get_request_info(conn)->request_uri;
        int userId = GetUserIdFromUrl(uri);
        if (userId == -1)
            throw runtime_error("Incorrect user id");

        if (SessionsService::CheckSession(inToken, user.ip_addr))
        {
            // get user info
            UserModel userInfo = UserService::GetUserById(userId);
            answJson = GlobalsForHandlers::UserModelToJson(userInfo);

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
        else
        {
            answJson["result"] = "Error";
            answJson["data"]["info"] = "Unauthorized";

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::ping: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}

int UserHandler::UpdateCurrentUser(mg_connection *conn, void *cbdata)
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
            throw runtime_error("Empty token");

        // get user ID, check if he exist
        user = UserService::GetUserByToken(inToken);
        if (user.id < 0)
            throw runtime_error("User does not exist");

        // check user address
        const mg_request_info *info = mg_get_request_info(conn);
        if (info->remote_addr != NULL)
            user.ip_addr = info->remote_addr;

        // get request body
        int buffSize = 1024;
        header = mg_get_header(conn, "Content-Length");
        if (header != NULL)
            buffSize = atoi(header);

        char *buf = new char[buffSize];
        json reqJson;
        string res;
        for (;;)
        {
            int ret = mg_read(conn, buf, buffSize);
            if (ret <= 0)
                break;
            res.assign(buf, buffSize);
        }

        delete buf;
        reqJson = json::parse(res);

        UserModel updatedUser;
        updatedUser.id = user.id;
        updatedUser.username = reqJson["username"].is_null() ? user.username : reqJson["username"].get<string>();
        updatedUser.password = reqJson["password"].is_null() ? user.password : reqJson["password"].get<string>();
        updatedUser.is_admin = reqJson["is_admin"].is_null() ? user.is_admin : reqJson["is_admin"].get<bool>();

        if (SessionsService::CheckSession(inToken, user.ip_addr))
        {
            // update user
            UserService::UpdateUser(updatedUser);
            answJson["result"] = "OK";
            answJson["data"]["info"] = "User was updated";

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
        else
        {
            answJson["result"] = "Error";
            answJson["data"]["info"] = "Unauthorized";

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::UpdateCurrentUser: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}

int UserHandler::DeleteUserById(mg_connection *conn, void *cbdata)
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
            throw runtime_error("Empty token");

        // get user ID, check if he exist
        user = UserService::GetUserByToken(inToken);
        if (user.id < 0)
            throw runtime_error("User does not exist");

        // check user address
        const mg_request_info *info = mg_get_request_info(conn);
        if (info->remote_addr != NULL)
            user.ip_addr = info->remote_addr;

        // get user id from url
        string uri = mg_get_request_info(conn)->request_uri;
        int userId = GetUserIdFromUrl(uri);
        if (userId == -1)
            throw runtime_error("Incorrect user id");

        if (user.is_admin && SessionsService::CheckSession(inToken, user.ip_addr))
        {
            // update user
            UserService::DeleteUser(userId);
            answJson["result"] = "OK";
            answJson["data"]["info"] = "User was deleted";

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
        else
        {
            answJson["result"] = "Error";
            answJson["data"]["info"] = "Unauthorized";

            S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
        }
    }
    catch (exception &e)
    {
        logger->error("AuthHandler::UpdateCurrentUser: {}", e.what());

        answJson["result"] = "Error";
        answJson["data"]["info"] = e.what();

        S = GlobalsForHandlers::PrepareAnswer(RESP_TYPES::OK_200, answJson.dump());
    }

    mg_printf(conn, S.c_str());
    return 1;
}
