#include "App.h"

// string to wstring conversion
#include <string>
#include <codecvt>
#include <locale>

#include "utils.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "debug_utils.lib")

// A dummy command that prints its arguments.
class CommandEcho : public dbgutils::ICommand {
public:
	CommandEcho()
		: dbgutils::ICommand(L"echo")
	{}

	~CommandEcho() = default;

	std::wstring execute(const dbgutils::CmdArgs &args) override
	{
		return wstr_concat(args, L" ");
	}
};


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
const float App::kFontSize = 22.f;

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
			static_cast<UINT>(ceil(800* dpiX / 96.f)),
			static_cast<UINT>(ceil(600.f * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
		);

		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			CreateTheConsole();
			
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}
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
	//static const WCHAR msc_fontName[] = L"Verdana";
	//static const WCHAR msc_fontName[] = L"Lucida Console";
	//static const WCHAR msc_fontName[] = L"Lucida Sans Unicode";
	//static const WCHAR msc_fontName[] = L"Andale Mono";
	static const WCHAR msc_fontName[] = L"Consolas";

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
			msc_fontName,
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
	dbgutils::CmdList commands{ {std::make_shared<CommandEcho>()} };
	dbgutils::Interpreter interp(commands);
	
	m_console = new Console(
		interp,
		32,				// history capacity
		32,				// output capacity
		WindowRectF(),	// position rectangle
		GetGraphicsContext()
	);
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
HRESULT App::on_render()
{
	HRESULT hr;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr) && !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)) {
		// Initialize rendering
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// Clear
		//const auto CLEARCOLOR = ColorFrom3i(0, 20, 80);// dark blue like my Windows command line
		const auto CLEARCOLOR = ColorFrom3i(0, 0, 0);
		clear_window(CLEARCOLOR);

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

void App::clear_window(const D2D1::ColorF &color)
{
	m_pRenderTarget->Clear(color);
}

//void App::draw_caret()
//{
//	// Map text position index to caret coordinate and hit-test rectangle.
//	bool isTrailingHit = false; // Use the leading character edge for simplicity here.
//	DWRITE_HIT_TEST_METRICS htm;
//	float x, y;// caret position
//	m_cmdlineItem.textLayout->HitTestTextPosition(
//		m_console.caret(),
//		isTrailingHit,
//		OUT &x,
//		OUT &y,
//		OUT &htm
//	);
//
//	// Get the caret width; respect user settings.
//	DWORD w = 1;
//	SystemParametersInfo(SPI_GETCARETWIDTH, 0, OUT &w, 0);
//
//	// Draw a thin rectangle.
//	auto r = RectF{
//		m_cmdlineItem.bbox.left + x - w / 2u,
//		m_cmdlineItem.bbox.top + htm.top,
//		m_cmdlineItem.bbox.left + x + (w - w / 2u),
//		m_cmdlineItem.bbox.top + htm.top + htm.height
//	};
//	m_pRenderTarget->FillRectangle(&r, m_pSolidBrush);
//}


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

	// Ignore non-printable characters.
	if (!iswprint(c)) {
		return;
	}

	auto redraw = m_console->HandleChar(c);
	if (redraw) {
		request_redraw();
	}
}

void App::OnWMKeydown(WPARAM wParam)
{
	auto key = static_cast<Key>(wParam);

	auto redraw = m_console->HandleKey(key);
	if (redraw) {
		request_redraw();
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

void App::request_redraw()
{
	InvalidateRect(m_hwnd, nullptr, TRUE);
}


//					Window callback
//
LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE) {
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		App *pApp = (App *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pApp)
		);

		result = 1;
	}
	else {
		App *pApp = reinterpret_cast<App *>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			));

		bool wasHandled = false;

		if (pApp) {
			switch (message) {
			case WM_SIZE: {
				pApp->OnResize(D2D1_SIZE_U{ LOWORD(lParam), HIWORD(lParam) });
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_PAINT:
			case WM_DISPLAYCHANGE: {
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);
				pApp->on_render();
				EndPaint(hwnd, &ps);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_CHAR: {
				pApp->OnWMChar(wParam);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_KEYDOWN: {
				pApp->OnWMKeydown(wParam);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_DESTROY: {
				PostQuitMessage(0);
			}
			wasHandled = true;
			result = 1;
			break;
			}
		}

		if (!wasHandled) {
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}