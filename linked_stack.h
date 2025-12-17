//接口四 出库管理 
#ifndef _LINKED_STACK_H
#define _LINKED_STACK_H

#include "medicine.h"

// 链栈结构
typedef struct {
    OutOrderNode *top;          // 栈顶指针
    int size;                   // 栈大小
} LinkedStack;

// 初始化与销毁
LinkedStack* createStack();
void destroyStack(LinkedStack *stack);

// 基本操作
int push(LinkedStack *stack, OutOrderNode order);    // 入栈
int pop(LinkedStack *stack, OutOrderNode *order);    // 出栈
int isStackEmpty(LinkedStack *stack);                // 判空
int getStackSize(LinkedStack *stack);                // 获取大小
OutOrderNode* peek(LinkedStack *stack);              // 查看栈顶

// 出库业务操作
int addOutOrder(LinkedStack *stack, int med_id, int quantity, const char *prescription_no);
int processOutOrder(LinkedStack *stack, OrderedList *inventory); // 处理出库单
void displayOutOrders(LinkedStack *stack);           // 显示所有出库单

// 出库台账
void generateOutReport(LinkedStack *stack, OrderedList *inventory, const char *date);

#endif
