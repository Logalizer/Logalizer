#include "MainFrameImpl.h"
#include <wx/textctrl.h>
#include <wx/wx.h>
#include <wx/wxprec.h>
#include <filesystem>
#include <iostream>
#include "platform/application.h"

namespace fs = std::filesystem;

MainFrameImpl::MainFrameImpl(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                             const wxSize& size, long style)
    : MainFrame(parent, id, title, pos, size, style)
{
   loadConfigs();
}

MainFrameImpl::~MainFrameImpl()
{
}

void MainFrameImpl::loadConfigs()
{
   try {
      fs::path config = getExeDir() / "config";
      for (const auto& entry : fs::directory_iterator(config)) {
         if (entry.path().extension() == ".json") {
            std::cout << entry.path() << std::endl;
            std::string profile_name = entry.path().stem().string();
            m_profileCombo->Append(profile_name);
            profiles_[profile_name] = entry.path().string();
         }
      }
   }
   catch (std::exception& e) {
      std::cout << e.what();
   }
   if (!m_profileCombo->IsListEmpty()) m_profileCombo->SetSelection(0);
}

void MainFrameImpl::onGenerate(wxCommandEvent& event)
{
   const std::string filePath = m_logFilePicker->GetPath().ToStdString();
   const std::string selectedProfile = m_profileCombo->GetStringSelection().ToStdString();
   const std::string profilePath = profiles_[selectedProfile];
   if (filePath.empty()) wxMessageBox(wxT("Choose input file"));
   executeCmd(filePath, profilePath);
}

void MainFrameImpl::executeCmd(const std::string& filePath, const std::string& profilePath)
{
   const std::string logalizerPath = getLogalizerPath().string();
   const std::string command = logalizerPath + " -f " + filePath + " -c " + profilePath;
   const char* command_str = command.c_str();
   std::cout << command_str << std::endl;
   if (const int returnval = system(command_str)) {
      std::cerr << command << " execution failed with code " << returnval << "\n";
   }
}
