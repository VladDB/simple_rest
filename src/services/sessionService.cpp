#include "sessionService.hpp"

std::string SessionsService::CreateSession(SessionModel session, std::string ip)
{
#ifdef __linux__
    uuid_t uu;
    uuid_generate_random(uu);
    char *genUuid = (char *)malloc(37);
    uuid_unparse(uu, genUuid);
    session.token = genUuid;
    if (genUuid)
        free(genUuid);
#else
    UUID uuid;
    UuidCreate(&uuid);
    unsigned char *str;
    UuidToStringA(&uuid, &str);
    session.token = (char *)str;
    RpcStringFreeA(&str);
#endif // __linux__

    // inser in db
    std::string newToken = "";
    try
    {
        if (MainDB.GetSession())
        {
            // create new user
            *MainDB.sql << "INSERT INTO SESSIONS (token, user_id) VALUES (:P0, :P1)",
                soci::use(session.token), soci::use(session.userId);
            newToken = session.token;

            // if up is not empty, update it for user
            if (!ip.empty())
            {
                *MainDB.sql << "UPDATE USERS SET ip_addr = :P0 WHERE id = :P1",
                    soci::use(ip), soci::use(session.userId);
            }
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in SessionsService::CreateSession: {}", e.what());
        std::string err = "Error while create session: ";
        err.append(e.what());
        MainDB.FreeSession();
        throw std::runtime_error(err);
    }

    return newToken;
}

bool SessionsService::CheckSession(std::string token, std::string ip)
{
    bool result = false;
    SessionModel session;
    try
    {
        if (MainDB.GetSession())
        {
            soci::indicator indId, indToken, indUserId, indCreateTm, indLastTm;
            soci::statement st = (MainDB.sql->prepare << "SELECT id, token, user_id, create_at, last_connect FROM SESSIONS WHERE token = :P0",
                                  soci::use(token),
                                  soci::into(session.id, indId), soci::into(session.token, indToken), soci::into(session.userId, indUserId),
                                  soci::into(session.createAt, indCreateTm), soci::into(session.lastConnect, indLastTm));
            st.execute();
            st.fetch();

            if (indId == soci::indicator::i_ok)
            {
                result = true;
                // update last connect time to now
                time_t now = time(0);
                tm nowTm = *localtime(&now);
                *MainDB.sql << "UPDATE SESSIONS SET last_connect = :P0 WHERE token = :P1",
                    soci::use(nowTm), soci::use(token);

                // if up is not empty, update it for user
                if (!ip.empty())
                {
                    *MainDB.sql << "UPDATE USERS SET ip_addr = :P0 WHERE id = :P1",
                        soci::use(ip), soci::use(session.userId);
                }
            }
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in SessionsService::CheckSession: {}", e.what());
        std::string err = "Error while create session: ";
        err.append(e.what());
        MainDB.FreeSession();
        throw std::runtime_error(err);
    }
    return result;
}

void SessionsService::DeleteSession(std::string token)
{
    SessionModel session;
    try
    {
        if (MainDB.GetSession())
        {
            soci::indicator indId, indToken, indUserId, indCreateTm, indLastTm;
            soci::statement st = (MainDB.sql->prepare << "SELECT id, token, user_id, create_at, last_connect FROM SESSIONS WHERE token = :P0",
                                  soci::use(token),
                                  soci::into(session.id, indId), soci::into(session.token, indToken), soci::into(session.userId, indUserId),
                                  soci::into(session.createAt, indCreateTm), soci::into(session.lastConnect, indLastTm));
            st.execute();
            st.fetch();

            if (indId == soci::indicator::i_ok)
            {
                // update last connect time to now
                *MainDB.sql << "DELETE FROM SESSIONS WHERE token = :P0",
                    soci::use(token);
            }
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in SessionsService::DeleteSession: {}", e.what());
        std::string err = "Error while delete session: ";
        err.append(e.what());
        MainDB.FreeSession();
        throw std::runtime_error(err);
    }
}

void SessionsService::CheckAllSessionsTime()
{
    try
    {
        if (MainDB.GetSession())
        {
            soci::indicator ind;
            soci::rowset<soci::row> r = (MainDB.sql->prepare << "DELETE FROM SESSIONS WHERE EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - last_connect)) >= " +
                               std::to_string(SESSION_LIFETIME / 1000) + " RETURNING id");

            // find count of deleted sessions
            auto diff = std::distance(r.begin(), r.end());
            if (ind == soci::indicator::i_ok && diff)
                logger->info("Deleted {} sessions", diff);
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in SessionsService::CreateSession: {}", e.what());
        std::string err = "Error while create session: ";
        err.append(e.what());
        MainDB.FreeSession();
        throw std::runtime_error(err);
    }
}
