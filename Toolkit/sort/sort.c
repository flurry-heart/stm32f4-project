#include "sort.h"

/**
 * @brief 冒泡排序函数
 * 
 * @param data 待排序的数组
 * @param length 待排序数组的长度
 */
void BubbleSort(int data[], int length)
{
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < length - 1 - i; j++)
        {
            if (data[j] > data[j + 1])
            {
                int temp = data[j + 1];
                data[j + 1] = data[j];
                data[j] = temp;
            }
        }
    }
}