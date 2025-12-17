#include <cstdio>  
#include "ordered_list.h"
#include <cstdlib>
#include <cstring>

// 创建有序顺序表（初始化容量）
OrderedList* createOrderedList(int capacity) {
    if (capacity <= 0 || capacity > MAX_MEDICINES) {
        return NULL; // 容量非法
    }
    OrderedList *list = (OrderedList*)malloc(sizeof(OrderedList));
    list->medicines = (Medicine*)malloc(sizeof(Medicine) * capacity);
    list->length = 0;
    list->capacity = capacity;
    return list;
}

// 销毁有序顺序表（释放内存）
void destroyOrderedList(OrderedList *list) {
    if (list != NULL) {
        if (list->medicines != NULL) {
            free(list->medicines);
            list->medicines = NULL;
        }
        free(list);
        list = NULL;
    }
}

// 插入药品（保持按ID升序有序）
int insertMedicine(OrderedList *list, Medicine med) {
    // 参数校验
    if (list == NULL || list->length >= list->capacity) {
        return -1; // 列表为空或已满
    }
    if (findMedicine(list, med.id) != NULL) {
        return -2; // 药品ID已存在，不允许重复插入
    }

    // 找到插入位置（按ID升序）
    int insert_idx = 0;
    while (insert_idx < list->length && list->medicines[insert_idx].id < med.id) {
        insert_idx++;
    }

    // 元素后移，腾出插入位置
    for (int i = list->length; i > insert_idx; i--) {
        memcpy(&list->medicines[i], &list->medicines[i-1], sizeof(Medicine));
    }

    // 插入新药品
    memcpy(&list->medicines[insert_idx], &med, sizeof(Medicine));
    list->length++;
    return 0; // 插入成功
}

// 按ID删除药品
int deleteMedicine(OrderedList *list, int id) {
    // 参数校验
    if (list == NULL || list->length == 0) {
        return -1; // 列表为空
    }

    // 找到要删除的索引
    int delete_idx = -1;
    for (int i = 0; i < list->length; i++) {
        if (list->medicines[i].id == id) {
            delete_idx = i;
            break;
        }
    }
    if (delete_idx == -1) {
        return -2; // 未找到该药品
    }

    // 元素前移，覆盖删除位置
    for (int i = delete_idx; i < list->length - 1; i++) {
        memcpy(&list->medicines[i], &list->medicines[i+1], sizeof(Medicine));
    }

    list->length--;
    return 0; // 删除成功
}

// 2.1 二分查找：按ID查找药品，返回药品指针（不存在返回NULL）
Medicine* binarySearchMedicine(OrderedList *list, int id) {
    if (list == NULL || list->length == 0) return NULL;

    int low = 0, high = list->length - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;  // 优化：避免low+high溢出
        if (list->medicines[mid].id == id) {
            return &list->medicines[mid];  // 找到，返回指针
        } else if (list->medicines[mid].id < id) {
            low = mid + 1;  // 目标在右半区
        } else {
            high = mid - 1;  // 目标在左半区
        }
    }
    return NULL;  // 未找到
}

// 替换原有findMedicine为二分查找（保持接口兼容性）
Medicine* findMedicine(OrderedList *list, int id) {
    return binarySearchMedicine(list, id);
}

// 2.2 批量插入药品：传入药品数组和数量，返回成功插入个数
int batchInsertMedicine(OrderedList *list, Medicine *med_arr, int med_count) {
    if (list == NULL || med_arr == NULL || med_count <= 0) return 0;

    int success_count = 0;
    for (int i = 0; i < med_count; i++) {
        // 调用单个插入接口，成功则计数+1
        if (insertMedicine(list, med_arr[i]) == 0) {
            success_count++;
        }
    }
    return success_count;
}

// 2.3 批量删除药品：传入ID数组和数量，返回成功删除个数
int batchDeleteMedicine(OrderedList *list, int *id_arr, int id_count) {
    if (list == NULL || id_arr == NULL || id_count <= 0) return 0;

    int success_count = 0;
    for (int i = 0; i < id_count; i++) {
        // 调用单个删除接口，成功则计数+1
        if (deleteMedicine(list, id_arr[i]) == 0) {
            success_count++;
        }
    }
    return success_count;
}

// 2.4 保存库存表到文件（二进制格式，确保数据完整性）
int saveToFile(OrderedList *list, const char *filename) {
    if (list == NULL || filename == NULL) return -1;

    // 打开文件（二进制写入模式，不存在则创建，存在则覆盖）
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) return -2;  // 文件打开失败

    // 先写入顺序表的核心信息（容量、当前长度）
    fwrite(&list->capacity, sizeof(int), 1, fp);
    fwrite(&list->length, sizeof(int), 1, fp);

    // 再写入所有药品数据
    if (list->length > 0) {
        fwrite(list->medicines, sizeof(Medicine), list->length, fp);
    }

    fclose(fp);
    return 0;  // 保存成功
}

// 2.5 从文件加载库存表（返回新创建的有序表，失败返回NULL）
OrderedList* loadFromFile(const char *filename) {
    if (filename == NULL) return NULL;

    // 打开文件（二进制读取模式）
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) return NULL;  // 文件不存在或打开失败

    // 先读取容量和长度
    int capacity, length;
    fread(&capacity, sizeof(int), 1, fp);
    fread(&length, sizeof(int), 1, fp);

    // 校验容量合法性（不超过最大药品数）
    if (capacity <= 0 || capacity > MAX_MEDICINES || length < 0 || length > capacity) {
        fclose(fp);
        return NULL;  // 数据非法
    }

    // 创建新的有序表
    OrderedList *list = createOrderedList(capacity);
    if (list == NULL) {
        fclose(fp);
        return NULL;
    }

    // 读取药品数据到新表
    list->length = length;
    if (length > 0) {
        fread(list->medicines, sizeof(Medicine), length, fp);
    }

    fclose(fp);
    return list;  // 加载成功
}
