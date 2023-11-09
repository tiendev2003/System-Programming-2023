#include <iostream>
#include <sys/shm.h>
#include "../include/user.h"

key_t users_key = (key_t) rand();
key_t users_count_key = (key_t) rand();
/*
Kết nối và truy cập vào danh sách người dùng đã tồn tại dựa trên users_shmid. 
Hàm này trả về con trỏ đến danh sách người dùng (User*).
*/
User* att_users(int users_shmid) {
  return (User*) shmat(users_shmid, NULL, 0);
}
/*
Kết nối và truy cập vào số lượng người dùng đã tồn tại dựa trên users_count_shmid. 
Hàm này trả về con trỏ đến số lượng người dùng (int*).*/
int* att_users_count(int users_count_shmid) {
  return (int*) shmat(users_count_shmid, NULL, 0);
}
