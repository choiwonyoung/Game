// Viewer2 �� ���̴� ���� Ÿ�Ե��� ���� �Ѵ�.

#pragma once


const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
//const int WINDOW_WIDTH = 800;
//const int WINDOW_HEIGHT = 600;

const int REAL_WINDOW_WIDTH = WINDOW_WIDTH+18;
const int REAL_WINDOW_HEIGHT = WINDOW_HEIGHT+115;



namespace DISP_MODE
{
	enum TYPE
	{
		DISP_800X600_RIGHT,
		DISP_1024X768_RIGHT,
		DISP_800X600_LEFT,
		DISP_1024X768_LEFT,
	};
}
