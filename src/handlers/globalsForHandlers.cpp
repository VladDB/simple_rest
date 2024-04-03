#include "globalsForHandlers.hpp"

const string BASE_HEADERS = "Cache-Control: no-cache\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Connection: close\r\n";

const string BASE_HEADERS_END = "\r\n";

namespace GlobalsForHandlers
{
    using json = nlohmann::json;

    string PrepareAnswer(RESP_TYPES type, string body)
    {
        string answer = "HTTP/1.1 ";
        switch (type)
        {
        case RESP_TYPES::OK_200:
            answer.append("200 OK\r\n");
            break;
        case RESP_TYPES::Unauthorized_401:
            answer.append("401 Unauthorized\r\n");
            break;
        default:
            answer.append("404\r\n");
            break;
        }

        answer.append(BASE_HEADERS);
        if (!body.empty())
        {
            answer.append("Content-Type: application/json; charset=UTF-8\r\n");
            answer.append("Content-Length: ").append(to_string(body.length())).append("\r\n");
            answer.append(BASE_HEADERS_END);
            answer.append(body);
        }
        else
            answer.append(BASE_HEADERS_END);

        return answer;
    }

    string TmToISO(tm timeTm)
    {
        char nowStr[31];
        strftime(nowStr, 30, "%Y-%m-%dT%H:%M:%S.000", &timeTm);
        return string(nowStr);
    }

    nlohmann::json UserModelToJson(UserModel user)
    {
        json jsonModel;
        
        jsonModel["id"] = user.id;
        jsonModel["username"] = user.username;
        jsonModel["ip_addr"] = user.ip_addr;
        jsonModel["is_admin"] = user.is_admin;
        jsonModel["create_at"] = TmToISO(user.create_at);
        
        return jsonModel;
    }

    nlohmann::json SessionModelToJson(SessionModel session)
    {
        json jsonModel;
        
        jsonModel["id"] = session.id;
        jsonModel["user_id"] = session.userId;
        jsonModel["token"] = session.token;
        jsonModel["create_at"] = TmToISO(session.createAt);
        jsonModel["last_connect"] = TmToISO(session.lastConnect);
        
        return jsonModel;
    }

} // namespace GlobalsForHandlers
