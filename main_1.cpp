#include <iostream>
#include <fstream>
#include <sstream>
#include "ordered_list.h"
#include "medicine.h"

using namespace std;

// 从CSV文件加载药品基本信息（文件路径：D:\数据结构课程设计\药物基本信息.csv）
int loadMedicineFromCSV(OrderedList *list, const char *filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "错误：无法打开文件 " << filename << endl;
        return -1;
    }

    string line;
    getline(file, line); // 跳过表头行（如果CSV有表头的话）

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Medicine med = {0}; // 初始化药品信息
        int col_idx = 0;    // 记录当前解析的列索引

        // 关键修正：CSV实际是逗号分隔，所以用','作为分隔符
        while (getline(ss, token, ',')) {
            switch (col_idx) {
                case 0: // 第1列：药品ID
                    med.id = atoi(token.c_str());
                    break;
                case 1: // 第2列：药品名称
                    strncpy(med.name, token.c_str(), MAX_NAME_LEN-1);
                    med.name[MAX_NAME_LEN-1] = '\0'; // 确保字符串结束符
                    break;
                case 2: // 第3列：产地
                    strncpy(med.origin, token.c_str(), MAX_ORIGIN_LEN-1);
                    med.origin[MAX_ORIGIN_LEN-1] = '\0';
                    break;
                case 3: // 第4列：规格
                    strncpy(med.spec, token.c_str(), MAX_SPEC_LEN-1);
                    med.spec[MAX_SPEC_LEN-1] = '\0';
                    break;
                case 4: // 第5列：库存量
                    med.stock = atoi(token.c_str());
                    break;
                case 5: // 第6列：预警阈值
                    med.warning_threshold = atoi(token.c_str());
                    break;
                default: // 多余列直接忽略
                    break;
            }
            col_idx++;
        }

        // 初始化其他字段
        med.last_usage = 0;
        memset(med.usage_history, 0, sizeof(med.usage_history));
        med.is_warning = 0;
        med.warning_time = 0;
        med.response_time = 0;

        // 插入有序顺序表
        int ret = insertMedicine(list, med);
        if (ret == -1) {
            cout << "警告：列表已满，无法插入药品 " << med.name << endl;
        } else if (ret == -2) {
            cout << "警告：药品 " << med.name << "（ID:" << med.id << "）已存在，跳过插入" << endl;
        } else {
            cout << "成功插入药品：" << med.name << "（ID:" << med.id << "）" << endl;
        }
    }

    file.close();
    cout << "\nCSV文件加载完成，当前库存表长度：" << list->length << endl;
    return 0;
}

// 测试Day1功能菜单
void testDay1Functions(OrderedList *list) {
    int choice, id;
    Medicine med = {0};
    Medicine *found_med = NULL;

    while (true) {
        cout << "\n===== Day1 有序顺序表功能测试 =====" << endl;
        cout << "1. 插入药品" << endl;
        cout << "2. 按ID删除药品" << endl;
        cout << "3. 按ID查找药品" << endl;
        cout << "4. 显示所有药品" << endl;
        cout << "0. 退出测试" << endl;
        cout << "请选择操作：";
        cin >> choice;

        switch (choice) {
            case 1: { // 加{}，独立作用域
                // 手动输入药品信息插入
                cout << "请输入药品ID："; cin >> med.id;
                cout << "请输入药品名称："; cin >> med.name;
                cout << "请输入药品产地："; cin >> med.origin;
                cout << "请输入药品规格："; cin >> med.spec;
                cout << "请输入库存量："; cin >> med.stock;
                cout << "请输入预警阈值："; cin >> med.warning_threshold;

                med.last_usage = 0;
                memset(med.usage_history, 0, sizeof(med.usage_history));
                med.is_warning = 0;

                int ret = insertMedicine(list, med); // 变量在case1内部定义
                if (ret == 0) cout << "插入成功！" << endl;
                else if (ret == -1) cout << "插入失败：列表已满！" << endl;
                else if (ret == -2) cout << "插入失败：药品ID已存在！" << endl;
                break;
            } // case1结束

            case 2: { // 加{}，独立作用域
                cout << "请输入要删除的药品ID："; cin >> id;
                int ret = deleteMedicine(list, id); // 变量在case2内部定义
                if (ret == 0) cout << "删除成功！" << endl;
                else if (ret == -1) cout << "删除失败：列表为空！" << endl;
                else if (ret == -2) cout << "删除失败：未找到该药品！" << endl;
                break;
            } // case2结束

            case 3: { // 加{}，独立作用域
                cout << "请输入要查找的药品ID："; cin >> id;
                found_med = findMedicine(list, id);
                if (found_med != NULL) {
                    cout << "查找成功：" << endl;
                    cout << "ID：" << found_med->id << endl;
                    cout << "名称：" << found_med->name << endl;
                    cout << "产地：" << found_med->origin << endl;
                    cout << "规格：" << found_med->spec << endl;
                    cout << "库存量：" << found_med->stock << endl;
                } else {
                    cout << "查找失败：未找到该药品！" << endl;
                }
                break;
            } // case3结束

            case 4: { // 加{}，独立作用域
                cout << "当前库存表所有药品（按ID升序）：" << endl;
                for (int i = 0; i < list->length; i++) {
                    med = list->medicines[i];
                    cout << "ID:" << med.id << " | 名称:" << med.name 
                         << " | 产地:" << med.origin << " | 库存:" << med.stock << endl;
                }
                break;
            } // case4结束

            case 0:
                cout << "退出测试！" << endl;
                return;

            default:
                cout << "无效操作，请重新选择！" << endl;
                break;
        }
    }
}

int main() {
    // 1. 创建有序顺序表（容量300，符合题目要求）
    OrderedList *inventory = createOrderedList(MAX_MEDICINES);
    if (inventory == NULL) {
        cout << "错误：创建库存表失败！" << endl;
        return -1;
    }

    // 2. 从CSV文件加载药品数据
    const char *csv_path = "D:\\数据结构课程设计\\药物基本信息.csv";
    int load_ret = loadMedicineFromCSV(inventory, csv_path);
    if (load_ret != 0) {
        destroyOrderedList(inventory);
        return -1;
    }

    // 3. 测试Day1功能
    testDay1Functions(inventory);

    // 4. 销毁资源
    destroyOrderedList(inventory);
    return 0;
}
