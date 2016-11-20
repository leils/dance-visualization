#ifndef __TYPES_H__
#define __TYPES_H__


/* Color struct */

typedef struct point {
   double x;
   double y;
   double z;
} point;

typedef struct tri_struct {
   point a;
   point b;
   point c;
} tri_struct;

#endif
