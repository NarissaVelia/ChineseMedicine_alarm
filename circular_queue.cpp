#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include "circular_queue.h"

using namespace std;

CircularQueue* createQueue(int capacity) {
    if (capacity <= 0) {
        cout << "错误：队列容量必须大于0" << endl;
        return nullptr;
    }
    
    CircularQueue* queue = new CircularQueue;
    
    queue->orders = new InOrder[capacity];
    
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->capacity = capacity;
    
    cout << "循环队列创建成功，容量：" << capacity << endl;
    return queue;
}

void destroyQueue(CircularQueue* queue) {
    if (queue == nullptr) {
        return;
    }
    
    delete[] queue->orders;
    
    delete queue;
    
    cout << "队列已销毁" << endl;
}

bool isQueueEmpty(CircularQueue* queue) {
    if (queue == nullptr) return true;
    return queue->size == 0;
}

bool isQueueFull(CircularQueue* queue) {
    if (queue == nullptr) return false;
    return queue->size == queue->capacity;
}

int getQueueSize(CircularQueue* queue) {
    if (queue == nullptr) return 0;
    return queue->size;
}

bool enqueue(CircularQueue* queue, InOrder order) {
    if (queue == nullptr) {
        cout << "错误：队列不存在" << endl;
        return false;
    }
    
    if (isQueueFull(queue)) {
        cout << "错误：队列已满，无法添加新订单" << endl;
        return false;
    }
    
    queue->orders[queue->rear] = order;
    
    queue->rear = (queue->rear + 1) % queue->capacity;
    
    queue->size++;
    
    cout << "订单入队成功，当前队列大小：" << queue->size << endl;
    return true;
}

bool dequeue(CircularQueue* queue, InOrder* order) {
    if (queue == nullptr || order == nullptr) {
        cout << "错误：参数无效" << endl;
        return false;
    }
    
    if (isQueueEmpty(queue)) {
        cout << "提示：队列为空，没有可处理的订单" << endl;
        return false;
    }
    
    *order = queue->orders[queue->front];
    
    queue->front = (queue->front + 1) % queue->capacity;
    
    queue->size--;
    
    cout << "订单出队成功，当前队列大小：" << queue->size << endl;
    return true;
}

bool addInOrder(CircularQueue* queue, int med_id, int quantity, const string& operator_name) {
    if (queue == nullptr) {
        cout << "错误：队列不存在" << endl;
        return false;
    }
    
    if (med_id <= 0) {
        cout << "错误：药品ID必须大于0" << endl;
        return false;
    }
    
    if (quantity <= 0) {
        cout << "错误：入库数量必须大于0" << endl;
        return false;
    }
    
    if (operator_name.empty()) {
        cout << "错误：操作员姓名不能为空" << endl;
        return false;
    }
    
    InOrder new_order;
    new_order.med_id = med_id;
    new_order.quantity = quantity;
    
    strncpy(new_order.operator_name, operator_name.c_str(), sizeof(new_order.operator_name) - 1);
    new_order.operator_name[sizeof(new_order.operator_name) - 1] = '\0';
    
    new_order.in_time = time(nullptr);
    
    if (enqueue(queue, new_order)) {
        cout << "成功添加入库单：" << endl;
        cout << "  药品ID：" << med_id << endl;
        cout << "  数量：" << quantity << endl;
        cout << "  操作员：" << operator_name << endl;
        return true;
    }
    
    return false;
}

int processInOrder(CircularQueue* queue, OrderedList* inventory) {
    if (queue == nullptr) {
        cout << "错误：队列不存在" << endl;
        return 0;
    }
    
    if (inventory == nullptr) {
        cout << "错误：库存主表不存在" << endl;
        return 0;
    }
    
    if (isQueueEmpty(queue)) {
        cout << "提示：没有待处理的入库单" << endl;
        return 0;
    }
    
    cout << "\n========== 开始处理入库单 ==========" << endl;
    cout << "待处理数量：" << queue->size << endl;
    
    int processed_count = 0;
    InOrder current_order;
    
    while (dequeue(queue, &current_order)) {
        processed_count++;
        
        Medicine* medicine = findMedicine(inventory, current_order.med_id);
        
        if (medicine != nullptr) {
            medicine->stock += current_order.quantity;
            
            cout << "\n[处理入库单 " << processed_count << "]" << endl;
            cout << "药品：" << medicine->name << " (ID: " << current_order.med_id << ")" << endl;
            cout << "入库数量：" << current_order.quantity << endl;
            cout << "原库存：" << medicine->stock - current_order.quantity << endl;
            cout << "新库存：" << medicine->stock << endl;
            cout << "操作员：" << current_order.operator_name << endl;
            
            char time_str[20];
            tm* timeinfo = localtime(&current_order.in_time);
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);
            cout << "入库时间：" << time_str << endl;
            
            if (medicine->is_warning && medicine->stock >= medicine->warning_threshold) {
                cout << "注意：库存已恢复，预警解除！" << endl;
                medicine->is_warning = false;
            }
        } else {
            cout << "警告：药品ID " << current_order.med_id << " 不存在，跳过此订单" << endl;
        }
    }
    
    cout << "\n========== 处理完成 ==========" << endl;
    cout << "共处理入库单：" << processed_count << " 个" << endl;
    
    return processed_count;
}

void displayInOrders(CircularQueue* queue) {
    if (queue == nullptr) {
        cout << "错误：队列不存在" << endl;
        return;
    }
    
    if (isQueueEmpty(queue)) {
        cout << "提示：入库队列为空" << endl;
        return;
    }
    
    cout << "\n========== 入库单队列详情 ==========" << endl;
    cout << "队列容量：" << queue->capacity << "，当前大小：" << queue->size << endl;
    cout << "队头位置：" << queue->front << "，队尾位置：" << queue->rear << endl;
    cout << "----------------------------------------" << endl;
    
    for (int i = 0; i < queue->size; i++) {
        int index = (queue->front + i) % queue->capacity;
        InOrder& order = queue->orders[index];
        
        char time_str[20];
        tm* timeinfo = localtime(&order.in_time);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", timeinfo);
        
        cout << "入库单 " << (i + 1) << ":" << endl;
        cout << "  药品ID：" << order.med_id << endl;
        cout << "  入库数量：" << order.quantity << endl;
        cout << "  操作员：" << order.operator_name << endl;
        cout << "  创建时间：" << time_str << endl;
        cout << "  状态：等待处理" << endl;
        cout << "----------------------------------------" << endl;
    }
    
    cout << "========== 显示结束 ==========" << endl;
}

void generateInReport(CircularQueue* queue, OrderedList* inventory, const string& date) {
    if (queue == nullptr || inventory == nullptr) {
        cout << "错误：参数无效" << endl;
        return;
    }
    
    cout << "\n==========================================" << endl;
    cout << "          中药库存入库台账" << endl;
    cout << "日期：" << date << endl;
    
    time_t now = time(nullptr);
    char time_str[20];
    tm* timeinfo = localtime(&now);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", timeinfo);
    cout << "生成时间：" << time_str << endl;
    cout << "==========================================" << endl;
    
    cout << left << setw(10) << "药品ID" 
         << setw(15) << "药品名称" 
         << setw(12) << "前日结余" 
         << setw(12) << "当日入库" 
         << setw(12) << "当日结余" << endl;
    
    cout << string(61, '-') << endl;
    
    if (isQueueEmpty(queue)) {
        cout << "今日无入库记录" << endl;
    } else {
        cout << "今日入库统计：" << endl;
        
        int total_quantity = 0;
        for (int i = 0; i < queue->size; i++) {
            int index = (queue->front + i) % queue->capacity;
            total_quantity += queue->orders[index].quantity;
        }
        
        cout << "待处理入库单数：" << queue->size << endl;
        cout << "待入库总量：" << total_quantity << endl;
        
        cout << "\n入库单详情：" << endl;
        for (int i = 0; i < queue->size; i++) {
            int index = (queue->front + i) % queue->capacity;
            InOrder& order = queue->orders[index];
            
            Medicine* medicine = findMedicine(inventory, order.med_id);
            string med_name = (medicine != nullptr) ? medicine->name : "未知药品";
            
            cout << "  " << (i + 1) << ". " << med_name 
                 << " (ID:" << order.med_id 
                 << ")，数量：" << order.quantity << endl;
        }
    }
    
    cout << "==========================================" << endl;
}
