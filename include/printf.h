/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _printf_h_
#define _printf_h_

#include <stdarg.h>
void printf(char *fmt, ...);

void _panic(const char *, int, const char *, ...)
    __attribute__((noreturn));

void printcharc(char ch);

#define TO_STRING(x) TO_STRING1(x)
#define TO_STRING1(x) #x

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

#define KERNEL_LOG(x) \
    printf("function < %s > in < %s - line %d > %s:\n", __func__, __FILE__, __LINE__, #x)

#endif /* _printf_h_ */
