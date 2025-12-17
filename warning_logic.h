#ifndef _WARNING_LOGIC_H
#define _WARNING_LOGIC_H

#include "ordered_list.h"
#include <ctime>

// -------------------------- 核心接口（文档定义） --------------------------
int calculateThreeDayAverage(Medicine *med);
void setWarningThreshold(Medicine *med);
void setDynamicWarningThreshold(Medicine *med, const char *current_date);
void updateAllWarnings(OrderedList *inventory);
double getWarningResponseTime(Medicine *med);
void autoCheckWarnings(OrderedList *inventory, const char *current_date);

// -------------------------- 辅助接口（新增：声明calculateFluctuationCoefficient） --------------------------
const char* getMedicineCategory(int med_id);
double getSeasonCoefficient(const char *category, int month);
// 关键：添加该函数声明
double calculateFluctuationCoefficient(Medicine *med);

#endif
