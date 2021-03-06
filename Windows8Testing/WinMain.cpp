// Welcome to Windows 8! 
/* I am here to test, play, and try out various features of Windows 8,
** and even learn how to use Direct3D 11 as well. So wish me luck! I hope
** to create a base format for setting up a window, and basic functions for 
** setting up Direct3D 11 applications to use in future appliccations.
*/

#include <Windows.h>
#include <iostream>
#include "Direct3DApp.h"
#include "Timer.h"
#include "Direct3DRender.h"
#include "GameObject_Cube.h"
// Include new headers

/******
*   GLOBAL VARIABLES
******/
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define WINDOW_TITLE L"Chris Carlos"

HWND				g_hWnd;          // Handle to the window
HINSTANCE			g_hInstance;	// Handle to the aplication instance
bool				g_bWindowed;	// Boolean for window or full-screen mode
// Timer Variables
__int64 g_cntsPerSec;
__int64 g_currTimeStamp;
__int64 g_prevTimeStamp;
float   g_secsPerCnt;
float   g_deltaTime;

// Direct X global variables declared below (when created)

// Entry point for the game or application

int WINAPI WinMain(HINSTANCE hInstance,     // Handle to the Window
    HINSTANCE hPrevInstance,                // Handle to the previous Window
    LPSTR lpCmdLine,                        // Command line string
    int lpCmdShow);                         // Show window value

LRESULT CALLBACK WndProc(HWND hWnd,			// Handle to the window
	UINT message,							// Incoming Message
	WPARAM wparam,							// Message Info
	LPARAM lparam);							// Message info


// Here is where we will create our window to use
void InitWindow(void)
{
    WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));

	// set up the window
	wndClass.cbSize = sizeof(WNDCLASSEX);			// size of window structure
	wndClass.lpfnWndProc = (WNDPROC)WndProc;				// message callback
	wndClass.lpszClassName = WINDOW_TITLE;					// class name
	wndClass.hInstance = g_hInstance;					// handle to the application
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// default cursor
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);	// background brush

	// register a new type of window
	RegisterClassEx(&wndClass);

	g_hWnd = CreateWindow(
		WINDOW_TITLE, WINDOW_TITLE, 							// window class name and title
		g_bWindowed ? WS_OVERLAPPEDWINDOW | WS_VISIBLE : (WS_POPUP | WS_VISIBLE),// window style
		CW_USEDEFAULT, CW_USEDEFAULT,							// x and y coordinates
		SCREEN_WIDTH, SCREEN_HEIGHT,							// width and height of window
		NULL, NULL,												// parent window and menu
		g_hInstance,											// handle to application
		NULL);

	// Display the window
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int lpCmdShow)
{
	// Set some variables that will be used in InitWindow()
	g_hInstance = hInstance;
	g_bWindowed = true;

    // Initialize the window
	InitWindow();
    
    // Local Variables
    //// Init timer
    //g_cntsPerSec = 0;
    //QueryPerformanceFrequency((LARGE_INTEGER*)&g_cntsPerSec);
    //g_secsPerCnt = 1.0f / (float)g_cntsPerSec;

    //g_prevTimeStamp = 0;
    //QueryPerformanceCounter((LARGE_INTEGER*)&g_prevTimeStamp);
    //g_currTimeStamp = 0;

    // Initiate timer
    Timer GameClock;
    GameClock.Tick();

    D3DApp  DxApp;
    Render::D3DRender DxRender;
    float gameTime = 0;
    if (DxApp.InitDevice(g_hWnd))
    {
        if (DxRender.Init(&DxApp))
        {
            printf("We are good to go!");
        }
        else
        {
            printf("Something went wrong!");
        }
    }
    else
    {
        printf("Failed to create the device! Abort!");
        return 0;
    }
    // Create cube object
    GO_Cube cubeOne;
    GO_Cube cubeTwo(
        3, 0, 0,
        1.5, 1.5, 1.5,
        0, 1, 0,
        false);
    GO_Cube cubeThree(
        -3, 0, 0, 
        0.5, 0.5, 0.5,
        1, 0, 0,
        true);
    GO_Cube cubeFour(
        0, 3, 0, 
        1.5, 0.5, 1,
        0, 0, 1,
        true);
    GO_Cube cubeFive(
        -3, 3, 0,
        1, 1, 1,
        1, 1, 0,
        true);
    GO_Cube cubeSix(
        3, 3, 0,
        1, 1, 1,
        0, 1, 1,
        true
        );
    std::vector<Object::GameObject*> myCubes;

    myCubes.push_back(&cubeOne);
    myCubes.push_back(&cubeTwo);
    myCubes.push_back(&cubeThree);
    myCubes.push_back(&cubeFour);
    //myCubes.push_back(&cubeFive);
    //myCubes.push_back(&cubeSix);
    // Add the objects to the buffer of our render
    for (auto i = 0; i < myCubes.size(); i++)
    {
        DxRender.AddToIndexBuffer(
            myCubes.at(i)->GetSizeOfIndexArray(), 
            myCubes.at(i)->GetPointerToIndexArray());
        DxRender.AddToVertBuffer(
            myCubes.at(i)->GetSizeOfVertexArray(),
            myCubes.at(i)->GetPointerToVerticesArray(),
            myCubes.at(i)->GetSizeOfVertices(),
            0);
    }

    // Use msg structure for catching windows Messages
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

    //MessageBox(g_hWnd, L"Hello world!", L"I'm a box!", MB_OK);

	// Enter Windows Main loop here
	while (msg.message != WM_QUIT)
	{
        // Basic windows Peek message to handle messages appropriately
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

        /*QueryPerformanceCounter((LARGE_INTEGER*)&g_currTimeStamp);
        g_deltaTime = ((g_currTimeStamp - g_prevTimeStamp) * g_secsPerCnt);*/
        //GameClock.Tick();
        //gameTime += GameClock.DeltaTime();
        //DxApp.Render(GameClock.DeltaTime());
        gameTime = GameClock.DeltaTime();
        DxRender.Render(gameTime, &myCubes);
        DxRender.Update(gameTime);
        //Sleep(1000);
        /*g_currTimeStamp = g_prevTimeStamp;*/
        
	}

    UnregisterClass(WINDOW_TITLE, g_hInstance);

    return 0;
}

// Windows procedure callback function; handle our windows messages here
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case (WM_PAINT):
		{
			InvalidateRect(hWnd, NULL, TRUE);
			break; 
		}

		case(WM_DESTROY) :
		{
			PostQuitMessage(0);
			break;
		}

		case(WM_KEYDOWN) :
		{
			switch (wparam)
			{
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
					break;
				}
			}
		}
	}

	return DefWindowProc(hWnd, message, wparam, lparam);
}

