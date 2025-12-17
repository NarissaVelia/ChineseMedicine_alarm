//接口六 主系统接口 
#ifndef _INVENTORY_SYSTEM_H
#define _INVENTORY_SYSTEM_H

#include "ordered_list.h"
#include "circular_queue.h"
#include "linked_stack.h"
#include "statistics.h"

// 系统主结构
typedef struct {
    OrderedList *inventory;     // 库存主表
    CircularQueue *inQueue;     // 入库队列
    LinkedStack *outStack;      // 出库栈
    DailyStats today_stats;     // 今日统计
    char current_date[11];      // 当前日期
} InventorySystem;

// 系统初始化
InventorySystem* initSystem();
void shutdownSystem(InventorySystem *system);

// 药品管理菜单
void medicineManagementMenu(InventorySystem *system);

// 入库管理菜单
void inStockManagementMenu(InventorySystem *system);

// 出库管理菜单
void outStockManagementMenu(InventorySystem *system);

// 库存查询菜单
void inventoryQueryMenu(InventorySystem *system);

// 统计报表菜单
void statisticsMenu(InventorySystem *system);

// 预警监控
void monitorWarnings(InventorySystem *system);  // 实时监控预警
void displayWarningList(InventorySystem *system); // 显示预警列表

// 系统工具
void backupData(InventorySystem *system);       // 数据备份
void restoreData(InventorySystem *system);      // 数据恢复
void systemLog(const char *message);            // 系统日志

#endif
