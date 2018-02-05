/*!***************************************************************************
@file    main.cpp
@author  mc-w
@date    2/25/2016
@brief   Stack-based menu design for navigating in a console.

@copyright (See LICENSE.md)
*****************************************************************************/
#include "console-utils.hpp"
#include <stack>
#include <map>
#include <string>
#include <vector>

namespace ASCIIMenus 
{
  // Callback Functions
  typedef void(*CallbackFunction)();

  // Enums
  enum ButtonState { SELECTED, NOT_SELECTED };
  enum Orientation { HORIZONTAL, VERTICAL };
}

//////////////////////////////////////////////////////
// Registered series of lookups for menus to interact. Each entry includes the menu in question
// represented as the name and a pointer to the container in question.
//////////////////////////////////////////////////////
class Container;
class MenuRegistry
{
public:
  // Register a specific key/containtainer association. Duplicate keys override eachother.
  static void Register(std::string str, Container *con) 
  { 
    registry_[str] = con;  
  }
  
  // Gets the container associted with the string key, returns null if it does not exist.
  static Container *GetContainer(std::string str)       
  { 
    auto iter = registry_.find(str); 
    if(iter != registry_.end())
      return registry_[str];

    return nullptr;
  }

private:
  // Private variables
  static std::map<std::string, Container *> registry_;
};

// Static init
std::map<std::string, Container *> MenuRegistry::registry_ = std::map<std::string, Container *>();



//////////////////////////////////////////////////////
// Menu Container and associated struct. The basic idea is that you create a label and a destination
// that allows menu navigation.
//////////////////////////////////////////////////////
struct Selectable
{
  Selectable(std::string label, std::string target, ASCIIMenus::CallbackFunction function = nullptr)
    : Label(label)
    , Target(target)
    , CallbackFunction(function)
  {  }

  void Call()
  {
    if (CallbackFunction != nullptr)
      CallbackFunction();
  }

  std::string Label;
  std::string Target;
  ASCIIMenus::CallbackFunction CallbackFunction;
};
class Container
{
public:
  // Effective ctor, registers the name for you. Deallocation needed after.
  static Container *Create(std::string menuName) 
  { 
    Container *c = new Container(menuName);
    MenuRegistry::Register(menuName, c); 
    return c;
  }

  // Member functions
  void AddItem(std::string label, std::string target, ASCIIMenus::CallbackFunction function = nullptr) 
  { 
    lineItems_.push_back(Selectable(label, target, function)); 
  }
  
  // Setter
  void SetOrientation(ASCIIMenus::Orientation o)   { orientation_ = o;  }
  void SetPosition(size_t x, size_t y) { x_ = x; y_ = y;    }
  void SetSelectedLine(size_t line) { selected_ = line; }

  // Accessors
  std::vector<Selectable> &GetAllItems()   { return lineItems_; }
  ASCIIMenus::Orientation GetOrientation() { return orientation_; }
  Selectable GetSelected()                 { return lineItems_[selected_]; }
  size_t GetSelectedLine()                 { return selected_; }
  size_t GetXPos()                         { return x_; }
  size_t GetYPos()                         { return y_; }


  // Changes to next selection, with wrapping.
  void Next() 
  { 
    ++selected_; 
    if(selected_ > lineItems_.size() - 1) 
      selected_ = 0; 
  }

  // Changes to previous selection, with wrapping.
  void Prev() 
  { 
    if(selected_ == 0) 
      selected_ = lineItems_.size() - 1; 
    else
      --selected_; 
  }

private:
  // Private ctor
    Container(std::string menuName) 
    : selected_(0)
    , name_(menuName)
    , lineItems_()
    , orientation_(ASCIIMenus::Orientation::VERTICAL)
    , x_(0)
    , y_(0)
  {  }

  // Private variables
  size_t selected_;
  std::string name_;
  std::vector<Selectable> lineItems_;
  ASCIIMenus::Orientation orientation_;
  size_t x_;
  size_t y_;
};



//////////////////////////////////////////////////////
// Stack-based menu system. Uses a stack of different menu containers
// to display the most recently navigated to on the top when draw is called.
//////////////////////////////////////////////////////
class MenuSystem
{
private:

  // Pushes a continer to the stack if possible.
  void pushContainer(Container *c)
  {
    stack_.top()->GetSelected().Call();
    if (c == nullptr)
    {
      if (stack_.top()->GetSelected().Target == "back")
        stack_.pop();
    }
    else
      stack_.push(c);
  }

  // Drawing a menu item at a location
  void drawItem(size_t x, size_t y, std::string str, ASCIIMenus::ButtonState buttonState)
  {
    if(buttonState == ASCIIMenus::NOT_SELECTED)
      RConsole::Canvas::DrawString(str.c_str(), static_cast<float>(x), static_cast<float>(y), colorUnselected_);
    else if(buttonState == ASCIIMenus::SELECTED)
      RConsole::Canvas::DrawString(str.c_str(), static_cast<float>(x), static_cast<float>(y), colorSelected_);
  }

public:
  // Ctor
  MenuSystem(std::string initial)
    : stack_()
    , colorSelected_(RConsole::MAGENTA)
    , colorUnselected_(RConsole::GREY)
  { 
    Container *c = MenuRegistry::GetContainer(initial);
    if(c == nullptr)
      throw "Verify the strings used for the intial menu choice!";
    stack_.push(c); 
  }
  
  // Setters
  void SetColorSelected(RConsole::Color c)   { colorSelected_ = c;   }
  void SetColorUnselected(RConsole::Color c) { colorUnselected_ = c; }
  
  // Member functions
  void Down() { stack_.top()->Next(); }
  void Up()   { stack_.top()->Prev(); }

  // Selects the currently highlighted line from the menu on the top of the stack
  void Select() 
  {
    if(stack_.size() > 0)
      pushContainer(MenuRegistry::GetContainer(stack_.top()->GetSelected().Target));
  }

  // Indicate a specific menu to push via name.
  void Select(std::string manualInput)
  {
    pushContainer(MenuRegistry::GetContainer(manualInput));
  }

  // Goes back. Returns if it did go back or not.
  bool Back() {
    if (stack_.size() > 0)
    {
      stack_.pop();
      return true;
    }
    
    return false;
  }

  // Draws the menu
  void Draw(size_t x = 3, size_t y = 2, bool drawAll = false)
  {

    if (stack_.size() == 0)
      return;

    if(drawAll)
      for (auto&& stackItem : stack_._Get_container())
      {
        const std::vector<Selectable> &v = stackItem->GetAllItems();
        const ASCIIMenus::Orientation o = stackItem->GetOrientation();
        const size_t xPos = stackItem->GetXPos();
        const size_t yPos = stackItem->GetYPos();

        // Vertical menus
        if (o == ASCIIMenus::VERTICAL)
        {
          for (size_t i = 0; i < v.size(); ++i)
          {
            if (i == stackItem->GetSelectedLine())
              drawItem(x + xPos, i + y + yPos, v[i].Label.c_str(), ASCIIMenus::SELECTED);
            else
              drawItem(x + xPos, i + y + yPos, v[i].Label.c_str(), ASCIIMenus::NOT_SELECTED);
          }
        }

        // Horizontal Menus
        else if (o == ASCIIMenus::HORIZONTAL)
        {
          size_t xOffset = 0;
          for (size_t i = 0; i < v.size(); ++i)
          {
            if (i == stackItem->GetSelectedLine())
              drawItem(xOffset + x + xPos, y + yPos, v[i].Label.c_str(), ASCIIMenus::SELECTED);
            else
              drawItem(xOffset + x + xPos, y + yPos, v[i].Label.c_str(), ASCIIMenus::NOT_SELECTED);
            xOffset += v[i].Label.size();
          }
        }
      }

    // Create variables
    const std::vector<Selectable> &v = stack_.top()->GetAllItems();
    const ASCIIMenus::Orientation o = stack_.top()->GetOrientation();
    const size_t xPos = stack_.top()->GetXPos();
    const size_t yPos = stack_.top()->GetYPos();

    // Vertical menus
    if (o == ASCIIMenus::VERTICAL)
    {
      for (size_t i = 0; i < v.size(); ++i)
      {
        if (i == stack_.top()->GetSelectedLine())
          drawItem(x + xPos, i + y + yPos, v[i].Label.c_str(), ASCIIMenus::SELECTED);
        else
          drawItem(x + xPos, i + y + yPos, v[i].Label.c_str(), ASCIIMenus::NOT_SELECTED);
      }
    }

    // Horizontal Menus
    else if (o == ASCIIMenus::HORIZONTAL)
    {
      size_t xOffset = 0;
      for (size_t i = 0; i < v.size(); ++i)
      {
        if (i == stack_.top()->GetSelectedLine())
          drawItem(xOffset + x + xPos, y + yPos, v[i].Label.c_str(), ASCIIMenus::SELECTED);
        else
          drawItem(xOffset + x + xPos, y + yPos, v[i].Label.c_str(), ASCIIMenus::NOT_SELECTED);
        xOffset += v[i].Label.size();
      }
    }
  }

private:
  // Private variables
  std::stack<Container *> stack_;
  RConsole::Color colorSelected_;
  RConsole::Color colorUnselected_;
};


