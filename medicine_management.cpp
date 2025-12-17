#include <iostream>
#include <cstring>
#include <conio.h>  
#include "ordered_list.h"  

using namespace std;

// -------------------------- 数据验证实现 --------------------------
// 验证药品ID：非负 + 未重复
int validateMedicineID(OrderedList *list, int id) {
    if (id <= 0) {
        cout << "错误：药品ID必须为正整数！" << endl;
        return -1;
    }
    if (findMedicine(list, id) != NULL) {
        cout << "错误：药品ID=" << id << "已存在，请勿重复添加！" << endl;
        return -2;
    }
    return 0; // 验证通过
}

// 验证药品信息：名称/产地/规格非空，库存量/阈值非负
int validateMedicineInfo(Medicine *med) {
    if (strlen(med->name) == 0 || strlen(med->name) >= MAX_NAME_LEN) {
        cout << "错误：药品名称不能为空且长度不能超过" << MAX_NAME_LEN-1 << "字符！" << endl;
        return -1;
    }
    if (strlen(med->origin) == 0 || strlen(med->origin) >= MAX_ORIGIN_LEN) {
        cout << "错误：产地不能为空且长度不能超过" << MAX_ORIGIN_LEN-1 << "字符！" << endl;
        return -2;
    }
    if (strlen(med->spec) == 0 || strlen(med->spec) >= MAX_SPEC_LEN) {
        cout << "错误：规格不能为空且长度不能超过" << MAX_SPEC_LEN-1 << "字符！" << endl;
        return -3;
    }
    if (med->stock < 0) {
        cout << "错误：库存量不能为负数！" << endl;
        return -4;
    }
    if (med->warning_threshold < 0) {
        cout << "错误：预警阈值不能为负数！" << endl;
        return -5;
    }
    return 0; // 验证通过
}

// -------------------------- 新增药品实现 --------------------------
int addMedicine(OrderedList *list) {
    if (list == NULL || list->length >= list->capacity) {
        cout << "错误：库存表已满，无法新增药品！" << endl;
        return -1;
    }

    Medicine med = {0};
    cout << "\n===== 新增药品 =====" << endl;

    // 输入并验证ID
    while (true) {
        cout << "请输入药品ID（正整数）：";
        cin >> med.id;
        if (validateMedicineID(list, med.id) == 0) {
            break; // ID验证通过
        }
        cout << "请重新输入！" << endl;
    }

    // 输入基本信息
    cout << "请输入药品名称：";
    cin >> med.name;
    cout << "请输入药品产地：";
    cin >> med.origin;
    cout << "请输入药品规格：";
    cin >> med.spec;
    cout << "请输入初始库存量（g）：";
    cin >> med.stock;
    cout << "请输入初始预警阈值（g）：";
    cin >> med.warning_threshold;

    // 初始化其他字段
    med.last_usage = 0;
    memset(med.usage_history, 0, sizeof(med.usage_history));
    med.is_warning = 0;
    med.warning_time = 0;
    med.response_time = 0;

    // 验证基本信息合法性
    if (validateMedicineInfo(&med) != 0) {
        cout << "新增失败：药品信息不合法！" << endl;
        return -2;
    }

    // 插入有序表
    if (insertMedicine(list, med) == 0) {
        cout << "新增成功！药品名称：" << med.name << "（ID:" << med.id << "）" << endl;
        return 0;
    } else {
        cout << "新增失败：未知错误！" << endl;
        return -3;
    }
}

// -------------------------- 修改药品实现 --------------------------
int modifyMedicine(OrderedList *list) {
    if (list == NULL || list->length == 0) {
        cout << "错误：库存表为空，无药品可修改！" << endl;
        return -1;
    }

    int id;
    cout << "\n===== 修改药品 =====" << endl;
    cout << "请输入要修改的药品ID：";
    cin >> id;

    // 查找药品
    Medicine *med = findMedicine(list, id);
    if (med == NULL) {
        cout << "错误：未找到ID=" << id << "的药品！" << endl;
        return -2;
    }

    // 显示当前信息
    cout << "\n当前药品信息：" << endl;
    cout << "ID：" << med->id << "（不可修改）" << endl;
    cout << "名称：" << med->name << endl;
    cout << "产地：" << med->origin << endl;
    cout << "规格：" << med->spec << endl;
    cout << "库存量：" << med->stock << "g" << endl;
    cout << "预警阈值：" << med->warning_threshold << "g" << endl;

    // 输入新信息（支持回车保留原信息）
    cout << "\n请输入新信息（直接回车保留原信息）：" << endl;
    cin.ignore(); // 清除缓冲区

    char input[MAX_NAME_LEN];
    // 修改名称
    cout << "名称（原：" << med->name << "）：";
    cin.getline(input, MAX_NAME_LEN);
    if (strlen(input) > 0) {
        strncpy(med->name, input, MAX_NAME_LEN-1);
        med->name[MAX_NAME_LEN-1] = '\0';
    }

    // 修改产地
    cout << "产地（原：" << med->origin << "）：";
    cin.getline(input, MAX_ORIGIN_LEN);
    if (strlen(input) > 0) {
        strncpy(med->origin, input, MAX_ORIGIN_LEN-1);
        med->origin[MAX_ORIGIN_LEN-1] = '\0';
    }

    // 修改规格
    cout << "规格（原：" << med->spec << "）：";
    cin.getline(input, MAX_SPEC_LEN);
    if (strlen(input) > 0) {
        strncpy(med->spec, input, MAX_SPEC_LEN-1);
        med->spec[MAX_SPEC_LEN-1] = '\0';
    }

    // 修改库存量
    cout << "库存量（原：" << med->stock << "g）：";
    cin.getline(input, 20);
    if (strlen(input) > 0) {
        int new_stock = atoi(input);
        if (new_stock >= 0) {
            med->stock = new_stock;
        } else {
            cout << "警告：库存量不能为负，保留原值！" << endl;
        }
    }

    // 修改预警阈值
    cout << "预警阈值（原：" << med->warning_threshold << "g）：";
    cin.getline(input, 20);
    if (strlen(input) > 0) {
        int new_threshold = atoi(input);
        if (new_threshold >= 0) {
            med->warning_threshold = new_threshold;
        } else {
            cout << "警告：预警阈值不能为负，保留原值！" << endl;
        }
    }

    // 验证修改后的信息
    if (validateMedicineInfo(med) != 0) {
        cout << "修改失败：部分信息不合法，已自动保留原合法值！" << endl;
        return -3;
    }

    cout << "修改成功！" << endl;
    return 0;
}

// -------------------------- 删除药品实现 --------------------------
int removeMedicine(OrderedList *list) {
    if (list == NULL || list->length == 0) {
        cout << "错误：库存表为空，无药品可删除！" << endl;
        return -1;
    }

    int id;
    cout << "\n===== 删除药品 =====" << endl;
    cout << "请输入要删除的药品ID：";
    cin >> id;

    // 查找药品
    Medicine *med = findMedicine(list, id);
    if (med == NULL) {
        cout << "错误：未找到ID=" << id << "的药品！" << endl;
        return -2;
    }

    // 二次确认
    cout << "确认要删除药品：" << med->name << "（ID:" << id << "）吗？（Y/N）：";
    char confirm = _getch(); // 无回显输入（Windows），Linux用getch()
    cout << endl;

    if (confirm == 'Y' || confirm == 'y') {
        if (deleteMedicine(list, id) == 0) {
            cout << "删除成功！" << endl;
            return 0;
        } else {
            cout << "删除失败：未知错误！" << endl;
            return -3;
        }
    } else {
        cout << "已取消删除！" << endl;
        return -4;
    }
}

// -------------------------- 查询药品实现（ID+名称模糊查询） --------------------------
void queryMedicine(OrderedList *list) {
    if (list == NULL || list->length == 0) {
        cout << "错误：库存表为空，无药品可查询！" << endl;
        return;
    }

    int choice;
    cout << "\n===== 查询药品 =====" << endl;
    cout << "1. 按ID精确查询" << endl;
    cout << "2. 按名称模糊查询" << endl;
    cout << "请选择查询方式：";
    cin >> choice;

    switch (choice) {
        case 1: { // 按ID精确查询
            int id;
            cout << "请输入药品ID：";
            cin >> id;
            Medicine *med = findMedicine(list, id);
            if (med != NULL) {
                cout << "\n查询结果：" << endl;
                cout << "ID：" << med->id << endl;
                cout << "名称：" << med->name << endl;
                cout << "产地：" << med->origin << endl;
                cout << "规格：" << med->spec << endl;
                cout << "库存量：" << med->stock << "g" << endl;
                cout << "预警阈值：" << med->warning_threshold << "g" << endl;
                cout << "预警状态：" << (med->is_warning ? "预警中" : "正常") << endl;
            } else {
                cout << "查询结果：未找到ID=" << id << "的药品！" << endl;
            }
            break;
        }

        case 2: { // 按名称模糊查询
            char keyword[MAX_NAME_LEN];
            cout << "请输入药品名称关键字：";
            cin >> keyword;

            cout << "\n查询结果（名称包含\"" << keyword << "\"的药品）：" << endl;
            cout << "ID\t名称\t\t产地\t\t规格\t\t库存量\t预警状态" << endl;
            cout << "------------------------------------------------------------" << endl;

            int count = 0;
            for (int i = 0; i < list->length; i++) {
                Medicine med = list->medicines[i];
                // 模糊匹配（包含关键字）
                if (strstr(med.name, keyword) != NULL) {
                    printf("%d\t%s\t\t%s\t\t%s\t\t%d\t%s\n",
                           med.id, med.name, med.origin, med.spec,
                           med.stock, med.is_warning ? "预警中" : "正常");
                    count++;
                }
            }

            if (count == 0) {
                cout << "无匹配结果！" << endl;
            } else {
                cout << "共找到" << count << "个匹配药品！" << endl;
            }
            break;
        }

        default:
            cout << "错误：无效查询方式！" << endl;
            break;
    }
}

// -------------------------- 显示所有药品实现 --------------------------
void showAllMedicines(OrderedList *list) {
    if (list == NULL || list->length == 0) {
        cout << "库存表为空！" << endl;
        return;
    }

    cout << "\n===== 所有药品信息（按ID升序） =====" << endl;
    cout << "ID\t名称\t\t产地\t\t规格\t\t库存量（g）\t预警阈值（g）\t预警状态" << endl;
    cout << "------------------------------------------------------------------------" << endl;

    for (int i = 0; i < list->length; i++) {
        Medicine med = list->medicines[i];
        printf("%d\t%s\t\t%s\t\t%s\t\t%d\t\t%d\t\t%s\n",
               med.id, med.name, med.origin, med.spec,
               med.stock, med.warning_threshold,
               med.is_warning ? "预警中" : "正常");
    }

    cout << "\n合计：" << list->length << "种药品" << endl;
}

// -------------------------- 药品管理主菜单实现 --------------------------
void medicineManagementMenu(OrderedList *list) {
    int choice;
    while (true) {
        system("cls"); // Windows清屏，Linux/macOS替换为"clear"
        cout << "==================== 药品管理模块 ====================" << endl;
        cout << "1. 新增药品" << endl;
        cout << "2. 修改药品信息" << endl;
        cout << "3. 删除药品" << endl;
        cout << "4. 查询药品" << endl;
        cout << "5. 显示所有药品" << endl;
        cout << "0. 返回主菜单" << endl;
        cout << "======================================================" << endl;
        cout << "请选择操作（0-5）：";
        cin >> choice;

        switch (choice) {
            case 1:
                addMedicine(list);
                break;
            case 2:
                modifyMedicine(list);
                break;
            case 3:
                removeMedicine(list);
                break;
            case 4:
                queryMedicine(list);
                break;
            case 5:
                showAllMedicines(list);
                break;
            case 0:
                cout << "返回主菜单！" << endl;
                return;
            default:
                cout << "错误：无效操作，请重新选择！" << endl;
                break;
        }

        cout << "\n按任意键继续..." << endl;
        _getch(); // 暂停等待用户操作（Windows），Linux用getch()
    }
}
