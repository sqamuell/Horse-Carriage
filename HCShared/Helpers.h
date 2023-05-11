#include "Arduino.h"

class Helpers
{
public:
	struct Vector2d
	{
		double x;
		double y;
	};

	struct Point2d
	{
		double x;
		double y;
	};

	static float mapf(float x, float in_min, float in_max, float out_min, float out_max);
	static String getValue(String data, char separator, int index);
	static double distance_between_points(Point2d a, Point2d b);
	static struct Vector2d vector_between_points(Point2d start, Point2d end);
	static double angle_between_vectors(struct Vector2d v1, struct Vector2d v2);
	static double radians_to_degrees(double radians);
	static double get_cross_product(struct Vector2d v1, struct Vector2d v2);
	static int get_angle_side(struct Vector2d v1, struct Vector2d v2);
};