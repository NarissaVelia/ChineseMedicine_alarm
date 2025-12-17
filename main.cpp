#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <conio.h>
#include "ordered_list.h"
#include "medicine.h"
#include "medicine_management.h"
#include "warning_logic.h"

using namespace std;

// -------------------------- 函数声明（关键：按“调用顺序”提前声明） --------------------------
// 1. 先声明被早期调用的函数
void systemMainMenu(OrderedList *inventory);
void testDay4Functions(OrderedList *list);
void testDay2Functions(OrderedList *list); // 关键：将testDay2Functions声明移到这里
int loadMedicineFromCSV(OrderedList *list, const char *filename);
int loadUsageHistoryFromCSV(OrderedList *list, const char *filename);

// -------------------------- 补全：loadMedicineFromCSV 函数实现（仅1份） --------------------------
int loadMedicineFromCSV(OrderedList *list, const char *filename) {
    // 你的原有实现（不变）
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "错误：无法打开CSV文件 " << filename << endl;
        return -1;
    }
    string line;
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Medicine med = {0};
        int col_idx = 0;
        while (getline(ss, token, ',')) {
            switch (col_idx) {
                case 0: med.id = atoi(token.c_str()); break;
                case 1: strncpy(med.name, token.c_str(), MAX_NAME_LEN-1); med.name[MAX_NAME_LEN-1] = '\0'; break;
                case 2: strncpy(med.origin, token.c_str(), MAX_ORIGIN_LEN-1); med.origin[MAX_ORIGIN_LEN-1] = '\0'; break;
                case 3: strncpy(med.spec, token.c_str(), MAX_SPEC_LEN-1); med.spec[MAX_SPEC_LEN-1] = '\0'; break;
                case 4: med.stock = atoi(token.c_str()); break;
                case 5: med.warning_threshold = atoi(token.c_str()); break;
                default: break;
            }
            col_idx++;
        }
        med.last_usage = 0;
        memset(med.usage_history, 0, sizeof(med.usage_history));
        med.is_warning = 0;
        med.warning_time = 0;
        med.response_time = 0;
        if (insertMedicine(list, med) != 0) {
            cout << "警告：药品 " << med.name << "（ID:" << med.id << "）插入失败（可能重复）" << endl;
        }
    }
    file.close();
    cout << "药物基本信息加载完成，当前库存表长度：" << list->length << endl;
    return 0;
}

// -------------------------- loadUsageHistoryFromCSV 函数实现（仅1份，无重复） --------------------------
int loadUsageHistoryFromCSV(OrderedList *list, const char *filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "错误：无法打开近7日用量文件 " << filename << endl;
        return -1;
    }

    string line;
    getline(file, line);
    cout << "表头验证通过：" << line << endl;
    cout << "药品ID映射规则：第1列→ID=0，第2列→ID=1，第3列→ID=2，第4列→ID=3，第5列→ID=4" << endl;

    while (getline(file, line)) {
        stringstream data_ss(line);
        string data_col;
        int col_idx = 0;
        int day_index = -1;  
        int day_offset = -1;  // 关键修复：将 day_offset 定义在这里（if 外部），扩大作用域
        
        while (getline(data_ss, data_col, ',')) {
            if (col_idx == 0) {
                if (data_col.substr(0, 2) == "D-") {
                    // 直接给外部定义的 day_offset 赋值，不再重新定义
                    day_offset = atoi(data_col.substr(2).c_str());  
                    if (day_offset >= 0 && day_offset <= 6) {
                        day_index = 6 - day_offset;
                        cout << "\n解析日期：" << data_col << " → 存储到usage_history[" << day_index << "]" << endl;
                    } else {
                        cout << "警告：日期" << data_col << "非法（需为D-0~D-6），跳过该行！" << endl;
                    }
                } else {
                    cout << "警告：日期" << data_col << "格式非法（需以D-开头），跳过该行！" << endl;
                }
                col_idx++;
                continue;
            }

            // 后续用量赋值逻辑（不变）
            if (day_index < 0) {
                col_idx++;
                continue;
            }
            int med_id = col_idx - 1;
            if (med_id < 0 || med_id > 4) {
                col_idx++;
                continue;
            }

            Medicine *med = findMedicine(list, med_id);
            if (med != NULL) {
                int usage = atoi(data_col.c_str());
                med->usage_history[day_index] = usage;
                if (day_index == 6) {
                    med->last_usage = usage;
                    cout << "药品ID=" << med_id << "（" << med->name << "）：D-0用量=" << usage << "，更新last_usage=" << med->last_usage << endl;
                } else {
                    cout << "药品ID=" << med_id << "（" << med->name << "）：" << data_col << " → 存储到索引[" << day_index << "]" << endl;
                }
            } else {
                cout << "警告：未找到药品ID=" << med_id << "，跳过用量" << data_col << endl;
            }

            col_idx++;
        }
    }

    file.close();
    cout << "\n近7日用量数据加载完成！" << endl;
    return 0;
}

// -------------------------- Day4 预警功能测试菜单 --------------------------
void testDay4Functions(OrderedList *list) {
    char current_date[11];
    cout << "\n===== Day4 预警计算逻辑测试 =====" << endl;
    cout << "请输入当前日期（格式：YYYY-MM-DD）：";
    cin >> current_date;

    while (true) {
        system("cls");
        cout << "===== 预警功能测试（当前日期：" << current_date << "）=====" << endl;
        cout << "1. 加载近7日用量数据" << endl;
        cout << "2. 计算单个药品前三日均用量" << endl;
        cout << "3. 设置基础预警阈值" << endl;
        cout << "4. 设置动态预警阈值（含季节/波动系数）" << endl;
        cout << "5. 执行实时预警监控" << endl;
        cout << "6. 显示所有药品预警状态" << endl;
        cout << "0. 返回主菜单" << endl;
        cout << "请选择操作：";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                const char *usage_csv = "D:\\数据结构课程设计\\近7日用量.csv";
                loadUsageHistoryFromCSV(list, usage_csv);
                break;
            }

            case 2: {
                int id;
                cout << "请输入药品ID：";
                cin >> id;
                Medicine *med = findMedicine(list, id);
                if (med != NULL) {
                    int avg = calculateThreeDayAverage(med);
                    cout << "药品：" << med->name << "，前三日均用量：" << avg << "g" << endl;
                } else {
                    cout << "未找到该药品！" << endl;
                }
                break;
            }

            case 3: {
                int id;
                cout << "请输入药品ID：";
                cin >> id;
                Medicine *med = findMedicine(list, id);
                if (med != NULL) {
                    int old_threshold = med->warning_threshold;
                    setWarningThreshold(med);
                    cout << "药品：" << med->name << endl;
                    cout << "原基础阈值：" << old_threshold << "g" << endl;
                    cout << "更新后基础阈值：" << med->warning_threshold << "g" << endl;
                } else {
                    cout << "未找到该药品！" << endl;
                }
                break;
            }

            case 4: {
                int id;
                cout << "请输入药品ID：";
                cin >> id;
                Medicine *med = findMedicine(list, id);
                if (med != NULL) {
                    int old_threshold = med->warning_threshold;
                    setDynamicWarningThreshold(med, current_date);
                    cout << "药品：" << med->name << endl;
                    cout << "原阈值：" << old_threshold << "g" << endl;
                    cout << "动态更新后阈值：" << med->warning_threshold << "g" << endl;
                } else {
                    cout << "未找到该药品！" << endl;
                }
                break;
            }

            case 5: {
                autoCheckWarnings(list, current_date);
                break;
            }

            case 6: {
                cout << "\n===== 所有药品预警状态 =====" << endl;
                cout << "ID\t名称\t\t库存量(g)\t预警阈值(g)\t预警状态" << endl;
                cout << "--------------------------------------------------------" << endl;
                for (int i = 0; i < list->length; i++) {
                    Medicine med = list->medicines[i];
                    printf("%d\t%s\t\t%d\t\t%d\t\t%s\n",
                           med.id, med.name, med.stock, med.warning_threshold,
                           med.is_warning ? "??  预警中" : "? 正常");
                }
                break;
            }

            case 0:
                return;

            default:
                cout << "无效操作，请重新选择！" << endl;
                break;
        }

        cout << "\n按任意键继续..." << endl;
        _getch();
    }
}

// -------------------------- 系统主菜单（扩展Day4功能入口） --------------------------
void systemMainMenu(OrderedList *inventory) {
    int choice;
    while (true) {
        system("cls");
        cout << "==================== 中药库存预警系统 ====================" << endl;
        cout << "1. 药品管理模块（Day3功能）" << endl;
        cout << "2. 有序表高级功能测试（Day2功能）" << endl;
        cout << "3. 预警计算逻辑测试（Day4功能）" << endl;
        cout << "4. 退出系统" << endl;
        cout << "========================================================" << endl;
        cout << "请选择操作（1-4）：";
        cin >> choice;
        switch (choice) {
            case 1: medicineManagementMenu(inventory); break;
            case 2: testDay2Functions(inventory); // 此时已声明，编译器可识别
                    cout << "按任意键返回主菜单..." << endl;
                    _getch();
                    break;
            case 3: testDay4Functions(inventory); break;
            case 4: destroyOrderedList(inventory); exit(0);
            default: cout << "无效操作！" << endl; _getch(); break;
        }
    }
}

// -------------------------- 主函数（整合所有功能） --------------------------
int main() {
    // 1. 创建库存表（容量300）
    OrderedList *inventory = createOrderedList(MAX_MEDICINES);
    if (inventory == NULL) {
        cout << "错误：创建库存表失败！" << endl;
        return -1;
    }

    // 2. 加载药品基本信息（药物基本信息.csv）
    const char *med_csv = "D:\\数据结构课程设计\\药物基本信息.csv";
    if (loadMedicineFromCSV(inventory, med_csv) != 0) {
        destroyOrderedList(inventory);
        return -1;
    }

    // 3. 进入系统主菜单
    systemMainMenu(inventory);

    return 0;
}

// -------------------------- 复用Day2测试函数（无修改） --------------------------
void testDay2Functions(OrderedList *list) {
    int choice, id, count;
    Medicine *found_med = NULL;
    const char *save_path = "D:\\数据结构课程设计\\inventory_data.bin";

    while (true) {
        system("cls");
        cout << "===== Day2 有序顺序表高级功能测试 =====" << endl;
        cout << "1. 二分查找药品（按ID）" << endl;
        cout << "2. 批量插入药品" << endl;
        cout << "3. 批量删除药品" << endl;
        cout << "4. 保存库存表到文件" << endl;
        cout << "5. 从文件加载库存表" << endl;
        cout << "6. 显示当前库存表" << endl;
        cout << "0. 返回主菜单" << endl;
        cout << "请选择操作：";
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << "请输入要查找的药品ID："; cin >> id;
                found_med = binarySearchMedicine(list, id);
                if (found_med != NULL) {
                    cout << "二分查找成功：" << endl;
                    cout << "ID：" << found_med->id << " | 名称：" << found_med->name 
                         << " | 库存：" << found_med->stock << "g" << endl;
                } else {
                    cout << "未找到ID=" << id << "的药品！" << endl;
                }
                break;
            }
            case 2: {
                cout << "请输入批量插入数量："; cin >> count;
                if (count <= 0 || count > list->capacity - list->length) {
                    cout << "插入数量非法！" << endl;
                    break;
                }
                Medicine *med_arr = (Medicine*)malloc(sizeof(Medicine) * count);
                for (int i = 0; i < count; i++) {
                    cout << "第" << i+1 << "个药品：" << endl;
                    cout << "ID："; cin >> med_arr[i].id;
                    cout << "名称："; cin >> med_arr[i].name;
                    cout << "库存："; cin >> med_arr[i].stock;
                    med_arr[i].warning_threshold = 100;
                    memset(med_arr[i].origin, 0, MAX_ORIGIN_LEN);
                    memset(med_arr[i].spec, 0, MAX_SPEC_LEN);
                    med_arr[i].is_warning = 0;
                }
                int success = batchInsertMedicine(list, med_arr, count);
                cout << "批量插入成功" << success << "个！" << endl;
                free(med_arr);
                break;
            }
            case 3: {
                cout << "请输入批量删除ID数量："; cin >> count;
                if (count <= 0) {
                    cout << "数量非法！" << endl;
                    break;
                }
                int *id_arr = (int*)malloc(sizeof(int) * count);
                for (int i = 0; i < count; i++) {
                    cout << "第" << i+1 << "个ID："; cin >> id_arr[i];
                }
                int success = batchDeleteMedicine(list, id_arr, count);
                cout << "批量删除成功" << success << "个！" << endl;
                free(id_arr);
                break;
            }
            case 4: {
                int ret = saveToFile(list, save_path);
                cout << (ret == 0 ? "保存成功！" : "保存失败！") << endl;
                break;
            }
            case 5: {
                OrderedList *loaded = loadFromFile(save_path);
                if (loaded != NULL) {
                    destroyOrderedList(list);
                    list = loaded;
                    cout << "加载成功！当前长度：" << list->length << endl;
                } else {
                    cout << "加载失败！" << endl;
                }
                break;
            }
            case 6: {
                showAllMedicines(list);
                break;
            }
            case 0:
                return;
            default:
                cout << "无效操作！" << endl;
                break;
        }

        cout << "按任意键继续..." << endl;
        _getch();
    }
}
