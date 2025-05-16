#include "geometry.h"

 /**
  * @brief 判断指定点是否在指定多边形内部
  * 
  * @param nVertex 多边形的顶点数
  * @param vertexX 多边形的顶点x坐标集合
  * @param vertexY 多边形的顶点y坐标集合
  * @param x 指定点的x坐标
  * @param y 指定点的y坐标
  * @return uint8_t 0: 不在多边形内部; 1: 在多边形内部;
  */
uint8_t pnpoly(uint8_t nVertex, int *vertexX, int *vertexY, int x, int y) 
{
    int i = 0, j = 0;
    int c = 0;

    for (i = 0, j = nVertex-1; i < nVertex; j = i++) 
    {
        if (((vertexY[i] > y) != (vertexY[j] > y)) &&
            (x < (vertexX[j] - vertexX[i]) * (y - vertexY[i]) / (vertexY[j] - vertexY[i]) + vertexX[i])) {
            c = !c;
        }
    }
    return c;
}
