/*
 *    Copyright (c) 2013 <benemorius@gmail.com>
 * 
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ObcMemo.h"
#include <ObcUI.h>
#include <stdlib.h>
#include <string.h>

const int wisdomQuoteOffsetEveryMax = 10;

static const char* const gWisdomQuotes[] = {
		"Drive Fast, Take Chances",
		"Tut mir Leid Michael",
		"This ain't Knight Rider",
		"How are you today Brad?",
		"Where would you like to go today?",
		"Vroom, vroom, vrooooooom....!",
		"Harder, Better, Faster, Stronger",
		"Slow down, got a death wish mate?",
		"Tell Daisy I said hello, will ya?",
		"BMW: 'Broke My Wallet'",
		"BMW: 'Babbling Mechanical Wench'",
		"BMW: 'Breaks Most Wrenches'",
		"You're not on the race track Brad!",
		"Safe driving, you must be Galya.",
		"Coconuts, coconuts, I love coconuts.",
		"Don't be stealin' ma vehicle yo!",
		"Yo dawg, I heard you love Galya!",
		"Be safe out there...",
		"Try not to kill anybody today.",
		"Warning, traffic jam in the Massey Tunnel",
		"Knock, knock. Who's there?",
		"I wouldn't go that way if I were you.",
		"Wish you knew your oil pressure, right?",
		"WARNING, you're out of fuel. Kidding...",
	};

static int RandomWisdomQuoteIdx()
{	
	const int numQuotes = sizeof(gWisdomQuotes) / sizeof(gWisdomQuotes[0]);
	return rand() % numQuotes;
}

using namespace ObcMemoState;

ObcMemo::ObcMemo(OpenOBC& obc)
	: ObcUITask(obc)
	, wisdomQuoteOffset(0)
	, wisdomQuoteIdx(RandomWisdomQuoteIdx())
{
	
	setDisplay("ObcMemo");
	std::string configState = obc.config->getValueByNameWithDefault("ObcMemoState", "Voltage");
	if(configState == "Voltage")
		state = Voltage;
	else if(configState == "FreeMem")
		state = FreeMem;
	else if(configState == "Wisdom")
		state = Wisdom;
	else if(configState == "Accelerometer")
		state = Accelerometer;
}

ObcMemo::~ObcMemo()
{
	
}

void ObcMemo::wake()
{
	runTask();
}

void ObcMemo::sleep()
{
	std::string configState;
	if(state == Voltage)
		configState = "Voltage";
	else if(state == FreeMem)
		configState = "FreeMem";
	else if(state == Wisdom)
		configState = "Wisdom";
	else if(state == Accelerometer)
		configState = "Accelerometer";
	obc.config->setValueByName("ObcMemoState", configState);
}

void ObcMemo::runTask()
{
	if(state == Voltage)
		setDisplay("%.2fV %.2fV %.2fV", obc.batteryVoltage->read(), obc.analogIn1->read(), obc.analogIn2->read());
	else if(state == FreeMem)
		setDisplay("Free memory: %i", get_mem_free());
	else if(state == Wisdom)
	{
		setDisplay("%s", gWisdomQuotes[wisdomQuoteIdx] + (wisdomQuoteOffset % strlen(gWisdomQuotes[wisdomQuoteIdx])));
		if (wisdomQuoteOffsetEvery == 0)
		{
			++wisdomQuoteOffset;
			wisdomQuoteOffsetEvery = wisdomQuoteOffsetEveryMax;
		}
		else
		{
			--wisdomQuoteOffsetEvery;
		}
	}
	else if(state == Accelerometer)
	{
		float x = obc.accel->getX();
		float y = obc.accel->getY();
		float z = obc.accel->getZ();
		setDisplay("x% 2.2f y% 2.2f z% 2.2f", x, y, z);
	}
}

void ObcMemo::buttonHandler(ObcUITaskFocus::type focus, uint32_t buttonMask)
{
	if(focus == ObcUITaskFocus::background)
	{
		if(buttonMask == BUTTON_MEMO_MASK)
			obc.ui->setActiveTask(this);
		return;
	}
	
	if(buttonMask == BUTTON_MEMO_MASK)
	{
		if(state == Voltage)
			state = FreeMem;
		else if(state == FreeMem)
			state = Wisdom;
		else if(state == Wisdom)
			state = Accelerometer;
		else if(state == Accelerometer)
			state = Voltage;

		if (state == Wisdom)
		{
			wisdomQuoteOffset = 0;
			wisdomQuoteOffsetEvery = wisdomQuoteOffsetEveryMax;
			wisdomQuoteIdx = RandomWisdomQuoteIdx();
		}
	}
}
