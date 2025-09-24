#include "test.pb.h"
#include <iostream>
#include <ostream>
#include <string>

using namespace pbTest;

int main() {
  LoginRequest req;
  req.set_name("Tom");
  req.set_pwd("Jerrys");

  std::string seri_str;
  if (req.SerializeToString(&seri_str)) {
    std::cout << seri_str.c_str() << "\n";
  }

  LoginRequest res;
  if (res.ParseFromString(seri_str)) {
    std::string name = res.name();
    std::string pwd = res.pwd();
    std::cout << name.c_str() << "\n" << pwd.c_str() << "\n";
  }
  std::cout << "\n";

  GetFriendListsResponse frsp;
  ResultCode *frc = frsp.mutable_result();
  frc->set_errcode(1);

  User *user1 = frsp.add_friend_list();
  user1->set_name("Tom");
  user1->set_age(20);
  user1->set_gender(pbTest::User_Gender_MAN);

  User *user2 = frsp.add_friend_list();
  user2->set_name("Jerry");
  user2->set_age(18);
  user2->set_gender(pbTest::User_Gender_MAN);

  std::cout << frsp.friend_list_size() << std::endl;
  User *resUser = frsp.mutable_friend_list(0);
  std::cout << resUser->name() << resUser->age() << resUser->gender()
            << std::endl;

  resUser->set_age(19);

  User *checkResUser = frsp.mutable_friend_list(0);
  std::cout << checkResUser->age() << std::endl;

  return 0;
}