#include <wx/textctrl.h>
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <iostream>
#include "MainFrameImpl.h"

class MyApp : public wxApp {
  public:
   virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
   auto *frame = new MainFrameImpl(nullptr);
   frame->Show(true);
   return true;
}
