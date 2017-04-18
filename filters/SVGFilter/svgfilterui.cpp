/*
 * xara-cairo, a vector drawing program
 *
 * Based on Xara LX, Copyright (C) 1993-2006 Xara Group Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 *
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

// svgfilterui.h: This implements the XAR <--> SVG GUI interface

#include "svgfilterui.h"
#include "svgexportdialog.h"

static bool s_bDoVerbose = false;

// Define a new application class
class MyApp : public wxApp
{
public:
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	virtual int OnRun();

public:
	bool m_bDoExport;
	bool m_bCancelled;
	wxString m_sOutFile;
	wxString m_sXMLFile;
};

IMPLEMENT_APP(MyApp)

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, _T("h"), _T("help"),	 _T("show this help message"),
			wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
		{ wxCMD_LINE_OPTION, _T("f"), _T("file"),	 _T("output filename") },
		{ wxCMD_LINE_OPTION, _T("x"), _T("xmlfile"), _T("xml configuration filename") },
		{ wxCMD_LINE_SWITCH, _T("i"), _T("import"),	 _T("specify import action") },
		{ wxCMD_LINE_SWITCH, _T("e"), _T("export"),  _T("specify export action") },
		{ wxCMD_LINE_NONE }
	};

	parser.SetDesc(cmdLineDesc);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	s_bDoVerbose = parser.Found(_T("v"));

	if (!parser.Found(_T("f"), &m_sOutFile))
	{
		ReportError(_T("A filename must be specified with -f"));
		return false;
	}

	if (!parser.Found(_T("x"), &m_sXMLFile))
	{
		ReportError(_T("An XML filename must be specified with -x"));
		return false;
	}
	if (!parser.Found(_T("i")) && !parser.Found(_T("e")))
	{
		ReportError(_T("An action must be specified with -i or -e"));
		return false;
	}
	m_bDoExport = parser.Found(_T("e"));

	// Open the output file for writing
	wxFileOutputStream OutFile(m_sXMLFile);
	if (!OutFile.Ok())
	{
		ReportError(_T("Failed to open output XML file."));
		// Set an appropriate error here
		return false;
	}
	wxTextOutputStream OutText(OutFile);

	OutText << _T("<XPFilterConfig>\n");
	OutText << _T("<Private ");

	m_bCancelled = false;
	if (m_bDoExport) {
		SVGExportDialog *dlg = new SVGExportDialog(NULL);
		if (dlg->ShowModal() != wxID_OK) {
			// user cancelled export
			m_bCancelled = true;
		} else {
			// output exporter settings
			if (dlg->m_SVGVersionComboBox->GetSelection() == 0)
				OutText << _T("version=\"1.1\" ");
			else
				OutText << _T("version=\"1.2\" ");
			if (dlg->m_VerboseCheckBox->IsChecked())
				OutText << _T("verbose=\"1\" ");
			else
				OutText << _T("verbose=\"0\" ");
		}
		dlg->Destroy();
	}
	OutText << _T("/>\n");
	OutText << _T("</XPFilterConfig>\n");

    return true;
}

int MyApp::OnRun()
{
	if (m_bCancelled)	// User cancelled export dialog
		return 1;

	return 0;			// Worked
}

void ReportError(const wxChar* pStr)
{
	wxFprintf(stderr, _T("ERROR:%s\n"), pStr);
}

void ReportWarning(const wxChar* pStr)
{
	wxFprintf(stderr, _T("WARNING:%s\n"), pStr);
}

void ReportMessage(const wxChar* pStr)
{
	if (s_bDoVerbose)
		wxFprintf(stderr, _T("MESSAGE:%s\n"), pStr);
}
