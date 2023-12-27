#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "../include/user.h"
#include "../include/message.h"
#include "../include/shm_queue.h"

using namespace std;

int users_shmid = shmget(users_key, MAX_USERS * sizeof(User), 0666);
int users_count_shmid = shmget(users_count_key, sizeof(int), 0666);

User current_user;

void init();
void create_user(string username, string password);
void select_user();
void receive_messages();
void send_messages();
void print_message(Message *message);
void showInterface();
int cinInt();
void clientRegister();
void clientLogin();
int main()
{
  init();
  // create_user();

  // system("clear");
  // cout << endl
  //      << "> Welcome " << current_user.name << endl;
  // cout << endl
  //      << "Nhập :q để thoát khỏi" << endl
  //      << endl;

  // thread receive_messages_thread(receive_messages);
  // receive_messages_thread.detach();
  // send_messages();
  showInterface();
  return 0;
}
void clientRegister()
{
  std::string username, password;
  std::vector<std::string> usernames;

  std::ifstream infile("db.txt");
  int count = 0;
  if (infile.is_open())
  {
    std::string line;
    std::string username, password;

    // Đọc từng dòng trong file
    while (std::getline(infile, line))
    {
      std::istringstream iss(line);

      // Tách username và password từ dòng
      if (std::getline(iss, username, ',') && std::getline(iss, password))
      {
        usernames.push_back(username);
      }
    }

    // Đóng file
    infile.close();
  }
  else
  {
    std::cerr << "Không thể mở file để đọc.\n";
  }
  bool isDuplicate = false;
  do
  {
    cout << "Nhập tên đăng nhập: ";
    cin >> username;

    for (const std::string &u : usernames)
    {
      if (u == username)
      {
        cout << u;
        isDuplicate = true;
        break;
      }
      else
      {
        isDuplicate = false;
      }
    }

    if (isDuplicate)
    {
      std::cout << "Tên đã tồn tại. Vui lòng nhập tên khác.\n";
    }
  } while (isDuplicate);
  cout << "Nhập mật khẩu: ";
  cin >> password;
  ofstream myfile("db.txt", ios_base::app);
  if (myfile.is_open())
  {
    myfile << username << "," << password << "\n";
    myfile.close();

    create_user(username, password);
    system("clear");
    cout << endl
         << "> Welcome " << current_user.name << endl;
    cout << endl
         << "Nhập :q để thoát khỏi" << endl
         << endl;

    thread receive_messages_thread(receive_messages);
    receive_messages_thread.detach();
    send_messages();
  }
  else
  {

    cout << "Unable to open file";
  }
}
void clientLogin()
{
  char username[50];
  cout << "Nhập tên đăng nhập: ";
  cin >> username;
  cout << "Nhập mật khẩu: ";
  char password[50];
  cin >> password;

  std::ifstream infile("db.txt");
  int count = 0;
  if (infile.is_open())
  {
    std::string line;
    std::string username, password;

    // Đọc từng dòng trong file
    while (std::getline(infile, line))
    {
      std::istringstream iss(line);

      // Tách username và password từ dòng
      if (std::getline(iss, username, ',') && std::getline(iss, password))
      {
        if (username == username && password == password)
        {
          count++;
          cout << "Đăng nhập thành công" << endl;
          infile.close();
          create_user(username, password);
          system("clear");
          cout << endl
               << "> Welcome " << current_user.name << endl;
          cout << endl
               << "Nhập :q để thoát khỏi" << endl
               << endl;

          thread receive_messages_thread(receive_messages);
          receive_messages_thread.detach();
          send_messages();
          break;
        }
      }
    }
    if (count == 0)
    {
      cout << "Đăng nhập thất bại" << endl;
    }

    // Đóng file
    infile.close();
  }
  else
  {
    std::cerr << "Không thể mở file để đọc.\n";
  }
}

void showInterface()
{
  cout << "Chào mừng đến với chat" << endl;

  while (1)
  {
    cout << "1. Đăng nhập" << endl;
    cout << "2. Đăng ký" << endl;
    cout << "3. Thoát" << endl;
    cout << "Nhập lựa chọn: ";
    int choice = cinInt();
    switch (choice)
    {
    case 1:
      cout << "Đăng nhập" << endl;
      clientLogin();
      break;
    case 2:
      cout << "Đăng ký" << endl;
      clientRegister();
      break;
    case 3:
      cout << "Thoát" << endl;
      exit(0);
      break;
    }
  }
}
int cinInt()
{
  int n;
  while (1)
  {

    // Kiểm tra nếu người dùng nhập một số hợp lệ
    if (cin >> n)
    {
      break; // Nếu là số, thoát khỏi vòng lặp
    }
    else
    {
      cin.clear();                                         // Xóa trạng thái lỗi của cin
      cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Loại bỏ dữ liệu không hợp lệ từ bộ nhớ đệm
      cout << "Sai định dạng! Vui lòng nhập lại.\n";
    }
  }
  return n;
}

void init()
{
  srand(time(NULL));

  if ((users_shmid < 0) || (users_count_shmid < 0))
  {
    perror("shmget");
    cout << "Unable to connect with the server!" << endl;

    exit(1);
  }
}

int create_shmq()
{
  key_t key = rand();
  int shmq_id = shmget(key, sizeof(ShmQueue), 0666 | IPC_CREAT);

  ShmQueue *shmq = att_shmq(shmq_id);
  memcpy(shmq, create_queue(), sizeof(ShmQueue));
  shmdt(shmq);

  return shmq_id;
}

void create_user(string username, string password)
{
  User *users = att_users(users_shmid);
  int *users_count = att_users_count(users_count_shmid);

  User *new_user = new User();

  strcpy(new_user->name, username.c_str());
  strcpy(new_user->password, password.c_str());
  new_user->shmq_id = create_shmq();

  memcpy(&users[(*users_count)], new_user, sizeof(User));
  memcpy(users_count, &++(*users_count), sizeof(int));
  memcpy(&current_user, new_user, sizeof(User));

  shmdt(users);
  shmdt(users_count);
}

void select_user(User *user_to_chat)
{
  User *users = att_users(users_shmid);
  int *users_count = att_users_count(users_count_shmid);

  if (*users_count == 1)
  {
    cout << endl
         << "0 Users online!" << endl;
  }
  else
  {
    users = att_users(users_shmid);

    cout << endl
         << "Online users:" << endl;
    for (int i = 0; i < (*users_count); i++)
    {
      if (users[i].shmq_id == current_user.shmq_id)
        continue;
      cout << i << " - " << users[i].name << endl;
    }

    int user_to_chat_index;
    cout << endl
         << "Select the user to chat: ";
    cin >> user_to_chat_index;

    memcpy(user_to_chat, &users[user_to_chat_index], sizeof(User));
  }

  shmdt(users);
  shmdt(users_count);
}

void receive_messages()
{
  while (true)
  {
    ShmQueue *shmq = att_shmq(current_user.shmq_id);
    if (!empty(shmq))
    {
      Message *message = new Message();

      if (dequeue(shmq, message))
        print_message(message);
    }
    shmdt(shmq);
  }
}

void send_messages()
{
  while (true)
  {
    char text[200];
    cin.getline(text, 200);

    Message *message = new Message();
    strcpy(message->source_name, current_user.name);
    strcpy(message->text, text);

    if (strcmp(text, ":q") == 0)
      break;
    else if (strlen(message->text) != 0)
    {
      char message_mode = 'B';
      cout << endl
           << "Select the message mode ([U]nicast or [B]roadcast): ";
      cin >> message_mode;

      if (message_mode == 'U' || message_mode == 'u')
      {
        User *user_to_chat = new User();
        select_user(user_to_chat);

        message->mode = UNICAST;
        message->sent_at = time(0);

        ShmQueue *shmq = att_shmq(user_to_chat->shmq_id);
        enqueue(shmq, message);
        shmdt(shmq);
      }
      else
      {
        message->mode = BROADCAST;
        message->sent_at = time(0);

        User *users = att_users(users_shmid);
        int *users_count = att_users_count(users_count_shmid);

        for (int i = 0; i < *users_count; i++)
        {
          if (users[i].shmq_id == current_user.shmq_id)
            continue;

          ShmQueue *shmq = att_shmq(users[i].shmq_id);
          enqueue(shmq, message);
          shmdt(shmq);
        }

        shmdt(users);
        shmdt(users_count);
      }
    }

    cout << endl;
  }
}

void print_message(Message *message)
{
  ofstream myfile("log.txt", ios_base::app);
  if (myfile.is_open())
  {
    myfile << ctime(&message->sent_at) << "\n";
    myfile << " > " << message->source_name << " (" << message->mode << "): " << message->text << "\n";
    myfile.close();
  }
  else
    cout << "Unable to open file";
  cout << endl
       << ctime(&message->sent_at);
  cout << " > " << message->source_name << " (" << message->mode << "): " << message->text << endl
       << endl;
}
