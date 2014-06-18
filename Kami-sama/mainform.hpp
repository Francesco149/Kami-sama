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

#pragma once

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>

namespace kamisama
{
	// represents the application and all of its windows as a whole
	// contains global information about the application and is 
	// responsible for initializing the program on startup
	class app : public wxApp
	{
	public:
		static const wxString appname; // application name
		static const wxString appver; // application version

		app(HINSTANCE hInstance);
		virtual ~app();
		static void rundll(HINSTANCE hInstance); // deploys the application as a dll
		HINSTANCE getinstance();
		virtual bool OnInit(); // called on startup
		static void fatal(std::string what); // displays a fatal error message and terminates the program

	protected:
		HINSTANCE hInstance;
	};

	class mainform : public wxFrame
	{
	public:
		static void init(HINSTANCE hInstance, const wxString &title, 
			const wxPoint &pos, const wxSize &size); // initializes the singleton
		static mainform *get(); // returns the singleton instance
		virtual ~mainform();

	protected:
		// menu ids, used for event handling
		enum
		{
			// file
		    wxID_FILE_EXIT = 1,
			wxID_FILE_HIDEMAPLE, 

			// help
		    wxID_HELP_ABOUT
		};

		static mainform *inst;
		wxTextCtrl *textkamioffsetx;
		wxTextCtrl *textkamioffsety;
		wxSlider *sliderkamispeed;
		wxStaticText *labelkamispeedvalue;
		bool lootchecked;
		bool isclientsided;
		bool autoattack;

		mainform(HINSTANCE hInstance, const wxString &title, const wxPoint &pos, const wxSize &size);

		void OnClose(wxCloseEvent &e);
		void OnFileHideMapleClicked(wxCommandEvent &e);
		void OnFileExitClicked(wxCommandEvent &e);
		void OnHelpAboutClicked(wxCommandEvent &e);

		void OnKamiSpeedScroll(wxCommandEvent &e);
		void OnKamiEnableClicked(wxCommandEvent &e);
		void OnKamiLootClicked(wxCommandEvent &e);
		void OnKamiCSClicked(wxCommandEvent &e);
		void OnKamiAutoAttackClicked(wxCommandEvent &e);

		void OnGnDClicked(wxCommandEvent &e);
		void OnPLootClicked(wxCommandEvent &e);
		void OnUnlimMPClicked(wxCommandEvent &e);
		void OnFullGMClicked(wxCommandEvent &e);
		void OnFilterClicked(wxCommandEvent &e);
		void OnCPUClicked(wxCommandEvent &e);
		void OnFAClicked(wxCommandEvent &e);
		void OnMobLagClicked(wxCommandEvent &e);
		void OnSkillInjectClicked(wxCommandEvent &e);
	};
}
