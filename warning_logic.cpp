#include "warning_logic.h"
#include <cmath>
#include <ctime>
#include <cstring>
#include <iostream>

using namespace std;

// -------------------------- 辅助功能实现（内部使用） --------------------------
// 1. 按药品ID划分类别（0-9=解表类，10-19=清热类，20-29=补益类，30-39=温里类，其他=理气类）
const char* getMedicineCategory(int med_id) {
    if (med_id >= 0 && med_id <= 9) {
        return "解表类";
    } else if (med_id >= 10 && med_id <= 19) {
        return "清热类";
    } else if (med_id >= 20 && med_id <= 29) {
        return "补益类";
    } else if (med_id >= 30 && med_id <= 39) {
        return "温里类";
    } else {
        return "理气类";
    }
}

// 2. 根据类别和月份获取季节系数α（参考季节系数.csv）
double getSeasonCoefficient(const char *category, int month) {
    // 季节划分：2-4=春(0)，5-7=夏(1)，8-10=秋(2)，11-1=冬(3)
    int season;
    if (month >= 2 && month <= 4) {
        season = 0;
    } else if (month >= 5 && month <= 7) {
        season = 1;
    } else if (month >= 8 && month <= 10) {
        season = 2;
    } else {
        season = 3;
    }

    // 季节系数矩阵（行：药品类别，列：春夏秋冬）
    double season_coeff[5][4] = {
        {1.2, 0.8, 1.0, 1.5},  // 解表类
        {0.9, 1.4, 1.1, 0.7},  // 清热类
        {1.0, 0.9, 1.3, 1.2},  // 补益类
        {0.8, 0.6, 0.9, 1.6},  // 温里类
        {1.1, 1.0, 1.1, 1.0}   // 理气类（默认）
    };

    // 匹配类别索引
    int cat_idx = 4;  // 默认理气类
    if (strcmp(category, "解表类") == 0) {
        cat_idx = 0;
    } else if (strcmp(category, "清热类") == 0) {
        cat_idx = 1;
    } else if (strcmp(category, "补益类") == 0) {
        cat_idx = 2;
    } else if (strcmp(category, "温里类") == 0) {
        cat_idx = 3;
    }

    return season_coeff[cat_idx][season];
}

// 3. 计算历史波动系数γ：γ=1+(σ÷μ)×0.5（σ=前三日用量标准差，μ=前三日均用量）
double calculateFluctuationCoefficient(Medicine *med) {
    if (med == NULL) {
        return 1.0;  // 异常情况返回默认值（无波动）
    }

    int sum = 0, count = 0;
    // 前三日用量：D-0（当天）、D-1（昨天）、D-2（前天）→ 对应usage_history[6]、[5]、[4]
    int three_days[3] = {
        med->usage_history[6],
        med->usage_history[5],
        med->usage_history[4]
    };

    // 计算前三日均用量μ（仅统计有效用量，排除0值）
    for (int i = 0; i < 3; i++) {
        if (three_days[i] > 0) {
            sum += three_days[i];
            count++;
        }
    }
    if (count == 0) {
        return 1.0;  // 无有效用量，波动系数设为1.0
    }
    double mu = sum / (double)count;

    // 计算标准差σ（衡量用量波动程度）
    double sigma_sum = 0.0;
    for (int i = 0; i < 3; i++) {
        if (three_days[i] > 0) {
            sigma_sum += pow(three_days[i] - mu, 2);  // 平方差求和
        }
    }
    double sigma = sqrt(sigma_sum / count);  // 标准差=平方差均值的平方根

    // 计算波动系数γ（控制在0.8~1.5之间，避免极端值影响阈值）
    double gamma = 1.0 + (sigma / mu) * 0.5;
    // 边界控制：低于0.8取0.8，高于1.5取1.5
    return (gamma < 0.8) ? 0.8 : ((gamma > 1.5) ? 1.5 : gamma);
}

// -------------------------- 核心接口实现（文档定义） --------------------------
// 1. 计算前三日均用量（D-0=当天、D-1=昨天、D-2=前天）
int calculateThreeDayAverage(Medicine *med) {
    if (med == NULL) {
        return 0;
    }

    int sum = 0, count = 0;
    // 前三日用量索引：D-0→6，D-1→5，D-2→4
    int three_days[3] = {
        med->usage_history[6],
        med->usage_history[5],
        med->usage_history[4]
    };

    // 统计有效用量（排除0值，避免拉低均值）
    for (int i = 0; i < 3; i++) {
        if (three_days[i] > 0) {
            sum += three_days[i];
            count++;
        }
    }

    // 无有效用量返回0，否则返回均值（整数除法，向下取整）
    return (count == 0) ? 0 : (sum / count);
}

// 2. 设置基础预警阈值：MAX(基础阈值, 前三日均值×10%/最近一次用量×10%)
void setWarningThreshold(Medicine *med) {
    if (med == NULL) {
        return;
    }

    int three_day_avg = calculateThreeDayAverage(med);
    int threshold;

    // 确定阈值计算基准：前三日有用量则用均值10%，无则用最近一次用量10%
    if (three_day_avg > 0) {
        threshold = static_cast<int>(three_day_avg * 0.1);
    } else {
        threshold = static_cast<int>(med->last_usage * 0.1);
    }

    // 最终阈值不低于药物基本信息中的基础阈值（避免阈值过低）
    med->warning_threshold = max(med->warning_threshold, threshold);
}

// 3. 设置动态预警阈值：动态阈值=基础阈值×α（季节系数）×γ（波动系数）
void setDynamicWarningThreshold(Medicine *med, const char *current_date) {
    if (med == NULL || current_date == NULL) {
        return;
    }

    // 步骤1：解析当前日期中的月份（格式：YYYY-MM-DD，月份在第6-7位）
    int month = atoi(current_date + 5);
    // 月份合法性校验（1-12）
    if (month < 1 || month > 12) {
        cout << "警告：日期格式非法，月份需为1-12，默认按1月计算！" << endl;
        month = 1;
    }

    // 步骤2：获取季节系数α
    const char *category = getMedicineCategory(med->id);
    double alpha = getSeasonCoefficient(category, month);

    // 步骤3：获取波动系数γ
    double gamma = calculateFluctuationCoefficient(med);

    // 步骤4：计算动态阈值（基础阈值×α×γ，向上取整确保阈值充足）
    int base_threshold = med->warning_threshold;
    int dynamic_threshold = static_cast<int>(ceil(base_threshold * alpha * gamma));

    // 步骤5：阈值下限控制（不低于基础阈值的50%，避免阈值过低）
    med->warning_threshold = max(dynamic_threshold, static_cast<int>(base_threshold * 0.5));

    // 日志输出（便于调试）
    cout << "动态阈值计算完成：" << endl;
    cout << "药品ID=" << med->id << "，类别=" << category << "，月份=" << month << endl;
    cout << "季节系数α=" << alpha << "，波动系数γ=" << gamma << endl;
    cout << "基础阈值=" << base_threshold << "g，动态阈值=" << med->warning_threshold << "g" << endl;
}

// 4. 检查并更新所有药品预警状态（库存量<阈值触发预警，≥阈值解除预警）
void updateAllWarnings(OrderedList *inventory) {
    if (inventory == NULL || inventory->length == 0) {
        cout << "警告：库存表为空，无需更新预警状态！" << endl;
        return;
    }

    time_t now = time(NULL);  // 当前时间（用于记录预警/解除时间）
    int warning_count = 0;     // 统计当前预警药品数量

    cout << "\n===== 开始更新所有药品预警状态 =====" << endl;
    for (int i = 0; i < inventory->length; i++) {
        Medicine *med = &inventory->medicines[i];

        // 情况1：触发预警（库存量<阈值 且 未处于预警状态）
        if (med->stock < med->warning_threshold && med->is_warning == 0) {
            med->is_warning = 1;
            med->warning_time = now;  // 记录预警触发时间
            warning_count++;
            cout << "【预警触发】ID=" << med->id << "，名称=" << med->name << endl;
            cout << "库存=" << med->stock << "g < 阈值=" << med->warning_threshold << "g" << endl;
        }
        // 情况2：解除预警（库存量≥阈值 且 处于预警状态）
        else if (med->stock >= med->warning_threshold && med->is_warning == 1) {
            med->is_warning = 0;
            med->response_time = now;  // 记录预警解除时间
            double response_hour = getWarningResponseTime(med);
            cout << "【预警解除】ID=" << med->id << "，名称=" << med->name << endl;
            cout << "库存=" << med->stock << "g ≥ 阈值=" << med->warning_threshold << "g，响应时间=" << response_hour << "小时" << endl;
        }
    }

    cout << "===== 预警状态更新完成，当前预警药品总数：" << warning_count << "个 =====" << endl;
}

// 5. 获取预警响应时间（单位：小时）
double getWarningResponseTime(Medicine *med) {
    if (med == NULL || med->warning_time == 0 || med->response_time == 0) {
        return 0.0;  // 无预警记录或未解除，返回0
    }

    // 时间差（秒）转换为小时（保留1位小数）
    double response_second = difftime(med->response_time, med->warning_time);
    return round(response_second / 3600.0 * 10) / 10;
}

// 6. 自动触发预警检查（整合“动态阈值更新+预警状态监控”）
void autoCheckWarnings(OrderedList *inventory, const char *current_date) {
    if (inventory == NULL || current_date == NULL) {
        cout << "错误：库存表为空或日期非法，无法执行自动预警检查！" << endl;
        return;
    }

    cout << "\n======================================================" << endl;
    cout << "自动预警检查开始（当前日期：" << current_date << "）" << endl;
    cout << "======================================================" << endl;

    // 步骤1：为所有药品更新动态阈值
    cout << "\n步骤1：更新所有药品动态阈值..." << endl;
    for (int i = 0; i < inventory->length; i++) {
        setDynamicWarningThreshold(&inventory->medicines[i], current_date);
    }

    // 步骤2：检查并更新所有药品预警状态
    cout << "\n步骤2：检查并更新预警状态..." << endl;
    updateAllWarnings(inventory);

    cout << "\n======================================================" << endl;
    cout << "自动预警检查完成" << endl;
    cout << "======================================================" << endl;
}
