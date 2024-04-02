#include "globalsForHandlers.hpp"

const string BASE_HEADERS = "Cache-Control: no-cache\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "Connection: close\r\n";

const string BASE_HEADERS_END = "\r\n";

namespace GlobalsForHandlers
{
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
} // namespace GlobalsForHandlers
