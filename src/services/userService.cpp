#include "userService.hpp"

std::string UserService::CreateUser(UserModel user)
{
    std::string result = "";
    try
    {
        if (MainDB.GetSession())
        {
            // check that user does not exist
            soci::indicator ind;
            int id = 0;
            soci::statement st = (MainDB.sql->prepare << "SELECT id FROM USERS WHERE username = :P0",
                                  soci::use(user.username), soci::into(id, ind));
            st.execute();
            st.fetch();

            if (ind == soci::indicator::i_null)
            {
                result = "The user has already exist";
            }
            else
            {
                // create new user
                *MainDB.sql << "INSERT INTO USERS (username, password) VALUES (:P0, P1)",
                    soci::use(user.username), soci::use(user.password);
                result = "The user is created";
            }
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserService::CreateUser: {}", e.what());
        std::string err = "Error while creating user: ";
        err.append(e.what());
        throw std::runtime_error(err);
        MainDB.FreeSession();
    }
    return result;
}

UserModel UserService::GetUserById(int id)
{
    UserModel user;
    try
    {
        if (MainDB.GetSession())
        {
            soci::indicator indId, indName, indPsw, indIp, indAdm, indTime;
            int adminCheck = 0;
            soci::statement st = (MainDB.sql->prepare << "SELECT id, username, password, ip_addr, is_admin, create_at \
            FROM USERS WHERE id = :P0",
                                  soci::use(id),
                                  soci::into(user.id, indId), soci::into(user.username, indName), soci::into(user.password, indIp),
                                  soci::into(user.ip_addr, indIp), soci::into(adminCheck, indAdm), soci::into(user.create_at, indTime));
            st.execute();
            st.fetch();

            if (indAdm == soci::indicator::i_ok)
                user.is_admin = adminCheck == 0 ? false : true;
            else
                user.is_admin = false;
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserService::GetUserById: {}", e.what());
        std::string err = "Error while getting user: ";
        err.append(e.what());
        throw std::runtime_error(err);
        MainDB.FreeSession();
    }

    return user;
}

UserModel UserService::GetUserByToken(std::string token)
{
    UserModel user;
    try
    {
        if (MainDB.GetSession())
        {
            soci::indicator indId, indName, indPsw, indIp, indAdm, indTime;
            int adminCheck = 0;
            soci::statement st = (MainDB.sql->prepare << "SELECT USERS.id, USERS.username, USERS.password, USERS.ip_addr, \
            USERS.is_admin, USERS.create_at FROM USERS JOIN SESSIONS ON USERS.id = SESSIONS.user_id AND SESSIONS.token = :P0",
                                  soci::use(token),
                                  soci::into(user.id, indId), soci::into(user.username, indName), soci::into(user.password, indIp),
                                  soci::into(user.ip_addr, indIp), soci::into(adminCheck, indAdm), soci::into(user.create_at, indTime));
            st.execute();
            st.fetch();

            if (indAdm == soci::indicator::i_ok)
                user.is_admin = adminCheck == 0 ? false : true;
            else
                user.is_admin = false;
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserService::GetUserById: {}", e.what());
        std::string err = "Error while getting user: ";
        err.append(e.what());
        throw std::runtime_error(err);
        MainDB.FreeSession();
    }

    return user;
}

int UserService::GetUserId(UserModel user)
{
    int id = 0;
    try
    {
        if (MainDB.GetSession())
        {
            soci::indicator ind;
            soci::statement st = (MainDB.sql->prepare << "SELECT id FROM USERS WHERE username = :P0",
                                  soci::use(user.username), soci::into(id, ind));
            st.execute();
            st.fetch();

            // if user does not exist, return -1
            if (ind == soci::indicator::i_null)
                id = -1;
        }
        MainDB.FreeSession();
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserService::GetUserId: {}", e.what());
        std::string err = "Error while getting user id: ";
        err.append(e.what());
        throw std::runtime_error(err);
        MainDB.FreeSession();
    }

    return id;
}

void UserService::UpdateUser(UserModel updatedUser)
{
}

void UserService::DeleteUser(int id)
{
    try
    {
        *MainDB.sql << "DELETE FROM USERS WHERE id = :P0",
            soci::use(id);
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserService::DeleteUser: {}", e.what());
        std::string err = "Error while delete user: ";
        err.append(e.what());
        throw std::runtime_error(err);
        MainDB.FreeSession();
    }
}

bool UserService::CheckUserPassword(UserModel user)
{
    bool result = false;
    try
    {
        soci::indicator ind;
        int id = 0;
        soci::statement st = (MainDB.sql->prepare << "SELECT id FROM USERS WHERE username = :P0 AND password = :P1",
                              soci::use(user.username), soci::use(user.password),
                              soci::into(id, ind));
        st.execute();
        st.fetch();

        if (ind == soci::indicator::i_null)
            result = false;
        else
            result = true;
    }
    catch (const std::exception &e)
    {
        logger->error("Error in UserService::CheckUserPassword: {}", e.what());
        std::string err = "Error while check password: ";
        err.append(e.what());
        throw std::runtime_error(err);
        MainDB.FreeSession();
    }

    return result;
}