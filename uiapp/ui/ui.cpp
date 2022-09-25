///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ui.h"

///////////////////////////////////////////////////////////////////////////

MyFrame::MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                 long style)
    : wxFrame(parent, id, title, pos, size, style)
{
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);

   this->Centre(wxBOTH);
}

MyFrame::~MyFrame()
{
}

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
                     long style)
    : wxFrame(parent, id, title, pos, size, style)
{
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);

   wxBoxSizer* bMainSizer;
   bMainSizer = new wxBoxSizer(wxVERTICAL);

   m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("Generate"));
   m_panelGenerate = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
   wxBoxSizer* bGenerateMainSizer;
   bGenerateMainSizer = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSelectLogSizer;
   bSelectLogSizer = new wxBoxSizer(wxVERTICAL);

   m_staticTextLogFileTitle =
       new wxStaticText(m_panelGenerate, wxID_ANY, wxT("Select Log File"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticTextLogFileTitle->Wrap(-1);
   bSelectLogSizer->Add(m_staticTextLogFileTitle, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

   m_logFilePicker = new wxFilePickerCtrl(m_panelGenerate, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"),
                                          wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE);
   bSelectLogSizer->Add(m_logFilePicker, 0, wxALL | wxEXPAND, 5);

   bGenerateMainSizer->Add(bSelectLogSizer, 1, wxEXPAND, 5);

   wxBoxSizer* bGenerateBtnSizer;
   bGenerateBtnSizer = new wxBoxSizer(wxHORIZONTAL);

   m_profileCombo =
       new wxComboBox(m_panelGenerate, wxID_ANY, wxT("Select profile"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
   bGenerateBtnSizer->Add(m_profileCombo, 1, wxALL | wxFIXED_MINSIZE, 5);

   m_formatCombo = new wxComboBox(m_panelGenerate, wxID_ANY, wxT("PNG"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
   m_formatCombo->Append(wxT("PNG"));
   m_formatCombo->Append(wxT("SVG"));
   m_formatCombo->SetSelection(0);
   m_formatCombo->Hide();

   bGenerateBtnSizer->Add(m_formatCombo, 0, wxALL | wxFIXED_MINSIZE, 5);

   m_generateButton = new wxButton(m_panelGenerate, wxID_ANY, wxT("Generate"), wxDefaultPosition, wxDefaultSize, 0);
   bGenerateBtnSizer->Add(m_generateButton, 2, wxALL, 5);

   bGenerateMainSizer->Add(bGenerateBtnSizer, 0, wxEXPAND, 5);

   m_panelGenerate->SetSizer(bGenerateMainSizer);
   m_panelGenerate->Layout();
   bGenerateMainSizer->Fit(m_panelGenerate);
   m_notebook->AddPage(m_panelGenerate, wxT("Generate"), true);
   m_panelConfig = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
   wxBoxSizer* bSizer14;
   bSizer14 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer16;
   bSizer16 = new wxBoxSizer(wxVERTICAL);

   m_comboBox7 = new wxComboBox(m_panelConfig, wxID_ANY, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
   bSizer16->Add(m_comboBox7, 0, wxALL | wxEXPAND, 5);

   bSizer14->Add(bSizer16, 0, wxEXPAND, 5);

   wxFlexGridSizer* fgSizer1;
   fgSizer1 = new wxFlexGridSizer(1, 2, 0, 0);
   fgSizer1->AddGrowableCol(0);
   fgSizer1->AddGrowableRow(0);
   fgSizer1->SetFlexibleDirection(wxBOTH);
   fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

   wxBoxSizer* bSizerContent;
   bSizerContent = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizerConfigBtns;
   bSizerConfigBtns = new wxBoxSizer(wxHORIZONTAL);

   m_button11 = new wxButton(m_panelConfig, wxID_ANY, wxT("+"), wxDefaultPosition, wxDefaultSize, 0);
   bSizerConfigBtns->Add(m_button11, 0, wxALIGN_BOTTOM | wxALL, 5);

   m_button12 = new wxButton(m_panelConfig, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0);
   bSizerConfigBtns->Add(m_button12, 0, wxALIGN_BOTTOM | wxALL, 5);

   bSizerContent->Add(bSizerConfigBtns, 0, wxEXPAND, 5);

   // m_dataViewListCtrl1 = new wxDataViewListCtrl( m_panelConfig, wxID_ANY, wxDefaultPosition, wxDefaultSize,
   // wxDV_HORIZ_RULES|wxDV_MULTIPLE|wxDV_ROW_LINES ); bSizerContent->Add( m_dataViewListCtrl1, 1, wxALL|wxEXPAND, 5 );

   fgSizer1->Add(bSizerContent, 2, wxEXPAND, 5);

   wxBoxSizer* bSizerRightPanel;
   bSizerRightPanel = new wxBoxSizer(wxVERTICAL);

   wxArrayString m_checkList2Choices;
   m_checkList2 = new wxCheckListBox(m_panelConfig, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList2Choices, 0);
   bSizerRightPanel->Add(m_checkList2, 1, wxALL | wxEXPAND, 5);

   // m_dataViewListCtrl2 = new wxDataViewListCtrl( m_panelConfig, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
   // bSizerRightPanel->Add( m_dataViewListCtrl2, 1, wxALL|wxEXPAND, 5 );

   fgSizer1->Add(bSizerRightPanel, 1, 0, 5);

   bSizer14->Add(fgSizer1, 1, wxEXPAND, 5);

   m_panelConfig->SetSizer(bSizer14);
   m_panelConfig->Layout();
   bSizer14->Fit(m_panelConfig);
   m_notebook->AddPage(m_panelConfig, wxT("Config Editor"), false);

   bMainSizer->Add(m_notebook, 1, wxEXPAND | wxALL, 5);

   this->SetSizer(bMainSizer);
   this->Layout();

   this->Centre(wxBOTH);

   // Connect Events
   m_generateButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onGenerate), NULL, this);
}

MainFrame::~MainFrame()
{
   // Disconnect Events
   m_generateButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onGenerate), NULL, this);
}
