#ifndef DEFINES_H
#define DEFINES_H

#include "../frameserver/defines.h"

#include <math.h>

#define PI 3.1415



struct area
{
	int xmin, width, ymin, height;
};

enum operation
{
	move,
	begin,
	end	 
};

class position
{
	public:
		position(void) {}
		~position(void) {}
		
		position(double x_in, double y_in)
		{
		x = x_in;
		y = y_in;
		}
		
		bool operator==(position pos)
		{
			if( fabs(pos.x - (*this).x) <= 0.01 && fabs(pos.y - (*this).y) <= 0.01 )
				return true;
			else
				return false;
		}	

		bool operator!=(position pos)
		{
			if( fabs(pos.x - (*this).x) > 0.01 && fabs(pos.y - (*this).y) > 0.01 )
				return true;
			else
				return false;
		}
		
		position operator=(position pos)
		{
			(*this).x=pos.x;
			(*this).y=pos.y;
			(*this).op=pos.op;
		}

		double x;
		double y;
		
		operation op;
};

#endif
