//接口五 统计与报表接口
#ifndef _STATISTICS_H
#define _STATISTICS_H

#include "ordered_list.h"
#include "circular_queue.h"
#include "linked_stack.h"

// 日常统计
typedef struct {
    char date[11];                  // 统计日期
    int total_prescriptions;        // 当日处方量
    int total_dosage;               // 当日总剂量
    int warning_triggered;          // 预警触发次数
    double avg_response_time;       // 平均响应时间
} DailyStats;

// 药品使用统计
typedef struct {
    int med_id;
    char med_name[MAX_NAME_LEN];
    int total_usage;                // 总用量
    int usage_frequency;            // 使用频次
    double usage_percentage;        // 用量占比
} UsageStats;

// 统计操作接口
DailyStats generateDailyReport(OrderedList *inventory, 
                               CircularQueue *inQueue, 
                               LinkedStack *outStack,
                               const char *date);

UsageStats* getTopUsageRanking(OrderedList *inventory, int top_n);  // 获取用量排名
UsageStats* getTopFrequencyRanking(OrderedList *inventory, int top_n); // 获取频次排名

// 对比分析
void compareThreeDaysUsage(OrderedList *inventory);  // 近三日用量对比

// 预警统计
void calculateAvgResponseTime(OrderedList *inventory);  // 计算平均响应时间
int getWarningDrugs(OrderedList *inventory, int *warning_ids); // 获取当前预警药品

// 文件输出
void exportStatistics(DailyStats stats, UsageStats *usage_ranking, const char *filename);

#endif
