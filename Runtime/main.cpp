#include <string>
#include <Windows.h>
#include <WinUser.h>

bool InitConsole()
{
	if (AllocConsole()) {
		FILE* pCout;
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		SetConsoleTitleW(L"Scratch Console");
		return true;
	}
	return false;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		printf("WM_PAINT\n");
		return S_OK;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_SYSCHAR:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_SIZE:
		printf("WM_MISC");
		return S_OK;
	case WM_DESTROY:
		PostQuitMessage(0);
		return S_OK;
	default:
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
}

bool InitWindow(HINSTANCE hInstance, LONG Width, LONG Height)
{
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	std::wstring WndClassName = L"Scratch Window";

	WNDCLASSEXW WndClass = { 0 };
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = &WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.lpszMenuName = nullptr;
	WndClass.lpszClassName = WndClassName.c_str();
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassExW(&WndClass))
	{
		return false;
	}

	RECT windowRect = { 0, 0, Width, Height };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWnd = CreateWindowW(WndClassName.c_str(), L"Scratch Window",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	::ShowWindow(hWnd, SW_SHOW);

	return true;
}

void InitMsgLoop()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

static int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	InitConsole();
	InitWindow(hInstance, 1920, 1080);
	InitMsgLoop();
	return 0;
}