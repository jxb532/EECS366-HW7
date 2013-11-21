/* Wes Rupert - wesrupert@outlook.com (wkr3)  *
 * Josh Braun - jxb532@case.edu (jxb532)      *
 * Case Western Reserve University - EECS 366 *
 * 11/21/2013 - Assignment 7                  */

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "frame_buffer.h"
#include "math.h"

// Adapted from Java code at http://www.cse.psu.edu/~plassman/sjthomas/bres/Line.java
void BresenhamLine(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c);

void linePosSteep(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c);
void linePosShallow(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c);
void lineNegShallow(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c);
void lineNegSteep(FrameBuffer* fb, int x_1, int y_1, int x_2, int y_2, Color c);

#endif