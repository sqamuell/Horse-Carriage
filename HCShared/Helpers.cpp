#include "Helpers.h"

float Helpers::mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String Helpers::getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
		if (data.charAt(i) == separator || i == maxIndex)
		{
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

double Helpers::distance_between_points(Point2d a, Point2d b)
{
	double dx = b.x - a.x;
	double dy = b.y - a.y;
	return sqrt(dx * dx + dy * dy);
}

Helpers::Vector2d Helpers::vector_between_points(Point2d start, Point2d end)
{
	struct Vector2d vec = {end.x - start.x, end.y - start.y};
	return vec;
}

double Helpers::angle_between_vectors(struct Vector2d v1, struct Vector2d v2)
{
	double cross_product = get_cross_product(v1, v2);

	double a_magnitude = sqrt(v1.x * v1.x + v1.y * v1.y);
	double b_magnitude = sqrt(v2.x * v2.x + v2.y * v2.y);
	double angle = acos(cross_product / (a_magnitude * b_magnitude));

	return angle;
}

double Helpers::radians_to_degrees(double radians)
{
	return radians * 180.0 / PI;
}

double Helpers::get_cross_product(struct Vector2d v1, struct Vector2d v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

int Helpers::get_angle_side(struct Vector2d v1, struct Vector2d v2)
{
	double cross_product = get_cross_product(v1, v2);
	if (cross_product > 0)
	{
		return 1;
	}
	else if (cross_product < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}