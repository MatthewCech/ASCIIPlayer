/*!***************************************************************************
@file    main.cpp
@author  mc-w
@date    2/25/2016
@brief   Stack-based menu design for navigating in a console.

@copyright (See LICENSE.md)
*****************************************************************************/
#pragma once
#include <stack>
#include <map>
#include <string>
#include <vector>
#include <ConsoleUtils/console-utils.hpp>

namespace ASCIIMenus 
{
  // Enums
  enum ButtonState { SELECTED, NOT_SELECTED };
  enum Orientation { HORIZONTAL, VERTICAL };
}

//////////////////////////////////////////////////////
// Registered series of lookups for menus to interact. Each entry includes the menu in question
// represented as the name and a pointer to the container in question.
//////////////////////////////////////////////////////
template<class T> class Container;
template<class T> class MenuRegistry
{
public:
  MenuRegistry()
    : registry_(std::map<std::string, Container<T> *>)

  // Register a specific key/containtainer association. Duplicate keys override eachother.
  static void Register(std::string str, T *con)
  { 
    registry_[str] = con;  
  }
  
  // Gets the container associted with the string key, returns null if it does not exist.
  static Container<T> *GetContainer(std::string str)
  { 
    auto iter = registry_.find(str); 
    if(iter != registry_.end())
      return registry_[str];

    return nullptr;
  }

private:
  // Private variables
  std::map<std::string, Container<T> *> registry_;
};



//////////////////////////////////////////////////////
// Menu Container and associated struct. The basic idea is that you create a label and a destination
// that allows menu navigation.
//////////////////////////////////////////////////////
template<class T> struct Selectable
{
  // Callback Functions
  typedef void(T::*CallbackFunc)();

  Selectable(std::string label, std::string target, CallbackFunc function = nullptr)
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
  CallbackFunc CallbackFunction;
};


class Menu
{
public:
  // Effective ctor, registers the name for you. Deallocation needed after.
  template<typename T> static Container<T>* Create(std::string menuName)
  {
    Container<T>* c = new Container<T>(menuName);
    MenuRegistry::Register(menuName, c);
    return c;
  }
};


template<class T> class Container
{
  // Callback Functions
  typedef void(T::*CallbackFunc)();


public:
  // Member functions
  void AddItem(std::string label, std::string target, CallbackFunc function = nullptr)
  { 
    lineItems_.push_back(Selectable(label, target, function)); 
  }
  
  // Setter
  void SetOrientation(ASCIIMenus::Orientation o)   { orientation_ = o;  }
  void SetPosition(size_t x, size_t y) { x_ = x; y_ = y;    }
  void SetSelectedLine(size_t line) { selected_ = line; }

  // Accessors
  std::vector<Selectable<T>> &GetAllItems() { return lineItems_; }
  ASCIIMenus::Orientation GetOrientation()  { return orientation_; }
  Selectable<T> GetSelected()               { return lineItems_[selected_]; }
  size_t GetSelectedLine()                  { return selected_; }
  size_t GetXPos()                          { return x_; }
  size_t GetYPos()                          { return y_; }


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
  std::vector<Selectable<T>> lineItems_;
  ASCIIMenus::Orientation orientation_;
  size_t x_;
  size_t y_;
};



//////////////////////////////////////////////////////
// Stack-based menu system. Uses a stack of different menu containers
// to display the most recently navigated to on the top when draw is called.
//////////////////////////////////////////////////////
template<class T> class MenuSystem
{
private:

  // Pushes a continer to the stack if possible.
  void pushContainer(Container<T> *c)
  {
    if(stack_.size() > 0)
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
  MenuSystem(std::string initial = "")
    : stack_()
    , colorSelected_(RConsole::MAGENTA)
    , colorUnselected_(RConsole::GREY)
  { 
    Container<T> *c = MenuRegistry::GetContainer(initial);
    if(c != nullptr)
      stack_.push(c); 
  }
  
  void SetColorSelected(RConsole::Color c)   { colorSelected_ = c;   }
  void SetColorUnselected(RConsole::Color c) { colorUnselected_ = c; }
  bool IsVisible() { return stack_.size() > 0;  }
  size_t MenuDepth() { return stack_.size(); }

  // Member functions
  void Down() { if (stack_.size() > 0) stack_.top()->Next(); }
  void Up()   { if (stack_.size() > 0) stack_.top()->Prev(); }

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

  // Attempts to close all menus
  void CloseAll() {
    while(stack_.size() > 0)
      stack_.pop();
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

    std::stack<Container *> tempStack = stack_;
    std::vector<Container *> newStack = std::vector<Container *>();
    while(!tempStack.empty())
    {
        newStack.push_back(tempStack.top());
        tempStack.pop();
    }

    if(drawAll)
      for (auto iter = newStack.rbegin(); iter != newStack.rend(); ++iter)
      {
        auto stackItem = *iter;
        const std::vector<Selectable> &v = stackItem->GetAllItems();
        const ASCIIMenus::Orientation o = stackItem->GetOrientation();
        size_t xPos = stackItem->GetXPos();
        size_t yPos = stackItem->GetYPos();

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
  std::stack<Container<T>*> stack_;
  RConsole::Color colorSelected_;
  RConsole::Color colorUnselected_;
};


