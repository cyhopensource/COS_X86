#ifndef _KMEM_H_
#define _KMEM_H_

unsigned int kmalloc_ap(unsigned int sz, unsigned int *phys);
unsigned int kmalloc(unsigned int sz);
unsigned int kmalloc_a(unsigned int sz);

#endif

