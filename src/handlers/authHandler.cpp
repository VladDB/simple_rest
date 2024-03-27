#include "authHandler.hpp"

vector<RestSession> RestSessions;

int AuthHandler::login(mg_connection *conn, void *cbdata)
{
    json answJson;
    string S = "";
    try
    {
        RestSession RSession;
        string inUser = "";
        string inPass = "";

        const char *header = mg_get_header(conn, "Authorization");
        if (header != NULL)
        {
            // get username and password
            const string forDecode = header;
            size_t pos = forDecode.find(" ");
            vector<BYTE> decodeData = base64_decode(forDecode.substr(pos + 1));
            string decode(decodeData.begin(), decodeData.end());
            pos = decode.find(":");
            inUser = decode.substr(0, pos);
            inPass = decode.substr(pos + 1);
        }

        const mg_request_info *info = mg_get_request_info(conn);
        // check user address
        if (info->remote_addr != NULL)
            RSession.remoteAdr = info->remote_addr;
        else
            RSession.remoteAdr = "Unknown";

        // if (!inUser.empty() && checkUsernameAndPassword(inUser, inPass, RSession))
        if (true)
        {
#ifdef __linux__
            uuid_t uu;
            uuid_generate_random(uu);
            char *genUuid = (char *)malloc(37);
            uuid_unparse(uu, genUuid);
            RSession.token = genUuid;
            if (genUuid)
                free(genUuid);
#else
            UUID uuid;
            UuidCreate(&uuid);
            unsigned char *str;
            UuidToStringA(&uuid, &str);
            RSession.token = (char *)str;
            RpcStringFreeA(&str);
#endif // __linux__

            time(&RSession.tokenCreated);

            RSession.timeOfLogin = RSession.tokenCreated;

            RestSessions.push_back(RSession);
            logger->info("New session by {}, token: {}", RSession.userName, RSession.token);

            answJson["token"] = RSession.token;
            answJson["name"] = RSession.userName;
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
            logger->info("Incorrect username or password! {}, ACCESS DENIED!", inUser);

            answJson["result"] = "Bad password or username";

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
        logger->error("AuthHandler::RestAuthLogin: {}", e.what());

        string err = "Server error: ";
        err.append(e.what());
        answJson["result"] = err;

        S = "HTTP/1.1 500\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Content-Length: " +
            to_string(answJson.dump().length()) +
            "\r\n"
            "Cache-Control: no-cache\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n"
            "\r\n";
    }

    mg_printf(conn, S.c_str());
    return 1;
}
