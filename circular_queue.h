//接口三 入库管理 
#ifndef _CIRCULAR_QUEUE_H
#define _CIRCULAR_QUEUE_H
#include "ordered_list.h"
#include "medicine.h"

// 循环队列结构
typedef struct {
    InOrder *orders;            // 订单数组
    int front;                  // 队头指针
    int rear;                   // 队尾指针
    int capacity;               // 队列容量
    int size;                   // 当前大小
} CircularQueue;

// 初始化与销毁
CircularQueue* createQueue(int capacity);
void destroyQueue(CircularQueue *queue);

// 基本操作
int enqueue(CircularQueue *queue, InOrder order);    // 入队
int dequeue(CircularQueue *queue, InOrder *order);   // 出队
int isQueueEmpty(CircularQueue *queue);              // 判空
int isQueueFull(CircularQueue *queue);               // 判满
int getQueueSize(CircularQueue *queue);              // 获取大小

// 入库业务操作
int addInOrder(CircularQueue *queue, int med_id, int quantity, const char *operator_name);
int processInOrder(CircularQueue *queue, OrderedList *inventory); // 处理入库单（更新库存）
void displayInOrders(CircularQueue *queue);          // 显示所有入库单

// 入库台账
void generateInReport(CircularQueue *queue, OrderedList *inventory, const char *date);

#endif
