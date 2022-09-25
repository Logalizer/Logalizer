///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/colour.h>
#include <wx/combobox.h>
#include <wx/dataview.h>
#include <wx/filepicker.h>
#include <wx/font.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame
///////////////////////////////////////////////////////////////////////////////
class MyFrame : public wxFrame {
  private:
  protected:
  public:
   MyFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 389),
           long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

   ~MyFrame();
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame {
  private:
  protected:
   wxNotebook* m_notebook;
   wxPanel* m_panelGenerate;
   wxStaticText* m_staticTextLogFileTitle;
   wxFilePickerCtrl* m_logFilePicker;
   wxComboBox* m_profileCombo;
   wxComboBox* m_formatCombo;
   wxButton* m_generateButton;
   wxPanel* m_panelConfig;
   wxComboBox* m_comboBox7;
   wxButton* m_button11;
   wxButton* m_button12;
   // wxDataViewListCtrl* m_dataViewListCtrl1;
   wxCheckListBox* m_checkList2;
   // wxDataViewListCtrl* m_dataViewListCtrl2;

   // Virtual event handlers, override them in your derived class
   virtual void onGenerate(wxCommandEvent& event)
   {
      event.Skip();
   }

  public:
   MainFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300),
             long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

   ~MainFrame();
};
