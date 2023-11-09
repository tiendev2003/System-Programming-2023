#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../include/shm_queue.h"

key_t generate_key();

//Tạo một hàng đợi mới và trả về con trỏ đến hàng đợi đó.
ShmQueue* create_queue() {
  ShmQueue* shmq = new ShmQueue();
  shmq->capacity = MAX_MESSAGES;  
  shmq->front = shmq->size = 0;  
  shmq->rear = MAX_MESSAGES - 1;
  shmq->locked = false;
  shmq->messages_list_shmid = shmget(generate_key(), MAX_MESSAGES * sizeof(Message), 0666|IPC_CREAT);

  return shmq;
}
//Kiểm tra xem hàng đợi có trống hay không. Trả về true nếu hàng đợi trống, ngược lại trả về false.
bool empty(ShmQueue* shmq) {
  return (shmq->size == 0);
}
//Kiểm tra xem hàng đợi có đầy hay không. Trả về true nếu hàng đợi đầy, ngược lại trả về false
bool full(ShmQueue* shmq) {
  return (shmq->size == shmq->capacity);
}
// Thêm một tin nhắn vào hàng đợi. Nếu hàng đợi đã đầy, không thực hiện thêm. Nếu thành công, tin nhắn được sao chép vào vùng nhớ chia sẻ.
void enqueue(ShmQueue* shmq, Message* message) {
  if(full(shmq)) return;
  shmq->locked = true;

  shmq->rear = (shmq->rear + 1) % shmq->capacity;
  shmq->size = shmq->size + 1;

  Message* messages = (Message*) shmat(shmq->messages_list_shmid, NULL, 0);
  memcpy(&messages[shmq->rear], message, sizeof(Message));
  shmdt(messages);
  shmq->locked = false;
}
// Lấy một tin nhắn từ hàng đợi và gán cho dest_message. Nếu hàng đợi trống hoặc đang bị khóa, trả về false.
bool dequeue(ShmQueue* shmq, Message* dest_message) {
  if(empty(shmq) || shmq->locked) return false;

  Message* messages = (Message*) shmat(shmq->messages_list_shmid, NULL, 0);
  memcpy(dest_message, &messages[shmq->front], sizeof(Message));
  shmdt(messages);

  shmq->front = (shmq->front + 1) % shmq->capacity;  
  shmq->size = shmq->size - 1;

  return true;
}
// Kết nối và truy cập vào hàng đợi chia sẻ đã tồn tại dựa trên shmid.
ShmQueue* att_shmq(int shmid) {
  return (ShmQueue*) shmat(shmid, NULL, 0);
}
//  Sinh một khóa ngẫu nhiên để sử dụng trong quá trình tạo bộ nhớ chia sẻ.
key_t generate_key() {
  srand(time(NULL));
  return (key_t) rand();
}
