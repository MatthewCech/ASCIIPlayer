/*
 * This is the static initalization file that contains all the differnet 
 * varaibles that need to be statically initialized from header libraries
 * or just in general.
 *
 * Note that with static initialization, order is not guarenteed in actual
 * execution, so do not treat the top-to-bottom order of this file as 
 * anything but sectioned off parts designed to make the reading easier.
 *
 ********************************************************/

  ///////////////////////////////////
 // static-init-console-utils.cpp //
///////////////////////////////////
#include "ConsoleUtils/console-utils.hpp"


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

