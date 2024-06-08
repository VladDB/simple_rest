#pragma once

#include <string>
#include <vector>

#include "civetweb.h"
#include <nlohmann/json.hpp>
#include "../components/log.hpp"
#include "../components/base64.h"

#include "../services/userService.hpp"
#include "../services/sessionService.hpp"

enum RESP_TYPES
{
    OK_200,
    Unauthorized_401
};

namespace GlobalsForHandlers
{
    extern std::string PrepareAnswer(RESP_TYPES type, const std::string &body = nullptr);
    extern std::string TmToISO(tm timeTm);
    extern nlohmann::json UserModelToJson(const UserModel &user);
    extern nlohmann::json SessionModelToJson(const SessionModel &session);
    extern int GetUserIdFromUrl(const std::string &url);
    extern std::string GetTokenFromHeader(const std::string &header);
}
