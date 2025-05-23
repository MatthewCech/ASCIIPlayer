// Based on https://github.com/MatthewCech/ASCIIMenus
#pragma once
#include <stack>
#include <map>
#include <string>
#include <vector>
#include <ConsoleUtils/console-utils.hpp>

namespace ASCIIPlayer
{
  class Lobby;
}

namespace ASCIIMenus 
{
  // Callback Functions
  typedef void(ASCIIPlayer::Lobby::*CallbackFunctionVoid)();
  typedef void(ASCIIPlayer::Lobby::* CallbackFunctionInt)(int);

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



//////////////////////////////////////////////////////
// Menu Container and associated struct. The basic idea is that you create a label and a destination
// that allows menu navigation.
//////////////////////////////////////////////////////
class Selectable
{
  enum class ArgType
  {
    NONE,
    INT
  };

public:
  Selectable(std::string label, std::string target, ASCIIPlayer::Lobby* lobby, ASCIIMenus::CallbackFunctionVoid function = nullptr)
    : Label(label)
    , Target(target)
    , callbackFunctionVoid_(function)
    , callbackFunctionInt_(nullptr)
    , lobby_(lobby)
    , arg_(0)
    , argType_(ArgType::NONE)
  {  }

  Selectable(std::string label, std::string target, ASCIIPlayer::Lobby* lobby, ASCIIMenus::CallbackFunctionInt function, int arg)
    : Label(label)
    , Target(target)
    , callbackFunctionVoid_(nullptr)
    , callbackFunctionInt_(function)
    , lobby_(lobby)
    , arg_(arg)
    , argType_(ArgType::INT)
  {  }


  void Call()
  {
      switch (argType_)
      {
        case ArgType::NONE:
          if(callbackFunctionVoid_ != nullptr)
            (lobby_->*callbackFunctionVoid_)();
          break;
        case ArgType::INT:
          (lobby_->*callbackFunctionInt_)(arg_);
          break;
      }
  }

  std::string Label;
  std::string Target;

private:
  ASCIIPlayer::Lobby *lobby_;
  ArgType argType_;
  
  ASCIIMenus::CallbackFunctionVoid callbackFunctionVoid_;

  ASCIIMenus::CallbackFunctionInt callbackFunctionInt_;
  int arg_;
};
class Container
{
public:
  // Effective ctor, registers the name for you. Deallocation needed after.
  static Container *Create(ASCIIPlayer::Lobby* lobby, std::string menuName)
  { 
    Container *c = new Container(menuName, lobby);
    MenuRegistry::Register(menuName, c); 
    return c;
  }

  // Member functions
  void AddItem(std::string label, std::string target, ASCIIMenus::CallbackFunctionVoid function = nullptr) 
  { 
    lineItems_.push_back(Selectable(label, target, lobby_, function));
  }

  // Member functions
  void AddItem(std::string label, std::string target, ASCIIMenus::CallbackFunctionInt function, int arg)
  {
    lineItems_.push_back(Selectable(label, target, lobby_, function, arg));
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
  Container(std::string menuName, ASCIIPlayer::Lobby *lobby)
  : selected_(0)
  , name_(menuName)
  , lineItems_()
  , orientation_(ASCIIMenus::Orientation::VERTICAL)
  , x_(0)
  , y_(0)
  , lobby_(lobby)
  {  }

  // Private variables
  size_t selected_;
  std::string name_;
  std::vector<Selectable> lineItems_;
  ASCIIMenus::Orientation orientation_;
  size_t x_;
  size_t y_;
  ASCIIPlayer::Lobby* lobby_;
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
    Container *c = MenuRegistry::GetContainer(initial);
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


