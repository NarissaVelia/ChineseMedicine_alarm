//接口一 核心数据结构定义 
#ifndef _MEDICINE_H
#define _MEDICINE_H

#include <cstring>
#include <ctime>

#define MAX_NAME_LEN 50
#define MAX_ORIGIN_LEN 50
#define MAX_SPEC_LEN 20
#define MAX_MEDICINES 300
#define HISTORY_DAYS 7  // 记录7天用量历史

// 药品基本信息结构
typedef struct {
    int id;                     // 药品编号（唯一标识）
    char name[MAX_NAME_LEN];    // 药品名称
    char origin[MAX_ORIGIN_LEN]; // 产地
    char spec[MAX_SPEC_LEN];    // 规格
    int stock;                  // 当前库存量
    int warning_threshold;      // 预警阈值
    int last_usage;             // 最近一次用量
    int usage_history[HISTORY_DAYS]; // 近7天用量历史
    int is_warning;             // 预警状态：0-正常，1-预警
    time_t warning_time;        // 预警开始时间
    time_t response_time;       // 预警响应时间（解除时间）
} Medicine;

// 入库单结构（用于循环队列）
typedef struct {
    int med_id;                 // 药品编号
    int quantity;               // 入库数量
    char operator_name[20];          // 操作员
    time_t in_time;             // 入库时间
} InOrder;

// 出库单结构（用于链栈）
typedef struct OutOrderNode {
    int med_id;                 // 药品编号
    int quantity;               // 出库数量
    char prescription_no[30];   // 处方号
    time_t out_time;            // 出库时间
    struct OutOrderNode *next;  // 下一节点指针
} OutOrderNode;

// 统计数据结构
typedef struct {
    int total_prescriptions;    // 总处方数
    int total_dosage;           // 总剂量
    int warning_count;          // 预警次数
    double avg_response_time;   // 平均响应时间
} Statistics;

#endif
