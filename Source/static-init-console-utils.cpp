#include "ConsoleUtils\console-utils.hpp"


// console static inits
namespace RConsole
{
#define DEFAULT_WIDTH_SIZE (rlutil::tcols() - 1)
#define DEFAULT_HEIGHT_SIZE rlutil::trows()

	// Static initialization in non-guaranteed order.
	CanvasRaster Canvas::r_ = CanvasRaster(DEFAULT_WIDTH_SIZE, DEFAULT_HEIGHT_SIZE);
	CanvasRaster Canvas::prev_ = CanvasRaster(DEFAULT_WIDTH_SIZE, DEFAULT_HEIGHT_SIZE);
	bool Canvas::hasLazyInit_ = false;
	bool Canvas::isDrawing_ = true;
	unsigned int Canvas::width_ = DEFAULT_WIDTH_SIZE;
	unsigned int Canvas::height_ = DEFAULT_HEIGHT_SIZE;
	Field2D<bool> Canvas::modified_ = Field2D<bool>(DEFAULT_WIDTH_SIZE, DEFAULT_HEIGHT_SIZE);
}
