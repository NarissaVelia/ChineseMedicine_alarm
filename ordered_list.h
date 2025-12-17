#ifndef _ORDERED_LIST_H
#define _ORDERED_LIST_H

//接口二 库存主表 
#include "medicine.h"

// 有序顺序表结构
typedef struct {
    Medicine *medicines;        // 药品数组
    int length;                 // 当前长度
    int capacity;               // 总容量
} OrderedList;

// 初始化与销毁
OrderedList* createOrderedList(int capacity);
void destroyOrderedList(OrderedList *list);

// 基本操作
int insertMedicine(OrderedList *list, Medicine med);      // 插入并保持有序
int deleteMedicine(OrderedList *list, int id);            // 按ID删除
Medicine* findMedicine(OrderedList *list, int id);        // 按ID查找（二分查找）
int updateMedicine(OrderedList *list, Medicine med);      // 更新药品信息

// 库存操作
int increaseStock(OrderedList *list, int id, int quantity);   // 增加库存
int decreaseStock(OrderedList *list, int id, int quantity);   // 减少库存
int getCurrentStock(OrderedList *list, int id);               // 获取当前库存

// 预警相关
int checkWarningStatus(OrderedList *list, int id);            // 检查是否需要预警
void updateWarningThreshold(OrderedList *list, int id);       // 更新预警阈值
int clearWarning(OrderedList *list, int id);                  // 清除预警状态

// 遍历操作
void traverseList(OrderedList *list, void (*visit)(Medicine*)); // 遍历所有药品
Medicine* getMedicineByIndex(OrderedList *list, int index);     // 按索引获取

// 文件操作
int saveToFile(OrderedList *list, const char *filename);       // 保存到文件
OrderedList* loadFromFile(const char *filename);               // 从文件加载

// 1. 二分查找（替换原有顺序查找，效率优化）
Medicine* binarySearchMedicine(OrderedList *list, int id);

// 2. 批量操作
// 批量插入药品（传入药品数组和数量，返回成功插入的个数）
int batchInsertMedicine(OrderedList *list, Medicine *med_arr, int med_count);
// 批量删除药品（传入ID数组和数量，返回成功删除的个数）
int batchDeleteMedicine(OrderedList *list, int *id_arr, int id_count);

// 3. 文件存储/读取
// 保存库存表到文件（二进制存储，支持后续恢复）
int saveToFile(OrderedList *list, const char *filename);
// 从文件加载库存表（覆盖原有数据，返回新创建的有序表）
OrderedList* loadFromFile(const char *filename);

#endif
