#include "message.h"
/**
 Shared Memory" (bộ nhớ chia sẻ) là một kỹ thuật cho phép các tiến trình
  hoặc luồng trong một ứng dụng chia sẻ cùng một vùng bộ nhớ.
  "Queue" (hàng đợi) là một cấu trúc dữ liệu trong lập trình, 
  trong đó các phần tử được thêm vào ở một đầu 
  và được lấy ra từ đầu kia theo nguyên tắc "First-In-First-Out" (FIFO),
  tức là phần tử được thêm vào trước sẽ được lấy ra trước.
*/
#ifndef SHM_QUEUE_H

#define SHM_QUEUE_H
#define MAX_MESSAGES 10000

typedef struct
{
  int capacity;            // Sức chứa của hàng đợi
  int size;                // Kích thước hiện tại của hàng đợi
  int front;               // Vị trí phần tử đầu tiên trong hàng đợi
  int rear;                // Vị trí phần tử cuối cùng trong hàng đợi
  int messages_list_shmid; // ID của vùng bộ nhớ chứa danh sách tin nhắn
  bool locked;             // Trạng thái khóa của hàng đợi
} ShmQueue;

ShmQueue *create_queue();                            // Tạo một hàng đợi mới
bool empty(ShmQueue *shmq);                          // Kiểm tra hàng đợi có trống hay không
bool full(ShmQueue *shmq);                           // Kiểm tra hàng đợi có đầy hay không
void enqueue(ShmQueue *shmq, Message *message);      // Thêm một tin nhắn vào hàng đợi
bool dequeue(ShmQueue *shmq, Message *dest_message); // Lấy một tin nhắn từ hàng đợi và gán cho dest_message

ShmQueue *att_shmq(int shmq_id); // Kết nối và truy cập vào hàng đợi đã tồn tại

#endif