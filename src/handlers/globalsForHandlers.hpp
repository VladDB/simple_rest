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
    extern string PrepareAnswer(RESP_TYPES type, string body = nullptr);
    extern string TmToISO(tm timeTm);
    extern nlohmann::json UserModelToJson(UserModel user);
    extern nlohmann::json SessionModelToJson(SessionModel session);
    extern int GetUserIdFromUrl(string url);
}
