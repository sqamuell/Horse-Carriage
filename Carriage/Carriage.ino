/*!
 * @file basicTest.ino
 * @brief Demonstrate various graphic painting effects
 * @n This demo supports Arduino Uno, Leonardo, Mega2560, FireBeetle-ESP32, FireBeetle-ESP8266, and FireBeetle-M0.
 * @copyright Copyright (c) 2010 DFRobot Co. Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [LuoYufeng] (yufeng.luo@dfrobot.com)
 * @version V0.1
 * @date 2020-01-07
 * @url https://github.com/DFRobot/DFRobot_GDL
 */
#include <Wire.h>
#include "DFRobot_UI.h"
#include "Arduino.h"
#include "DFRobot_GDL.h"
#include "DFRobot_Type.h"
#include "DFRobot_Touch.h"
#include "NoteCoder.h"
#include "Helpers.h"

#define TFT_DC 8
#define TFT_CS 10
#define TFT_RST 9
#define TFT_BL 4

#define SPEAKER_PIN A3

/**
	 @brief Constructor When the screen uses hardware SPI communication, the driver IC is st7789, and the screen resolution is 240x320, this constructor can be called
	 @param dc Command/data line pin for SPI communication
	 @param cs Chip select pin for SPI communication
	 @param rst Reset pin of the screen
*/
DFRobot_ILI9488_320x480_HW_SPI screen((uint8_t)TFT_DC, (uint8_t)TFT_CS, (uint8_t)TFT_RST, (uint8_t)TFT_BL);
DFRobot_Touch_GT911 touch;

/**
	 @brief Constructor
	 @param gdl Screen object
	 @param touch Touch object
*/
DFRobot_UI ui(&screen, &touch);

#define LENGTH 100
#define SEGMENT_LENGTH 30
#define NOTE_DURATION 200
#define WAIT_TIME 2000

Helpers::Point2d points[LENGTH];

int lastPointIndex = 0;
uint16_t color = 0x00FF;

void setup()
{
	Serial.begin(9600);

	// while (!Serial);

	reset();

	ui.begin();
}

bool smooth = false;
bool is_playing = false;
int current_index = 0;

void reset()
{
	sendEndMessage();

	for (int i = 0; i < LENGTH; i++)
	{
		points[i] = {-1, -1};
	}

	lastPointIndex = 0;
	color = 0x00FF;

	smooth = false;
	is_playing = false;
	current_index = 0;
}

void loop()
{
	if (is_playing == true)
	{
		play_audio();
	}

	String scan = touch.scan();

	String x = Helpers::getValue(scan, ',', 1);
	String y = Helpers::getValue(scan, ',', 2);

	int16_t xValue = x.toInt();
	int16_t yValue = y.toInt();

	if (lastPointIndex < LENGTH && xValue > 0 && yValue > 0)
	{
		bool shouldAdd = (lastPointIndex > 0 && Helpers::distance_between_points({xValue, yValue}, points[lastPointIndex - 1]) > 1.0);

		if (lastPointIndex == 0)
		{
			points[lastPointIndex] = {double(xValue), double(yValue)};

			lastPointIndex += 1;
		}
		else if (shouldAdd)
		{
			points[lastPointIndex] = {double(xValue), double(yValue)};

			screen.drawLine(points[lastPointIndex].x, points[lastPointIndex].y, points[lastPointIndex - 1].x, points[lastPointIndex - 1].y, color += 0x0700);

			lastPointIndex += 1;
		}
	}

	if (lastPointIndex > 0 && xValue == 0 && yValue == 0)
	{
		lastPointIndex = LENGTH;
	}

	if (lastPointIndex == LENGTH && !smooth)
	{
		Serial.println("Compute and generate sounds");

		smooth = true;

		computeAndGenerateSounds();
	}

	if (scan.length() > 15)
	{
		// call reset here
		reset();
	}

	delay(10);
}

double directions[LENGTH];
int directions_length;

void computeAndGenerateSounds()
{
	int number_input_lines;
	int segmentCount;
	get_line_length(&number_input_lines, &segmentCount);

	Helpers::Point2d segmentPoints[segmentCount + 1];
	divide_line_into_points(segmentPoints, segmentCount, number_input_lines);

	calculate_directions(directions, segmentPoints, segmentCount);
	directions_length = segmentCount;

	for (int i = 0; i < segmentCount; i++)
	{
		// Serial.println(directions[i]);
	}

	current_index = 0;
	is_playing = true;
}

void sendEndMessage()
{
	int endMessageNotes[] = {
			noteCoder.startMessage,
			noteCoder.endMessage,
			noteCoder.endMessage,
			noteCoder.endMessage,
			4096 + (3 * noteCoder.step),
			4096 + (2 * noteCoder.step),
			4096 + (1 * noteCoder.step),
			4096 + (0 * noteCoder.step),
			noteCoder.endMessage,
			noteCoder.stop};
	for (int thisNote = 0; endMessageNotes[thisNote] != -1; thisNote++)
	{
		tone(SPEAKER_PIN, endMessageNotes[thisNote]);
		delay(NOTE_DURATION);
		noTone(SPEAKER_PIN);
	}
	is_playing = false;
}

NoteCoder noteCoder = NoteCoder();
void play_audio()
{
	double currentDirection = directions[current_index];

	int frequencies[3];
	noteCoder.encodeDirection(frequencies, currentDirection);
	Serial.println(currentDirection, 3);

	int notes[5];
	noteCoder.createAudioArray(frequencies, notes);

	for (int thisNote = 0; notes[thisNote] != -1; thisNote++)
	{
		tone(SPEAKER_PIN, notes[thisNote]);
		delay(NOTE_DURATION);
		noTone(SPEAKER_PIN);
	}

	current_index++;
	delay(WAIT_TIME);
	if (current_index >= directions_length)
	{
		sendEndMessage();
	}
}

void get_line_length(int *number_input_lines, int *segmentCount)
{
	int length = 0;
	*number_input_lines = 0;
	for (int i = 1; i < LENGTH; i++)
	{
		Helpers::Point2d point = points[i];
		Helpers::Point2d lastPoint = points[i - 1];

		if (point.x == -1 && point.y == -1)
		{
			*number_input_lines = i - 1;
			break;
		}

		length += Helpers::distance_between_points(point, lastPoint);
	}

	*segmentCount = floor(length / SEGMENT_LENGTH);
}

void divide_line_into_points(Helpers::Point2d *segmentPoints, int segmentCount, int number_input_lines)
{
	int currentSegment = 0;
	int currentLine = 0;
	double overhang = 0;

	while (currentLine < number_input_lines)
	{
		Helpers::Point2d _start = points[currentLine];
		Helpers::Point2d end = points[currentLine + 1];

		Serial.print(Helpers::distance_between_points({0, 0}, {3, 4}));

		double _lineDistance = Helpers::distance_between_points(_start, end);

		Helpers::Point2d start = {
				_start.x + (end.x - _start.x) * (overhang / _lineDistance),
				_start.y + (end.y - _start.y) * (overhang / _lineDistance)};

		double lineDistance = Helpers::distance_between_points(start, end);
		int lineSegments = floor((_lineDistance) / SEGMENT_LENGTH);
		overhang = lineDistance - (lineSegments * SEGMENT_LENGTH);

		for (int i = 0; i <= lineSegments; i++)
		{
			if (currentSegment <= segmentCount)
			{
				if (overhang == 0 && i == lineSegments)
				{
					continue;
				}
				segmentPoints[currentSegment] = {
						start.x + ((end.x - start.x) * (SEGMENT_LENGTH / lineDistance) * i),
						start.y + ((end.y - start.y) * (SEGMENT_LENGTH / lineDistance) * i)};

				currentSegment++;
			}
		}

		currentLine++;
	}
	Serial.print(segmentPoints[0].x);
	Serial.print("|");
	Serial.println(segmentPoints[0].y);
}

void calculate_directions(double *directions, Helpers::Point2d *segmentVertices, int segmentCount)
{
	directions[0] = 0.500;

	for (int i = 1; i < segmentCount; i++)
	{
		Helpers::Vector2d previousSegmentVector = Helpers::vector_between_points(segmentVertices[i - 1], segmentVertices[i]);
		Helpers::Vector2d nextSegmentVector = Helpers::vector_between_points(segmentVertices[i], segmentVertices[i + 1]);

		double angleInRad = Helpers::angle_between_vectors(previousSegmentVector, nextSegmentVector);
		double angleInDegrees = Helpers::radians_to_degrees(angleInRad);

		// dot product to check the side were on
		int side = Helpers::get_angle_side(previousSegmentVector, nextSegmentVector);
		if (side != 0)
		{
			directions[i] = Helpers::mapf(angleInDegrees, side == -1 ? 0 : 90, side == -1 ? 90 : 180, side == -1 ? 0 : 0.5, side == -1 ? 0.5 : 1);
		}
		else
		{
			directions[i] = 0.5;
		}
	}
}