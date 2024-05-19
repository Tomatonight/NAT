#ifndef BITMAP_H
#define BITMAP_H
#include<linux/types.h>
#include<linux/printk.h>
#include<linux/slab.h>
//#include<linux/slab.h>
struct bitmap
{
u_int32_t base;
u_int32_t len;
char* values;
};
void alloc_bitmap(struct bitmap* bp,u_int32_t base,u_int32_t cnt);
void free_bitmap(struct bitmap* bp);
u_int32_t alloc_value(struct bitmap* bp);
void free_value(struct bitmap* bp,u_int32_t value);
int test_value(struct bitmap* bp,u_int32_t value);
void set_value(struct bitmap* bp,u_int32_t value,int flag);
#endif