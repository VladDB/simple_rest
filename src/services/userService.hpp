#pragma once

#include "../components/tdb.hpp"

struct UserModel
{
    int id = -1;
    std::string username = "";
    std::string password = "";
    std::string ip_addr = "";
    bool is_admin = false;
    std::tm create_at;
};

class UserService
{
private:
    UserModel _user;

public:
    UserService(UserModel user) : _user(user) {};
    //~UserService();

    static std::string CreateUser(UserModel user);
    
    static UserModel GetUserById(int id);

    static UserModel GetUserByToken(std::string token);

    static std::vector<UserModel> GetAllUsers();
    
    // if id = -1 user does not exist
    static int GetUserId(UserModel user);
    
    static void UpdateUser(UserModel updatedUser);
    
    static void DeleteUser(int id);
    
    // check username and password, set id to model and update ip_addr in table
    static bool CheckUserPassword(UserModel user);
};
