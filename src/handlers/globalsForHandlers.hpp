#pragma once

#include <string>
#include <vector>

#include "civetweb.h"
#include <nlohmann/json.hpp>
#include "../components/log.hpp"
#include "../components/base64.h"

#include "../services/userService.hpp"
#include "../services/sessionService.hpp"

using namespace std;

enum RESP_TYPES
{
    OK_200,
    Unauthorized_401
};

namespace GlobalsForHandlers
{
    string PrepareAnswer(RESP_TYPES type, string body = nullptr);

    // extern std::string UserModelToJson(UserModel user);
    // extern std::string SessionModelToJson(SessionModel session);
}
