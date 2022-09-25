#include <wx/textctrl.h>
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <iostream>
#include <unordered_map>
#include "ui/ui.h"

class MainFrameImpl : public MainFrame {
  public:
   MainFrameImpl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
                 const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300),
                 long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

   ~MainFrameImpl();
   virtual void onGenerate(wxCommandEvent& event);
   void loadConfigs();

  private:
   std::unordered_map<std::string, std::string> profiles_;
   void executeCmd(const std::string& filePath, const std::string& profilePath);
};
