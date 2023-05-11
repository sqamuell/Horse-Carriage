#include "NoteCoder.h"
#include "Arduino.h"

NoteCoder::NoteCoder()
{
}

float NoteCoder::decodeDirection(int *frequencies)
{
	int number = 0;

	for (int i = 0; i < sizeof(frequencies) - 1; i++)
	{
		int digit = round((frequencies[i] - startMessage - step) / step);
		number += digit * pow(10, i);
	}

	float direction = float(number) / 1000;
	return direction;
}

void NoteCoder::encodeDirection(int *frequencies, float direction)
{
	int intDirection = round(direction * 1000);

	int index = 0;
	while (intDirection > 0 && index < 3)
	{
		frequencies[index] = startMessage + (intDirection % 10) * step + step;
		intDirection /= 10;
		index++;
	}
}

void NoteCoder::createAudioArray(int frequencies[], int *notes)
{
	notes[0] = startMessage;
	notes[1] = frequencies[0];
	notes[2] = frequencies[1];
	notes[3] = frequencies[2];
	notes[4] = endMessage;
	notes[5] = stop;
}