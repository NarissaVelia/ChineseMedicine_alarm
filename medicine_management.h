#ifndef _MEDICINE_MANAGEMENT_H
#define _MEDICINE_MANAGEMENT_H

#include "ordered_list.h"

// -------------------------- 数据验证接口 --------------------------
// 验证药品ID合法性（非负、未重复）
int validateMedicineID(OrderedList *list, int id);
// 验证药品基本信息合法性（名称、产地、规格非空，库存量/阈值非负）
int validateMedicineInfo(Medicine *med);

// -------------------------- 药品管理功能接口 --------------------------
// 新增药品（含数据验证）
int addMedicine(OrderedList *list);
// 修改药品信息（按ID查找，含数据验证）
int modifyMedicine(OrderedList *list);
// 删除药品（按ID，含二次确认）
int removeMedicine(OrderedList *list);
// 查询药品（支持按ID/名称模糊查询，含界面展示）
void queryMedicine(OrderedList *list);
// 显示所有药品（格式化界面）
void showAllMedicines(OrderedList *list);

// -------------------------- 药品管理菜单接口 --------------------------
// 药品管理主菜单（循环交互）
void medicineManagementMenu(OrderedList *list);

#endif
