#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <Windows.h>

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;

void setcursortype(CURSOR_TYPE c)
{
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c)
	{
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void gotoxy(int x, int y)
{
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

using namespace cv;

const char* d_str = " .-/i:+=x$X%8#";
const short vid_x = 128;
const short vid_y = 72;
const short dither_lvl = strlen(d_str);

int main(void)
{
	Mat frame, grayscale, resized;
	int idx;
	char vid_path[256], screen[vid_y * (vid_x * 2 + 1)];
	
	for (int i = 1; i < vid_y; i++)
		screen[i * (vid_x * 2 + 1) - 1] = '\n';
	screen[vid_y * (vid_x * 2 + 1) - 1] = '\0';

	while (true)
	{
		std::cout << "Enter your video path: ";
		std::cin >> vid_path;

		VideoCapture cap(vid_path);
		float fps = cap.get(CAP_PROP_FPS);
		std::cout << "prop_fps: " << fps << std::endl << "Press Enter to play...";
		getchar();

		if (!cap.isOpened()) {
			std::cerr << "ERROR! Unable to open video\n";
			return -1;
		}

		setcursortype(NOCURSOR);
		while (true)
		{
			cap.read(frame);
			if (frame.empty())
			{
				std::cerr << "ERROR! blank frame grabbed\n";
				break;
			}

			resize(frame, resized, cv::Size(vid_x, vid_y));
			cvtColor(resized, grayscale, CV_RGB2GRAY);

			gotoxy(0, 0);
			for (int i = 0; i < vid_y; i++)
			{
				for (int j = 0; j < vid_x; j++)
				{
					idx = ((grayscale.at<uchar>(i, j) + ((256 / 2) / dither_lvl)) / (256 / dither_lvl));
					if (idx >= dither_lvl) idx = dither_lvl - 1;
					screen[i * (vid_x * 2 + 1) + j * 2] = screen[i * (vid_x * 2 + 1) + j * 2 + 1] = d_str[idx];
				}
			}
			fprintf(stdout, "%s", screen);

			imshow("video", grayscale);
			if (waitKey(1000 / fps) > 0)
				break;
		}
	}
	return 0;
}