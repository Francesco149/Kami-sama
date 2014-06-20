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

/*
	Hacks AoBs

	GnD: 00 00 00 00 8B 95 ? ? ? ? 89 55 ? 8B 85 ? ? ? ? 50 E8 ? ? ? ? 83 C4 ? 85 C0

	PLoot:
	tubi: 89 86 ? ? ? ? E8 ? ? ? ? 89 86 ? ? ? ? 5E C2 ? ? CC CC CC CC
	instadrop: 0D ? ? ? ? 83 C4 ? E9 ? ? ? ? DD 05 ? ? ? ? DC C9
	nolootanim: 81 FE ? ? ? ? 0F 8D ? ? ? ? 85 ED 0F 84 ? ? ? ? 81 FE ? ? ? ? BF ? ? ? ? 7E ? B8 ? ? ? ? 2B C6

	Unlimited MP: 7D ? 33 FF 81 FB ? ? ? ? 75 ? 8B 6C 24 ? 8B CD
	Full Godmode 1: 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 84 24 ? ? ? ? 64 A3 ? ? ? ? 8B F1 C7 44 24 ? ? ? ? ? 8B 2D ? ? ? ? 8D 84 24
	Full Godmode 2: 55 8D 6C 24 ? 83 EC ? 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC ? A1 ? ? ? ? 33 C5 89 45 ? 53 56 57 50 8D 45 ? 64 A3 ? ? ? ? 8B F9 8B 87 ? ? ? ? 8B 88 ? ? ? ? 51
	Item filter: 8B CF 89 46 ? E8 ? ? ? ? 8B CF 89 46 ? E8 ? ? ? ? 0F B6 D0

	CPU Hack: 
	nobg, noobj, notiles: E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 51 8B C4
	noclouds: 55 8D 6C 24 ? 83 EC ? 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC ? A1 ? ? ? ? 33 C5 89 45 ? 53 56 57 50 8D 45 ? 64 A3 ? ? ? ? 89 4D ? 51
	nodmg:
	7C ? B8 ? ? ? ? E8 ? ? ? ? 8B C4 6A ? 50
	0F 84 ? ? ? ? 2D ? ? ? ? 75 ? 85 DB 74 ? 8B 13
	7E ? 8B 54 24 ? 52 8B 54 24 ? 52 51
	75 ? 8B 44 24 ? 50 6A ? 51
	7D ? 8B 4C 24 ? 8B 54 24 ? 51 52

	Fusion Attack: 89 34 81 40 89 44 24 ? 8B 44 24 ? 85 C0
	Mob Lag: 77 ? 89 54 24 ? E9 ? ? ? ? 8B FF 5D

	Skill Inject:
	0F 84 ? ? ? ? 2B 9E ? ? ? ? 0F 88 ? ? ? ? 8B 86 ? ? ? ? 50
	0F 84 ? ? ? ? 8B 16 8B 42 ? 8B CE FF D0 3D ? ? ? ? 74 ? 3D ? ? ? ? 74 ? 3D ? ? ? ? 74 ? 3D ? ? ? ? 74 ? 3D ? ? ? ? 75 ? 8D 4C 24 ? 51
	75 ? 8B 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 ? 83 F8 ? 0F 85 ? ? ? ? 8B 8E ? ? ? ? 8B 54 24 ? 6A ? 6A ? 6A ? 8D 44 24 ? 50 51
	0F 85 ? ? ? ? 8B 8E ? ? ? ? 8B 54 24 ? 6A ? 6A ? 6A ?
	hook: 8B 8E ? ? ? ? 8B 54 24 ? 6A ? 6A ? 6A ? 8D 44 24 ? 50
	0F 87 ? ? ? ? 0F B6 80 ? ? ? ? FF 24 85 ? ? ? ? 84 DB 74 ? 8B 8E ? ? ? ? 85 C9
	FF 24 85 ? ? ? ? 84 DB 74 ? 8B 8E ? ? ? ? 85 C9
	75 ? C7 85 ? ? ? ? ? ? ? ? EB ? C7 85 ? ? ? ? ? ? ? ? 8B 95 ? ? ? ? 89 55 ? 8B 85 ? ? ? ? 50
	7D ? 33 F6 89 74 24 ? 85 FF 7D ? 33 FF 81 FB ? ? ? ? 75 ? 8B 6C 24 ?
*/

#include "mainform.hpp"
#include "resource.h"
#include "utils.hpp"

#include "itemhook.hpp"
#include "kami.hpp"

#include "gnd.hpp"
#include "perfectloot.hpp"
#include "unlimitedmp.hpp"
#include "fullgodmode.hpp"
#include "itemfilter.hpp"
#include "cpuhack.hpp"
#include "fusionattack.hpp"
#include "moblag.hpp"
#include "skillinject.hpp"

#include <boost/lexical_cast.hpp>

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/slider.h>

#include <list>

//#define SEPARATETHREAD

#define menu_bind(functor, id) \
	Bind(wxEVT_COMMAND_MENU_SELECTED, functor, this, id);

// ------------------------------------                 code width                  -------------------------------->|

namespace kamisama
{
	// {{{
	// app begin
	const wxString app::appname = "Kami-sama";
	const wxString app::appver = "r2";

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
		mainform::init(hInstance, appname, wxDefaultPosition, wxSize(340, 485));
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
		wxStaticBoxSizer *kamibox = new wxStaticBoxSizer(wxVERTICAL, basepanel, "Kami");
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
			kamibox->Add(sizercheckboxes, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
		}

		// hacks groupbox
		wxStaticBoxSizer *hacksbox = new wxStaticBoxSizer(wxVERTICAL, basepanel, "Hacks");
		{
			wxStaticBox *box = hacksbox->GetStaticBox();

			wxGridSizer *grid = new wxGridSizer(4, 5, 5);
			{
				wxCheckBox *tmp;
				std::vector<wxCheckBox * const> checkboxes;

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "GnD"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnGnDClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "P.Loot"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnPLootClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, wxT("\x221E MP")));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnUnlimMPClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "Full GM"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnFullGMClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "Filter"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnFilterClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "CPU Hax"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnCPUClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "FA"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnFAClicked, this);

				checkboxes.push_back(tmp = new wxCheckBox(box, wxID_ANY, "Mob Lag"));
				tmp->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnMobLagClicked, this);

				boost_foreach(wxCheckBox * const & cb, checkboxes)
					grid->Add(cb);
			}

			hacksbox->AddSpacer(5);
			hacksbox->Add(grid, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
		}

		// skill inject groupbox
		wxStaticBoxSizer *skillinjectbox = new wxStaticBoxSizer(wxVERTICAL, basepanel, "Skill Injection");
		{
			wxStaticBox *box = skillinjectbox->GetStaticBox();

			comboskillid = new wxComboBox(box, wxID_ANY, wxEmptyString, wxDefaultPosition, 
				wxDefaultSize, wxArrayString(), wxCB_READONLY);
			{
				wxArrayString choices;

				choices.Add("27121202: Apocalypse (Luminous)");
				choices.Add("27001201: Abyssal Drop (Luminous)");
				choices.Add("27111202: Moonlight Spear (Luminous)");
				choices.Add("61001005: Dragon Slash (Kaiser)");
				choices.Add("61121104: Blade Burst (Kaiser)");
				choices.Add("31121001: Demon Impact (Dragon Slayer)");
				choices.Add("1001005: Slash Blast (Paladin)");
				comboskillid->Append(choices);

				comboskillid->SetClientData(0, reinterpret_cast<void *>(27121202)); // ghetto as fuck
				comboskillid->SetClientData(1, reinterpret_cast<void *>(27001201));
				comboskillid->SetClientData(2, reinterpret_cast<void *>(27111202));
				comboskillid->SetClientData(3, reinterpret_cast<void *>(61001005));
				comboskillid->SetClientData(4, reinterpret_cast<void *>(61121104));
				comboskillid->SetClientData(5, reinterpret_cast<void *>(31121001));
				comboskillid->SetClientData(6, reinterpret_cast<void *>(1001005));

				comboskillid->SetSelection(0);
			}

			wxCheckBox *enable = new wxCheckBox(box, wxID_ANY, "Enable");
			enable->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &mainform::OnSkillInjectClicked, this);

			skillinjectbox->AddSpacer(5);
			skillinjectbox->Add(comboskillid, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
			skillinjectbox->Add(enable, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
		}

		basesizer->Add(kamibox, 0, wxALL | wxEXPAND, 10);
		basesizer->Add(hacksbox, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);
		basesizer->Add(skillinjectbox, 0, wxRIGHT | wxLEFT | wxBOTTOM | wxEXPAND, 10);

		basepanel->SetAutoLayout(true);
		basepanel->SetSizer(basesizer);
		basepanel->Layout(); // fixes the layout snapping into place after the first resize

		try
		{
			// force kami's singleton to init here to avoid lag when first enabling kami
			maple::kami::get();

			// enable hooks that are always active
			if (!maple::itemhook::get()->enable(true))
				wxLogWarning("Failed to enable item hook! Loot will not work.");
		
			// force all hacks singletons to initialize
			maple::gnd::get();
			maple::perfectloot::get();
			maple::unlimitedmp::get();
			maple::fullgodmode::get();
			maple::itemfilter::get();
			maple::cpuhack::get();
			maple::fusionattack::get();
			maple::moblag::get();
			maple::skillinject::get();
		}
		catch (const std::exception &e)
		{
			app::fatal(e.what());
		}

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

		maple::gnd::get()->enable(false);
		maple::perfectloot::get()->enable(false);
		maple::unlimitedmp::get()->enable(false);
		maple::fullgodmode::get()->enable(false);
		maple::itemfilter::get()->enable(false);
		maple::cpuhack::get()->enable(false);
		maple::fusionattack::get()->enable(false);
		maple::moblag::get()->enable(false);
		maple::skillinject::get()->enable(false);

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

	void mainform::OnGnDClicked(wxCommandEvent &e)
	{
		maple::gnd::get()->enable(e.IsChecked());
	}

	void mainform::OnPLootClicked(wxCommandEvent &e)
	{
		maple::perfectloot::get()->enable(e.IsChecked());
	}

	void mainform::OnUnlimMPClicked(wxCommandEvent &e)
	{
		maple::unlimitedmp::get()->enable(e.IsChecked());
	}

	void mainform::OnFullGMClicked(wxCommandEvent &e)
	{
		maple::fullgodmode::get()->enable(e.IsChecked());
	}

	void mainform::OnFilterClicked(wxCommandEvent &e)
	{
		maple::itemfilter::get()->enable(e.IsChecked());
	}

	void mainform::OnCPUClicked(wxCommandEvent &e)
	{
		maple::cpuhack::get()->enable(e.IsChecked());
	}

	void mainform::OnFAClicked(wxCommandEvent &e)
	{
		maple::fusionattack::get()->enable(e.IsChecked());
	}

	void mainform::OnMobLagClicked(wxCommandEvent &e)
	{
		maple::moblag::get()->enable(e.IsChecked());
	}

	void mainform::OnSkillInjectClicked(wxCommandEvent &e)
	{
		boost::shared_ptr<maple::skillinject> psi = maple::skillinject::get();

		if (e.IsChecked())
		{
			dword skillid = reinterpret_cast<dword>(comboskillid->GetClientData(comboskillid->GetSelection()));
			
			if (skillid == 0)
				wxLogFatalError("Something went wrong with the skill id combo box. Report this to the developer");

			psi->setdelay(1);
			psi->setskillid(skillid);
			psi->enable(true);
		}
		else
			psi->enable(false);
	}
	// mainform end
	// }}}
}
