#include "bitmap.h"
void alloc_bitmap(struct bitmap *bp, u_int32_t base, u_int32_t cnt)
{
    bp->base = base;
    bp->len = cnt / 8;
    bp->values = (char *)kzalloc(cnt / 8, GFP_KERNEL);

}
void free_bitmap(struct bitmap* bp)
{
    if(bp->values)
    {
        kfree(bp->values);
    }
}
u_int32_t alloc_value(struct bitmap *bp)
{
    char *tmp = bp->values;
    for (int i = 0; i < bp->len; i++)
    {
        u_int8_t value = *tmp;
        if (value == 0xff)
        {
            tmp++;
            continue;
        }
        for (int j = 0; j < 8; j++)
        {
            if (!(value & (0x80 >> j)))
            {
                set_value(bp,i * 8 + j + bp->base, 1);
                return i * 8 + j + bp->base;
            }
        }
    }
    printk("bitmap alloc err\n");
    return -1;
}
void free_value(struct bitmap *bp, u_int32_t value)
{
    if (value < bp->base || value >= bp->base + bp->len * 8)
    {
        printk("bitmap free err\n");
        return;
    }
    set_value(bp,value, 0);
}
int test_value(struct bitmap *bp, u_int32_t value)
{
    if (value < bp->base || value >= bp->base + bp->len * 8)
    {
        printk("bitmap test err\n");
        return -1;
    }
    u_int32_t idx = value - bp->base;
    return (*(bp->values + idx / 8)) & (0x80 >> (idx % 8));
}
void set_value(struct bitmap *bp,u_int32_t value, int flag)
{
    u_int32_t idx = value - bp->base;
    char *tmp = bp->values + (idx) / 8;
    int index = idx % 8;
    if (flag)
        *tmp = (*tmp) | (0x80 >> index);
    else
        *tmp = (*tmp) & (~(0x80 >> index));
}