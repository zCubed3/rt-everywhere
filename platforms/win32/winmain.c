/****************************************************************************************/
/* Copyright (c) 2023 zCubed3                                                        */
/*                                                                                      */
/* Permission is hereby granted, free of charge, to any person obtaining a copy         */
/* of this software and associated documentation files (the "Software"), to deal        */
/* in the Software without restriction, including without limitation the rights         */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            */
/* copies of the Software, and to permit persons to whom the Software is                */
/* furnished to do so, subject to the following conditions:                             */
/*                                                                                      */
/* The above copyright notice and this permission notice shall be included in all       */
/* copies or substantial portions of the Software.                                      */
/*                                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        */
/* SOFTWARE.                                                                            */
/****************************************************************************************/

#include <Windows.h>

// TODO: Use windows specific console calls?
#include <stdio.h>
#include <stdlib.h>

#include <rt_everywhere.h>

viewport_t viewport = { 0, 0 };
HBITMAP hbitmap = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// TODO
	switch (uMsg) {
		case WM_SIZE: {
			viewport.width = LOWORD(lParam);
			viewport.height = HIWORD(lParam);

			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP bm = CreateCompatibleBitmap(hdcMem, viewport.width, viewport.height);
			SelectObject(hdcMem, bm);

			// TODO: Use something faster than SetPixel
			for (int x = 0; x < viewport.width; x++) {
				for (int y = 0; y < viewport.height; y++) {
					point_t point = { x, y };
					rvec3_t col;

					trace_scene(col, viewport, point);

					SetPixel(hdc, x, y, RGB(col[0] * 255, col[1] * 255, col[2] * 255));
				}
			}

			EndPaint(hwnd, &ps);
			break;
		}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	const TCHAR* WINDOW_CLASS = TEXT("RT Everywhere Class");
	const TCHAR* WINDOW_NAME = TEXT("RT Everywhere");

	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));

	wc.lpfnWndProc = &WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = WINDOW_CLASS;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		WINDOW_CLASS,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,

		CW_USEDEFAULT,
		CW_USEDEFAULT,
		256,
		256,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL) {
		fprintf(stderr, "Failed to open HWND!\n");
		fflush(stderr);
		return 1;
	}

	ShowWindow(hwnd, nCmdShow);

	MSG msg;
	memset(&msg, 0, sizeof(msg));

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}