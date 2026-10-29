#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count)
{
    size_t used = 0;
    for (size_t i = 0; i < count; ++i)
    {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used)
            return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t a = (uint32_t)(challenge >> 32);
    uint32_t b = (uint32_t)(challenge >> 16);
    uint32_t c = (uint32_t)challenge;
    uint32_t t1 = (3u * a) ^ (b + 0x9f7fu);
    uint32_t t0 = 3u * (c - a) + 0x74e0u;
    values[0] = (3u * t0 + t1) ^ 0xd64fu;
    size_t count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    uint32_t v0 = ((((uint32_t)(challenge >> 0x2b)) * 2) & 0x3e) + 0x16;
    values[0] = v0;
    for (uint32_t i = 1; i < 6; i++)
    {
        values[i] = ((values[i - 1] + i) * 5 - ((challenge >> (0x5 + i * 4)) & 0x3f));
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    uint32_t t = (uint32_t)challenge & 0x7u;
    uint32_t b = (uint32_t)(challenge >> 0x20) ^ (uint32_t)(challenge >> 0x3);
    uint32_t ret = 0;
    values[0] = t;
    switch (t)
    {
    case 0:
        ret = 5u * b + 0xed2bu;
        break;
    case 1:
        ret = 5u * b + 0x424cdu;
        break;
    case 2:
        ret = 5u * b + 0xd218u;
        break;
    case 3:
        ret = (b ^ 0x5c43u) + (b >> 1);
        break;
    case 4:
        ret = (b ^ 0x1491u) + (b >> 2);
        break;
    case 5:
        ret = (4u * b) ^ 0xa169u;
        break;
    case 6:
        ret = (4u * b) ^ 0x4990u;
        break;
    case 7:
        ret = 3u * b + 0x2fd7u;
        break;
    default:
        break;
    }
    values[1] = ret;
    return write_answer(out, cap, values, count);
}

static uint32_t p4_walk(uint32_t a, uint32_t lo, uint32_t hi, uint32_t d)
{
    uint32_t mid = lo + ((hi - lo) >> 1);
    if (a == mid)
        return (mid + 0x86u) ^ d;
    if (a > mid)
    {
        uint32_t r = p4_walk(a, mid + 1, hi, d);
        return ((5u * r) ^ (mid + d)) ^ 0x10u;
    }
    else
    {
        uint32_t r = p4_walk(a, lo, mid - 1, d);
        return ((5u * r) ^ (mid + d)) ^ 0xeu;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t a = (uint32_t)challenge & 0x7fu;
    uint32_t d = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    values[0] = p4_walk(a, 0, 0x7fu, d);
    return write_answer(out, cap, values, count);
}

struct p5_node
{
    int next; // 节点下标，-1 = NULL
    uint32_t a1;
    uint32_t a2;
};

static const struct p5_node p5_nodes[12] = {
    /* 0  0x405ba0 */ {2, 0x0a60u, 0x0e2fu},
    /* 1  0x405bb0 */ {5, 0x0675u, 0xdd7cu},
    /* 2  0x405bc0 */ {11, 0x0541u, 0x6fd2u},
    /* 3  0x405bd0 */ {1, 0x0241u, 0xa171u},
    /* 4  0x405be0 */ {9, 0x0312u, 0x8fc6u},
    /* 5  0x405bf0 */ {7, 0x0030u, 0x4496u},
    /* 6  0x405c00 */ {0, 0x077bu, 0x4671u},
    /* 7  0x405c10 */ {4, 0x0143u, 0x2877u},
    /* 8  0x405c20 */ {10, 0x0405u, 0x3cfau},
    /* 9  0x405c30 */ {6, 0x084eu, 0x0479u},
    /* 10 0x405c40 */ {3, 0x0947u, 0xc887u},
    /* 11 0x405c50 */ {-1, 0x0b0eu, 0xbc43u},
};

static const int p5_heads[8] = {8, 10, 3, 1, 5, 7, 4, 9};

int solve_phase_5(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    int cur = p5_heads[challenge & 7];
    int k = (challenge >> 3) & 3;
    uint32_t acc = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    acc ^= p5_nodes[cur].a2;
    acc ^= 0x942eu;
    acc += p5_nodes[cur].a1;
    uint32_t last = p5_nodes[cur].a1;
    for (int i = 0; i < k; ++i)
    {
        cur = p5_nodes[cur].next;
        acc ^= p5_nodes[cur].a2;
        acc += p5_nodes[cur].a1;
        last = p5_nodes[cur].a1;
    }
    values[0] = acc;
    values[1] = last;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap)
{
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap)
{
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    return 1;
}
