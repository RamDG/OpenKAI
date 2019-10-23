/*
 * Console.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: Kai Yan
 */

#include "Console.h"
#include "../Script/Kiss.h"

namespace kai
{

Console::Console()
{
	m_iY = 0;
}

Console::~Console()
{
	endwin();
}

bool Console::init(void* pKiss)
{
	IF_F(!this->BASE::init(pKiss));
	Kiss* pK = (Kiss*) pKiss;

	initscr();
	noecho();
	cbreak();
	start_color();
	use_default_colors();
	init_pair(CONSOLE_COL_TITLE, COLOR_WHITE, -1);
	init_pair(CONSOLE_COL_NAME, COLOR_GREEN, -1);
	init_pair(CONSOLE_COL_FPS, COLOR_YELLOW, -1);
	init_pair(CONSOLE_COL_MSG, COLOR_WHITE, -1);
	init_pair(CONSOLE_COL_ERROR, COLOR_RED, -1);

	return true;
}

void Console::addMsg(const string &msg, int iCol, int iX, int iLine)
{
	attrset(iCol);
	m_iY += iLine;
	mvaddstr(m_iY, iX, msg.c_str());
}

void Console::draw(void)
{
	move(0,0);
    refresh();

    erase();

	attrset(COLOR_PAIR(CONSOLE_COL_TITLE)|A_BOLD);
	mvaddstr(0, 1, this->getName()->c_str());
	m_iY = 1;
}

}
