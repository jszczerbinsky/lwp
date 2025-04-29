#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct {
	int x;
	int y;
	int w;
	int h;
} Bounds;

typedef struct {
	float x;
	float y;
	float w;
	float h;
} BoundsF;

typedef struct {
	int w;
	int h;
} Size;

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	float w;
	float h;
} SizeF;

typedef struct {
	float x;
	float y;
} PointF;

#endif
