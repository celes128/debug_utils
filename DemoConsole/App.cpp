#include "App.h"

// string to wstring conversion
#include <string>
#include <codecvt>
#include <locale>

#include "utils.h"
#include "commands.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "debug_utils.lib")


//					Entry point
//
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	// Ignore the return value because we want to run the program even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (FAILED(CoInitialize(NULL))) {
		return 1;
	}

	{// Local block so that the App destructor runs before CoUninitialize.
		App app;

		if (SUCCEEDED(app.Initialize())) {
			app.RunMessageLoop();
		}
	}

	CoUninitialize();

	return 0;
}



//					Initialization and shutdown
//

//const WCHAR kFontName[] = L"Verdana";
//const WCHAR kFontName[] = L"Lucida Console";
//const WCHAR kFontName[] = L"Lucida Sans Unicode";
//const WCHAR kFontName[] = L"Andale Mono";

const WCHAR *App::kFontName		= L"Consolas";
const float App::kFontSize		= 22.f;

App::App()
	: m_hwnd(NULL)
	, m_pD2DFactory(NULL)
	, m_pDWriteFactory(NULL)
	, m_pRenderTarget(NULL)
	, m_pTextFormat(NULL)
	, m_pSolidBrush(NULL)
	, m_pStrokeStyle(NULL)
	, m_console(NULL)
{
}

App::~App()
{
	ReleaseResources();
}

void App::ReleaseResources()
{
	delete m_console;
	m_console = nullptr;

	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pSolidBrush);
	SafeRelease(&m_pStrokeStyle);
}

// Creates the application window and initializes
// device-independent resources.
HRESULT App::Initialize()
{
	HRESULT hr = S_OK;

	// Initialize device-indpendent resources, such as the Direct2D factory.
	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr)) {
		// Create and register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = App::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"TestAppTextEditing";
		RegisterClassEx(&wcex);

		// Create the application window.
		//
		// Because the CreateWindow function takes its size in pixels, we
		// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;
		m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		m_hwnd = CreateWindow(
			wcex.lpszClassName,
			L"Plot",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(1024* dpiX / 96.f)),
			static_cast<UINT>(ceil(768.f * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
		);
		if (!m_hwnd) {
			return E_FAIL;
		}

		hr = CreateDeviceResources();
		if (FAILED(hr)) {
			return hr;
		}
		
		CreateTheConsole();

		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);

		//// Test - Send two "lorem" commands to the console
		//// in order to test the scrolling.
		//SendMessage(m_hwnd, WM_CHAR, )
	}

	return hr;
}

// Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D and
// DirectWrite factories,  and a DirectWrite Text Format object
// (used for identifying particular font characteristics).
HRESULT App::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr)) {
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr)) {
		hr = m_pDWriteFactory->CreateTextFormat(
			kFontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			App::kFontSize,
			L"", //locale
			&m_pTextFormat
		);
	}

	if (SUCCEEDED(hr)) {
		// Center the text horizontally and vertically.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	return hr;
}

//  This method creates resources which are bound to a particular
//  Direct3D device. It's all centralized here, in case the resources
//  need to be recreated in case of Direct3D device loss (eg. display
//  change, remoting, removal of video card, etc).
HRESULT App::CreateDeviceResources()
{
	if (m_pRenderTarget) {
		return S_OK;
	}

	HRESULT hr = S_OK;

	// Create a Direct2D render target.
	hr = m_pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_hwnd, WindowSizeU()),
		&m_pRenderTarget
	);
	if (SUCCEEDED(hr)) {
		// Create an orange brush.
		hr = m_pRenderTarget->CreateSolidColorBrush(ColorFrom3i(209, 117, 25), &m_pSolidBrush);
	}

	if (SUCCEEDED(hr)) {
		hr = m_pD2DFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_ROUND,
				0.0f,
				D2D1_DASH_STYLE_SOLID,
				0.0f),
			nullptr,
			0,
			&m_pStrokeStyle
		);
	}

	return hr;
}

//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
void App::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pSolidBrush);
	SafeRelease(&m_pStrokeStyle);
}


void App::CreateTheConsole()
{
	// Create an interpreter.
	dbgutils::CmdList commands{
		std::make_shared<CommandEcho>(),
		std::make_shared<CommandLoremIpsum>()
	};
	dbgutils::Interpreter interp(commands);
	
	// Position rectangle.
	auto w = WindowSizeF().width * 0.9f;
	auto h = WindowSizeF().height * 0.9f;
	auto x = (WindowSizeF().width - w) / 2.f;
	auto y = (WindowSizeF().height - h) / 2.f;
	auto r = RectF_FromPointAndSize({ x,y }, { w,h });

	m_console = new Console(
		interp,
		32,				// history capacity
		32,				// output capacity
		r,
		m_pRenderTarget,
		GetGraphicsContext()
	);

	// Add a CommandListCommands command to the interpreter.
	auto *interpreter = m_console->GetInterpreter();
	interpreter->InstallCommand(std::make_shared<CommandListCommands>(interpreter));
}


//					Rendering

//
//  Called whenever the application needs to display the client window.
//
//  Note that this function will not render anything if the window
//  is occluded (e.g. when the screen is locked).
//  Also, this function will automatically discard device-specific
//  resources if the Direct3D device disappears during function
//  invocation, and will recreate the resources the next time it's
//  invoked.
//
HRESULT App::OnRender()
{
	HRESULT hr;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr) && !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)) {
		// Initialize rendering
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// Clear
		const auto CLEARCOLOR = ColorFrom3i(0, 0, 0);
		ClearWindow(CLEARCOLOR);

		// Render objects here...
		if (m_console) {
			Renderer ren{ m_pRenderTarget, m_pSolidBrush };
			m_console->Draw(ren);
		}

		// Finalize rendering
		hr = m_pRenderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET) {
			hr = S_OK;
			DiscardDeviceResources();
		}
	}

	return hr;
}

void App::ClearWindow(const D2D1::ColorF &color)
{
	m_pRenderTarget->Clear(color);
}


//					Win32 message handling functions
//

void App::OnResize(const D2D1_SIZE_U &size)
{
	if (!m_pRenderTarget) {
		return;
	}

	// Note: This method can fail, but it's okay to ignore the
	// error here -- it will be repeated on the next call to EndDraw.
	m_pRenderTarget->Resize(size);

	// TODO:
	//m_console->SetRectangle(WindowRectF());
}

void App::OnWMChar(WPARAM wParam)
{
	auto c = static_cast<wchar_t>(wParam);

	auto redraw = m_console->HandleChar(c);
	if (redraw) {
		SendRedrawRequest();
	}
}

void App::OnWMKeydown(WPARAM wParam)
{
	auto key = static_cast<Key>(wParam);

	//// Get Modifier Keys State
	//auto ctrl = is_key_down(VK_CONTROL);
	//auto alt = is_key_down(VK_MENU);
	//auto mod = ModKeyState{ ctrl, alt };

	auto redraw = m_console->HandleKey(key);
	if (redraw) {
		SendRedrawRequest();
	}
}

void App::OnWMMouseWheel(WPARAM wParam)
{
	auto zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	auto redraw = m_console->HandleMouseWheel(zDelta);
	if (redraw) {
		SendRedrawRequest();
	}
}



//					Utils
//

D2D1_SIZE_F App::WindowSizeF() const
{
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	return D2D1::SizeF(
		static_cast<float>(rc.right - rc.left),
		static_cast<float>(rc.bottom - rc.top)
	);
}

D2D1_SIZE_U App::WindowSizeU() const
{
	return D2D1_SIZE_U{
		(UINT32)WindowSizeF().width,
		(UINT32)WindowSizeF().height
	};
}

RectF App::WindowRectF() const
{
	return RectF_FromPointAndSize(
		Point2dF{ 0.f,0.f },
		WindowSizeF()
	);
}

GraphicsContext App::GetGraphicsContext() const
{
	return GraphicsContext{
		m_pDWriteFactory,
		m_pTextFormat
	};
}

void App::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void App::SendRedrawRequest()
{
	InvalidateRect(m_hwnd, nullptr, TRUE);
}


//					Window callback
//
LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		App *app = (App *)pcs->lpCreateParams;

		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));

		return 1;
	}

	App *app = reinterpret_cast<App *>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	if (!app) {
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	LRESULT result = 0;
	bool	wasHandled = true;

	switch (message) {
	case WM_SIZE: {
		app->OnResize(D2D1_SIZE_U{ LOWORD(lParam), HIWORD(lParam) });
	}break;

	case WM_PAINT:
	case WM_DISPLAYCHANGE: {
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		app->OnRender();
		EndPaint(hwnd, &ps);
	}break;

	case WM_CHAR: {
		app->OnWMChar(wParam);
	}break;

	case WM_KEYDOWN: {
		app->OnWMKeydown(wParam);
	}break;

	case WM_MOUSEWHEEL: {
		app->OnWMMouseWheel(wParam);
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
		result = 1;
	}break;

	default:
		wasHandled = false;
		break;
	}

	if (!wasHandled) {
		result = DefWindowProc(hwnd, message, wParam, lParam);
	}

	return result;
}