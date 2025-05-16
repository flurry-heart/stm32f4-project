#ifndef __FONT_H__
#define __FONT_H__

typedef struct ChineseCell_12x12
{
    char index[4];
    unsigned char data[24];
} ChineseCell_12x12;

typedef struct ChineseCell_16x16
{
    char index[4];
    unsigned char data[32];
} ChineseCell_16x16;

typedef struct ChineseCell_24x24
{
    char index[4];
    unsigned char data[72];
} ChineseCell_24x24;

typedef struct ChineseCell_32x32
{
    char index[4];
    unsigned char data[128];
} ChineseCell_32x32;

extern const unsigned char ascii_06x12[95][12];
extern const unsigned char ascii_08x16[95][16];
extern const unsigned char ascii_12x24[95][36];
extern const unsigned char ascii_16x32[95][64];

extern const ChineseCell_12x12 chinese_12x12[];
extern const ChineseCell_16x16 chinese_16x16[];
extern const ChineseCell_24x24 chinese_24x24[];
extern const ChineseCell_32x32 chinese_32x32[];

#endif // __FONT_H__