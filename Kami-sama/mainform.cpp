/*
	Copyright 2014 Francesco "Franc[e]sco" Noferi (francesco149@gmail.com)

	This file is part of Kami-sama.

	Kami-sama is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Kami-sama is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Kami-sama. If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainform.hpp"
#include "resource.h"
#include "utils.h"

#include "itemhook.hpp"
#include "kami.hpp"

#include <boost/lexical_cast.hpp>

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/slider.h>

//#define SEPARATETHREAD

#define menu_bind(functor, id) \
	Bind(wxEVT_COMMAND_MENU_SELECTED, functor, this, id);

// ------------------------------------                 code width                  -------------------------------->|

namespace kamisama
{
	// {{{
	// app begin
	const wxString app::appname = "Kami-sama";
	const wxString app::appver = "r1";

	void app::rundll(HINSTANCE hInstance)
	{
		try
		{
			// placeholder cmd line args
			int argc = 1;
			char *argv[] = { "Kami-sama" };

			// manually run the wxWidgets app
			// (used to deploy as a dll)
			kamisama::app *papp = new kamisama::app(hInstance);
			wxApp::SetInstance(papp);
			wxEntry(argc, argv);
		}
		catch (const std::exception &e)
		{
			fatal(e.what());
		}

		FreeLibraryAndExitThread(hInstance, 0); // unload injected dll
	}

	app::app(HINSTANCE hInstance)
		: wxApp(), 
		  hInstance(hInstance)
	{
		// empty
	}

	app::~app()
	{
		// empty
	}

	bool app::OnInit()
	{
		mainform *frame;

		// create main frame
		mainform::init(hInstance, appname, wxDefaultPosition, wxSize(340, 305));
		frame = mainform::get();

		if (!frame) // out of memory?
		{
			fatal("app::OnInit: could not create frame! Are we out of memory?");
			return false;
		}

		// display top level window
		SetTopWindow(frame); // optional (I think)
		frame->Show(); // makes the main frame visible

		return true;
	}

	void app::fatal(std::string what)
	{
		static const wxString msg(what);
		wxLogFatalError(msg);
	}
	// app end
	// }}}

	// {{{
	// mainform begin
	mainform *mainform::inst;

	void mainform::init(HINSTANCE hInstance, const wxString &title, 
		const wxPoint &pos, const wxSize &size)
	{
		if (inst)
			return;

		inst = new mainform(hInstance, title, pos, size);
	}

	mainform *mainform::get()
	{
		return inst;
	}

	mainform::mainform(HINSTANCE hInstance, const wxString &title, const wxPoint &pos, const wxSize &size)
		: wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER ^ wxMAXIMIZE_BOX), 
		  lootchecked(true), isclientsided(false), autoattack(false)
	{
		wxPanel *basepanel = new wxPanel(this);
		wxBoxSizer *basesizer = new wxBoxSizer(wxVERTICAL);

		// we're on windows, so who cares about dealing with cross-platform icons
		// this is the only way that seems to work to set icon in an injected dll
		HWND hWnd = GetHWND();
		HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

		SendMessage(hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));

		// create menu bar
		wxMenuBar *mbar = new wxMenuBar;

		// file menu
		wxMenu *menu = new wxMenu;
		menu->AppendCheckItem(wxID_FILE_HIDEMAPLE, "Hide MapleStory");
		menu->Append(wxID_FILE_EXIT, "Exit");
		mbar->Append(menu, "File"); // add menu to the menu bar 

		// bind menu events
		menu_bind(&mainform::OnFileHideMapleClicked, wxID_FILE_HIDEMAPLE);
		menu_bind(&mainform::OnFileExitClicked, wxID_FILE_EXIT);

		// help menu
		menu = new wxMenu;
		menu->Append(wxID_HELP_ABOUT, "About");
		mbar->Append(menu, "Help"); // add menu to the menu bar

		menu_bind(&mainform::OnHelpAboutClicked, wxID_HELP_ABOUT);

		SetMenuBar(mbar);
		CreateStatusBar();

		Bind(wxEVT_CLOSE_WINDOW, &mainform::OnClose, this);

		// kami groupbox
		wxStaticBoxSizer *kamibox = new wxStaticBoxSizer(wxVERTICAL,
			basepanel, "Kami");
		{
			wxStaticBox *box = kamibox->GetStaticBox();

			// offsets
			wxStaticText *labelkamioffset = new wxStaticText(box, wxID_ANY, "Offset:");

			wxBoxSizer *sizeroffsets = new wxBoxSizer(wxHORIZONTAL);
			{
				wxStaticText *labelkamioffsetx = new wxStaticText(box, wxID_ANY, "X:");
				wxStaticText *labelkamioffsety = new wxStaticText(box, wxID_ANY, "Y:");

				textkamioffsetx = new wxTextCtrl(box, wxID_ANY, "100");
				textkamioffsety = new wxTextCtrl(box, wxID_ANY, "0");

				sizeroffsets->Add(labelkamioffsetx, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
				sizeroffsets->Add(textkamioffsetx, 1, wxRIGHT | wxEXPAND, 20);
				sizeroffsets->Add(labelkamioffsety, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
				sizeroffsets->Add(textkamioffsety, 1, wxEXPAND, 0);
			}

			// speed slider
			wxStaticText *labelkamispeed = new wxStaticText(box, wxID_ANY, "Speed:");
			labelkamispeedvalue = new wxStaticText(box, wxID_ANY, "100 teleports / second (10.000000 ms)");

			sliderkamispeed = new wxSlider(box, wxID_ANY, 100, 1, 100);
			sliderkamispeed->Bind(wxEVT_SLIDER, &mainform::OnKamiSpeedScroll, this);

			// enable / loot / char block movement
			wxBoxSizer *sizercheckboxes = new wxBoxSizer(wxHORIZONTAL);
			{
				wxCheckBox *checkkamienabled = new wxCheckBox(box, wxID_ANY, "Enable");
				checkkamienabled->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnKamiEnableClicked, this);

				wxCheckBox *checkkamiloot = new wxCheckBox(box, wxID_ANY, "Loot");
				checkkamiloot->SetValue(true);
				checkkamiloot->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnKamiLootClicked, this);

				wxCheckBox *checkkamiblockchar = new wxCheckBox(box, wxID_ANY, "No Mov.");
				checkkamiblockchar->SetValue(true);
				isclientsided = true;
				checkkamiblockchar->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnKamiCSClicked, this);

				wxCheckBox *checkkamiautoattack = new wxCheckBox(box, wxID_ANY, "Spam CTRL");
				checkkamiautoattack->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnKamiAutoAttackClicked, this);

				sizercheckboxes->Add(checkkamienabled, 0, wxRIGHT, 10);
				sizercheckboxes->Add(checkkamiloot, 0, wxRIGHT, 10);
				sizercheckboxes->Add(checkkamiautoattack, 0, wxRIGHT, 10);
				sizercheckboxes->Add(checkkamiblockchar);
			}

			kamibox->AddSpacer(5);
			kamibox->Add(labelkamioffset, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);
			kamibox->Add(sizeroffsets, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
			kamibox->Add(labelkamispeed, 0, wxRIGHT | wxLEFT | wxBOTTOM, 10);
			kamibox->Add(sliderkamispeed, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
			kamibox->Add(labelkamispeedvalue, 0, wxRIGHT | wxLEFT | wxBOTTOM | 
				wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 10);
			kamibox->Add(sizercheckboxes, 0, wxRIGHT | wxLEFT | wxEXPAND, 10);
		}

		basesizer->Add(kamibox, 1, wxALL | wxEXPAND, 10);

		basepanel->SetAutoLayout(true);
		basepanel->SetSizer(basesizer);
		basepanel->Layout(); // fixes the layout snapping into place after the first resize

		// force kami's singleton to init here to avoid lag when first enabling kami
		maple::kami::get();

		// enable hooks that are always active
		if (!maple::itemhook::get()->enable(true))
			wxLogWarning("Failed to enable item hook! Loot will not work.");

		wxLogStatus("Idle.");
	}

	mainform::~mainform()
	{
		// empty
	}

	void mainform::OnClose(wxCloseEvent &e)
	{
		if (e.CanVeto()) // forced termination should not ask to kill maple
		{
			int res = wxMessageBox("Do you also want to close MapleStory?", 
				app::appname, wxICON_INFORMATION | wxYES_NO | wxCANCEL, this);

			if (res == wxYES)
				TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);

			if (res == wxCANCEL)
			{
				e.Veto();
				return;
			}
		}

		// clean up all memory modifications here to avoid crashes
		maple::kami::get()->unhook();

#ifdef SEPARATETHREAD
		maple::kami::get()->stopsticking();
#endif

		if (!maple::itemhook::get()->enable(false))
			wxLogError("Failed to disable item hook! This might cause MapleStory to crash.");

		e.Skip();
	}

	void mainform::OnHelpAboutClicked(wxCommandEvent &e)
	{
		wxString ver = wxString::Format("%s %s", app::appname, app::appver);

		wxMessageBox(
			wxString::Format("%s\n\n"
				"coded by Francesco \"Franc[e]sco\" Noferi\n"
				"http://sabishiimedia.wordpress.com/\n"
				"francesco1149@gmail.com", ver),
			ver, wxICON_INFORMATION | wxOK, this
		);
	}

	void mainform::OnFileHideMapleClicked(wxCommandEvent &e)
	{
		HWND hMoopla = maple::wnd();
		ShowWindow(hMoopla, e.IsChecked() ? SW_HIDE : SW_SHOW);
	}

	void mainform::OnFileExitClicked(wxCommandEvent &e)
	{
		wxLogStatus("Terminating");
		Close(false);
	}

	void mainform::OnKamiSpeedScroll(wxCommandEvent &e)
	{
		float ms = 1000.0 / static_cast<float>(sliderkamispeed->GetValue());

		labelkamispeedvalue->SetLabelText(
			wxString::Format("%d teleports / second (%f ms)", sliderkamispeed->GetValue(), ms)
		);
	}

	void mainform::OnKamiEnableClicked(wxCommandEvent &e)
	{
		if (e.IsChecked())
		{
			long offx, offy;
			float ms = 1000.0 / static_cast<float>(sliderkamispeed->GetValue());
			dword microsecondsdelay = static_cast<dword>(ms * 1000.f);

			try   
			{
				offx = boost::lexical_cast<long>(textkamioffsetx->GetValue().ToStdString());
				offy = boost::lexical_cast<long>(textkamioffsety->GetValue().ToStdString());
			}
			catch(boost::bad_lexical_cast &e)
			{
				wxLogError("Invalid offset: %s.", wxString(e.what()));
				wxLogStatus("Failed to enable Kami.");
				return;
			}

#ifdef SEPARATETHREAD
			if (!isclientsided)
				maple::kami::get()->startsticking(lootchecked, offx, offy, 
					microsecondsdelay, isclientsided, autoattack);
			else
#endif
			maple::kami::get()->hook(lootchecked, offx, offy, microsecondsdelay, isclientsided, autoattack);
			wxLogStatus("Kami enabled.");
		}
		else
		{
			maple::kami::get()->unhook();

#ifdef SEPARATETHREAD
			maple::kami::get()->stopsticking();
#endif
			wxLogStatus("Kami disabled.");
		}
	}

	void mainform::OnKamiLootClicked(wxCommandEvent &e)
	{
		lootchecked = e.IsChecked();
	}

	void mainform::OnKamiCSClicked(wxCommandEvent &e)
	{
		isclientsided = e.IsChecked();
	}

	void mainform::OnKamiAutoAttackClicked(wxCommandEvent &e)
	{
		autoattack = e.IsChecked();
	}
	// mainform end
	// }}}
}
