#pragma once

#include "framework.h"

struct GraphicsContext {
	IDWriteFactory			*dwriteFactory{ nullptr };
	IDWriteTextFormat		*textFormat{ nullptr };
};