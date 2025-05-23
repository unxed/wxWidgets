/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        textctrl.cpp
// Purpose:     part of the widgets sample showing wxTextCtrl
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/timer.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/dcclient.h"
    #include "wx/radiobox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/text.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    TextPage_Reset = wxID_HIGHEST,

    TextPage_Set,
    TextPage_Add,
    TextPage_Insert,
    TextPage_Clear,
    TextPage_Load,

    TextPage_StreamRedirector,

    TextPage_Password,
    TextPage_NoVertScrollbar,
    TextPage_WrapLines,
    TextPage_Textctrl
};

// textctrl line number radiobox values
enum TextLines
{
    TextLines_Single,
    TextLines_Multi,
    TextLines_Max
};

// wrap style radio box
enum WrapStyle
{
    WrapStyle_None,
    WrapStyle_Word,
    WrapStyle_Char,
    WrapStyle_Best,
    WrapStyle_Max
};

// Alignment style radio box
enum AlignmentStyle
{
    Align_Left,
    Align_Center,
    Align_Right,
};

#ifdef __WXMSW__

// textctrl kind values
enum TextKind
{
    TextKind_Plain,
    TextKind_Rich,
    TextKind_Rich2,
    TextKind_Max
};

#endif // __WXMSW__

// default values for the controls
static const struct ControlValues
{
    TextLines textLines;

    bool password;
    bool readonly;
    bool processEnter;
    bool processTab;
    bool filename;
    bool noVertScrollbar;

    WrapStyle wrapStyle;
    AlignmentStyle alignmentStyle;

#ifdef __WXMSW__
    TextKind textKind;
#endif // __WXMSW__
} DEFAULTS =
{
    TextLines_Multi,    // multiline
    false,              // not password
    false,              // not readonly
    true,               // do process enter
    false,              // do not process Tab
    false,              // not filename
    false,              // don't hide vertical scrollbar
    WrapStyle_Word,     // wrap on word boundaries
    Align_Left,         // leading-alignment
#ifdef __WXMSW__
    TextKind_Plain      // plain EDIT control
#endif // __WXMSW__
};

// ----------------------------------------------------------------------------
// TextWidgetsPage
// ----------------------------------------------------------------------------

// Define a new frame type: this is going to be our main frame
class TextWidgetsPage : public WidgetsPage
{
public:
    // ctor(s) and dtor
    TextWidgetsPage(WidgetsBookCtrl *book, wxVector<wxBitmapBundle>& imaglist);

    virtual wxWindow *GetWidget() const override { return m_text; }
    virtual wxTextEntryBase *GetTextEntry() const override { return m_text; }
    virtual void RecreateWidget() override { CreateText(); }

    // lazy creation of the content
    virtual void CreateContent() override;

protected:
    // create an info text contorl
    wxTextCtrl *CreateInfoText(wxWindow* parent);

    // create a horz sizer holding a static text and this text control
    wxSizer *CreateTextWithLabelSizer(const wxString& label,
                                      wxTextCtrl *text,
                                      const wxString& label2 = wxEmptyString,
                                      wxTextCtrl *text2 = nullptr,
                                      wxWindow* statBoxParent = nullptr);

    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonClearLog(wxCommandEvent& event);

    void OnButtonSet(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonInsert(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonLoad(wxCommandEvent& event);

    void OnStreamRedirector(wxCommandEvent& event);
    void OnButtonQuit(wxCommandEvent& event);

    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTextPasted(wxClipboardTextEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnUpdateUIClearButton(wxUpdateUIEvent& event);

    void OnUpdateUIPasswordCheckbox(wxUpdateUIEvent& event);
    void OnUpdateUINoVertScrollbarCheckbox(wxUpdateUIEvent& event);
    void OnUpdateUIWrapLinesRadiobox(wxUpdateUIEvent& event);

    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    void OnIdle(wxIdleEvent& event);

    // reset the textctrl parameters
    void Reset();

    // (re)create the textctrl
    void CreateText();

    // is the control currently single line?
    bool IsSingleLine() const
    {
        return m_radioTextLines->GetSelection() == TextLines_Single;
    }

    // the controls
    // ------------

    // the radiobox to choose between single and multi line
    wxRadioBox *m_radioTextLines;

    // and another one to choose the wrapping style
    wxRadioBox *m_radioWrap;

    // and yet another one to choose the alignment style
    wxRadioBox *m_radioAlign;

    // the checkboxes controlling text ctrl styles
    wxCheckBox *m_chkPassword,
               *m_chkReadonly,
               *m_chkProcessEnter,
               *m_chkProcessTab,
               *m_chkFilename,
               *m_chkNoVertScrollbar;

    // under MSW we test rich edit controls as well here
#ifdef __WXMSW__
    wxRadioBox *m_radioKind;
#endif // __WXMSW__

    // the textctrl itself and the sizer it is in
    wxTextCtrl *m_text;
    wxStaticBoxSizer *m_sizerText;

    // the information text zones
    wxTextCtrl *m_textPosCur,
               *m_textRowCur,
               *m_textColCur,
               *m_textPosLast,
               *m_textLineLast,
               *m_textSelFrom,
               *m_textSelTo,
               *m_textRange;

    // and the data to show in them
    long m_posCur,
         m_posLast,
         m_selFrom,
         m_selTo;

    wxString m_range10_20;

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
    DECLARE_WIDGETS_PAGE(TextWidgetsPage)
};

// ----------------------------------------------------------------------------
// WidgetsTextCtrl
// ----------------------------------------------------------------------------

class WidgetsTextCtrl : public wxTextCtrl
{
public:
    WidgetsTextCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& value,
                    int flags)
        : wxTextCtrl(parent, id, value, wxDefaultPosition, wxDefaultSize, flags)
    {
        Bind(wxEVT_LEFT_DOWN, &WidgetsTextCtrl::OnLeftClick, this);
    }

private:
    // Show the result of HitTest() at the mouse position if Alt is pressed.
    void OnLeftClick(wxMouseEvent& event)
    {
        event.Skip();
        if ( !event.AltDown() )
            return;

        wxString where;
        wxTextCoord x, y;
        switch ( HitTest(event.GetPosition(), &x, &y) )
        {
            default:
                wxFAIL_MSG( "unexpected HitTest() result" );
                wxFALLTHROUGH;

            case wxTE_HT_UNKNOWN:
                x = y = -1;
                where = "nowhere near";
                break;

            case wxTE_HT_BEFORE:
                where = "before";
                break;

            case wxTE_HT_BELOW:
                where = "below";
                break;

            case wxTE_HT_BEYOND:
                where = "beyond";
                break;

            case wxTE_HT_ON_TEXT:
                where = "at";
                break;
        }

        wxLogMessage("Mouse is %s (%ld, %ld)", where, x, y);
    }
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(TextWidgetsPage, WidgetsPage)
    EVT_IDLE(TextWidgetsPage::OnIdle)

    EVT_BUTTON(TextPage_Reset, TextWidgetsPage::OnButtonReset)

    EVT_BUTTON(TextPage_StreamRedirector, TextWidgetsPage::OnStreamRedirector)

    EVT_BUTTON(TextPage_Clear, TextWidgetsPage::OnButtonClear)
    EVT_BUTTON(TextPage_Set, TextWidgetsPage::OnButtonSet)
    EVT_BUTTON(TextPage_Add, TextWidgetsPage::OnButtonAdd)
    EVT_BUTTON(TextPage_Insert, TextWidgetsPage::OnButtonInsert)
    EVT_BUTTON(TextPage_Load, TextWidgetsPage::OnButtonLoad)

    EVT_UPDATE_UI(TextPage_Clear, TextWidgetsPage::OnUpdateUIClearButton)

    EVT_UPDATE_UI(TextPage_Password, TextWidgetsPage::OnUpdateUIPasswordCheckbox)
    EVT_UPDATE_UI(TextPage_NoVertScrollbar, TextWidgetsPage::OnUpdateUINoVertScrollbarCheckbox)
    EVT_UPDATE_UI(TextPage_WrapLines, TextWidgetsPage::OnUpdateUIWrapLinesRadiobox)

    EVT_UPDATE_UI(TextPage_Reset, TextWidgetsPage::OnUpdateUIResetButton)

    EVT_TEXT(TextPage_Textctrl, TextWidgetsPage::OnText)
    EVT_TEXT_ENTER(TextPage_Textctrl, TextWidgetsPage::OnTextEnter)
    EVT_TEXT_PASTE(TextPage_Textctrl, TextWidgetsPage::OnTextPasted)

    EVT_CHECKBOX(wxID_ANY, TextWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, TextWidgetsPage::OnCheckOrRadioBox)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXX11__)
    #define FAMILY_CTRLS NATIVE_CTRLS
#elif defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(TextWidgetsPage, "Text",
                       FAMILY_CTRLS | EDITABLE_CTRLS
                       );

// ----------------------------------------------------------------------------
// TextWidgetsPage creation
// ----------------------------------------------------------------------------

TextWidgetsPage::TextWidgetsPage(WidgetsBookCtrl *book, wxVector<wxBitmapBundle>& imaglist)
               : WidgetsPage(book, imaglist, text_xpm)
{
    // init everything
#ifdef __WXMSW__
    m_radioKind =
#endif // __WXMSW__
    m_radioWrap =
    m_radioAlign =
    m_radioTextLines = nullptr;

    m_chkPassword =
    m_chkReadonly =
    m_chkProcessEnter =
    m_chkProcessTab =
    m_chkFilename =
    m_chkNoVertScrollbar = nullptr;

    m_text =
    m_textPosCur =
    m_textRowCur =
    m_textColCur =
    m_textPosLast =
    m_textLineLast =
    m_textSelFrom =
    m_textSelTo =
    m_textRange = nullptr;

    m_sizerText = nullptr;

    m_posCur =
    m_posLast =
    m_selFrom =
    m_selTo = -2; // not -1 which means "no selection"
}

void TextWidgetsPage::CreateContent()
{
    // left pane
    static const wxString modes[] =
    {
        "single line",
        "multi line",
    };

    wxStaticBoxSizer *sizerLeft = new wxStaticBoxSizer(wxVERTICAL, this, "&Set textctrl parameters");
    wxStaticBox* const sizerLeftBox = sizerLeft->GetStaticBox();

    m_radioTextLines = new wxRadioBox(sizerLeftBox, wxID_ANY, "&Number of lines:",
                                      wxDefaultPosition, wxDefaultSize,
                                      WXSIZEOF(modes), modes,
                                      1, wxRA_SPECIFY_COLS);

    sizerLeft->Add(m_radioTextLines, 0, wxGROW | wxALL, 5);
    sizerLeft->AddSpacer(5);

    m_chkPassword = CreateCheckBoxAndAddToSizer(
                        sizerLeft, "&Password control", TextPage_Password, sizerLeftBox
                    );
    m_chkReadonly = CreateCheckBoxAndAddToSizer(
                        sizerLeft, "&Read-only mode", wxID_ANY, sizerLeftBox
                    );
    m_chkProcessEnter = CreateCheckBoxAndAddToSizer(
                        sizerLeft, "Process &Enter", wxID_ANY, sizerLeftBox
                    );
    m_chkProcessTab = CreateCheckBoxAndAddToSizer(
                        sizerLeft, "Process &Tab", wxID_ANY, sizerLeftBox
                    );
    m_chkFilename = CreateCheckBoxAndAddToSizer(
                        sizerLeft, "&Filename control", wxID_ANY, sizerLeftBox
                    );
    m_chkNoVertScrollbar = CreateCheckBoxAndAddToSizer(
                        sizerLeft, "No &vertical scrollbar",
                        TextPage_NoVertScrollbar, sizerLeftBox
                    );
    m_chkFilename->Disable(); // not implemented yet
    sizerLeft->AddSpacer(5);

    static const wxString wrap[] =
    {
        "no wrap",
        "word wrap",
        "char wrap",
        "best wrap",
    };

    m_radioWrap = new wxRadioBox(sizerLeftBox, TextPage_WrapLines, "&Wrap style:",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(wrap), wrap,
                                 1, wxRA_SPECIFY_COLS);
    sizerLeft->Add(m_radioWrap, 0, wxGROW | wxALL, 5);

    static const wxString halign[] =
    {
        "left",
        "centre",
        "right",
    };

    m_radioAlign = new wxRadioBox(sizerLeftBox, wxID_ANY, "&Text alignment",
                                    wxDefaultPosition, wxDefaultSize,
                                    WXSIZEOF(halign), halign, 1);
    sizerLeft->Add(m_radioAlign, 0, wxGROW | wxALL, 5);

#ifdef __WXMSW__
    static const wxString kinds[] =
    {
        "plain edit",
        "rich edit",
        "rich edit 2.0",
    };

    m_radioKind = new wxRadioBox(sizerLeftBox, wxID_ANY, "Control &kind",
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(kinds), kinds,
                                 1, wxRA_SPECIFY_COLS);

    sizerLeft->AddSpacer(5);
    sizerLeft->Add(m_radioKind, 0, wxGROW | wxALL, 5);
#endif // __WXMSW__

    wxButton *btn = new wxButton(sizerLeftBox, TextPage_Reset, "&Reset");
    sizerLeft->Add(2, 2, 0, wxGROW | wxALL, 1); // spacer
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBoxSizer *sizerMiddleUp = new wxStaticBoxSizer(wxVERTICAL, this, "&Change contents:");
    wxStaticBox* const sizerMiddleUpBox = sizerMiddleUp->GetStaticBox();

    btn = new wxButton(sizerMiddleUpBox, TextPage_Set, "&Set text value");
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(sizerMiddleUpBox, TextPage_Add, "&Append text");
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(sizerMiddleUpBox, TextPage_Insert, "&Insert text");
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(sizerMiddleUpBox, TextPage_Load, "&Load file");
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(sizerMiddleUpBox, TextPage_Clear, "&Clear");
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    btn = new wxButton(sizerMiddleUpBox, TextPage_StreamRedirector, "St&ream redirection");
    sizerMiddleUp->Add(btn, 0, wxALL | wxGROW, 1);

    wxStaticBoxSizer *sizerMiddleDown = new wxStaticBoxSizer(wxVERTICAL, this, "&Info:");
    wxStaticBox* const sizerMiddleDownBox = sizerMiddleDown->GetStaticBox();

    m_textPosCur = CreateInfoText(sizerMiddleDownBox);
    m_textRowCur = CreateInfoText(sizerMiddleDownBox);
    m_textColCur = CreateInfoText(sizerMiddleDownBox);

    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    "Current pos:",
                    m_textPosCur, "", nullptr, sizerMiddleDownBox
                  ),
                  0, wxRIGHT, 5);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    "Col:",
                    m_textColCur, "", nullptr, sizerMiddleDownBox
                  ),
                  0, wxLEFT | wxRIGHT, 5);
    sizerRow->Add(CreateTextWithLabelSizer
                  (
                    "Row:",
                    m_textRowCur, "", nullptr, sizerMiddleDownBox
                  ),
                  0, wxLEFT, 5);
    sizerMiddleDown->Add(sizerRow, 0, wxALL, 5);

    m_textLineLast = CreateInfoText(sizerMiddleDownBox);
    m_textPosLast = CreateInfoText(sizerMiddleDownBox);
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          "Number of lines:",
                          m_textLineLast,
                          "Last position:",
                          m_textPosLast,
                          sizerMiddleDownBox
                        ),
                        0, wxALL, 5
                     );

    m_textSelFrom = CreateInfoText(sizerMiddleDownBox);
    m_textSelTo = CreateInfoText(sizerMiddleDownBox);
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          "Selection: from",
                          m_textSelFrom,
                          "to",
                          m_textSelTo,
                          sizerMiddleDownBox
                        ),
                        0, wxALL, 5
                     );

    m_textRange = new wxTextCtrl(sizerMiddleDownBox, wxID_ANY, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_READONLY);
    sizerMiddleDown->Add
                     (
                        CreateTextWithLabelSizer
                        (
                          "Range 10..20:",
                          m_textRange,
                          "", nullptr, sizerMiddleDownBox
                        ),
                        0, wxALL, 5
                     );

    sizerMiddleDown->Add
                     (
                          new wxStaticText
                          (
                            sizerMiddleDownBox,
                            wxID_ANY,
                            "Alt-click in the text to see HitTest() result"
                          ),
                          wxSizerFlags().Border()
                     );

    wxSizer *sizerMiddle = new wxBoxSizer(wxVERTICAL);
    sizerMiddle->Add(sizerMiddleUp, 0, wxGROW);
    sizerMiddle->Add(sizerMiddleDown, 1, wxGROW | wxTOP, 5);

    // right pane
    m_sizerText = new wxStaticBoxSizer(wxHORIZONTAL, this, "&Text:");
    Reset();
    CreateText();
    m_sizerText->SetMinSize(150, 0);

    // the 3 panes panes compose the upper part of the window
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(m_sizerText, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// creation helpers
// ----------------------------------------------------------------------------

wxTextCtrl *TextWidgetsPage::CreateInfoText(wxWindow* parent)
{
    static int s_maxWidth = 0;
    if ( !s_maxWidth )
    {
        // calc it once only
        GetTextExtent("9999999", &s_maxWidth, nullptr);
    }

    wxTextCtrl *text = new wxTextCtrl(parent, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition,
                                      wxSize(s_maxWidth, wxDefaultCoord),
                                      wxTE_READONLY);
    return text;
}

wxSizer *TextWidgetsPage::CreateTextWithLabelSizer(const wxString& label,
                                                 wxTextCtrl *text,
                                                 const wxString& label2,
                                                 wxTextCtrl *text2,
                                                 wxWindow* statBoxParent)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    sizerRow->Add(new wxStaticText(statBoxParent ? statBoxParent : this, wxID_ANY, label), 0,
                  wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
    sizerRow->Add(text, 0, wxALIGN_CENTRE_VERTICAL);
    if ( text2 )
    {
        sizerRow->Add(new wxStaticText(statBoxParent ? statBoxParent : this, wxID_ANY, label2), 0,
                      wxALIGN_CENTRE_VERTICAL | wxLEFT | wxRIGHT, 5);
        sizerRow->Add(text2, 0, wxALIGN_CENTRE_VERTICAL);
    }

    return sizerRow;
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void TextWidgetsPage::Reset()
{
    m_radioTextLines->SetSelection(DEFAULTS.textLines);

    m_chkPassword->SetValue(DEFAULTS.password);
    m_chkReadonly->SetValue(DEFAULTS.readonly);
    m_chkProcessEnter->SetValue(DEFAULTS.processEnter);
    m_chkProcessTab->SetValue(DEFAULTS.processTab);
    m_chkFilename->SetValue(DEFAULTS.filename);
    m_chkNoVertScrollbar->SetValue(DEFAULTS.noVertScrollbar);

    m_radioWrap->SetSelection(DEFAULTS.wrapStyle);
    m_radioAlign->SetSelection(DEFAULTS.alignmentStyle);

#ifdef __WXMSW__
    m_radioKind->SetSelection(DEFAULTS.textKind);
#endif // __WXMSW__
}

void TextWidgetsPage::CreateText()
{
    int flags = GetAttrs().m_defaultFlags;
    switch ( m_radioTextLines->GetSelection() )
    {
        default:
            wxFAIL_MSG( "unexpected lines radio box selection" );
            wxFALLTHROUGH;

        case TextLines_Single:
            break;

        case TextLines_Multi:
            flags |= wxTE_MULTILINE;
            m_chkPassword->SetValue(false);
            break;
    }

    if ( m_chkPassword->GetValue() )
        flags |= wxTE_PASSWORD;
    if ( m_chkReadonly->GetValue() )
        flags |= wxTE_READONLY;
    if ( m_chkProcessEnter->GetValue() )
        flags |= wxTE_PROCESS_ENTER;
    if ( m_chkProcessTab->GetValue() )
        flags |= wxTE_PROCESS_TAB;
    if ( m_chkNoVertScrollbar->GetValue() )
        flags |= wxTE_NO_VSCROLL;

    switch ( m_radioWrap->GetSelection() )
    {
        default:
            wxFAIL_MSG( "unexpected wrap style radio box selection" );
            wxFALLTHROUGH;

        case WrapStyle_None:
            flags |= wxTE_DONTWRAP; // same as wxHSCROLL
            break;

        case WrapStyle_Word:
            flags |= wxTE_WORDWRAP;
            break;

        case WrapStyle_Char:
            flags |= wxTE_CHARWRAP;
            break;

        case WrapStyle_Best:
            // this is default but use symbolic file name for consistency
            flags |= wxTE_BESTWRAP;
            break;
    }

    switch ( m_radioAlign->GetSelection() )
    {
        case Align_Left:
            flags |= wxTE_LEFT;
            break;
        case Align_Center:
            flags |= wxTE_CENTER;
            break;
        case Align_Right:
            flags |= wxTE_RIGHT;
            break;
        default:
            wxFAIL_MSG( "unexpected alignment style radio box selection" );
    }

#ifdef __WXMSW__
    switch ( m_radioKind->GetSelection() )
    {
        default:
            wxFAIL_MSG( "unexpected kind radio box selection" );
            wxFALLTHROUGH;

        case TextKind_Plain:
            break;

        case TextKind_Rich:
            flags |= wxTE_RICH;
            break;

        case TextKind_Rich2:
            flags |= wxTE_RICH2;
            break;
    }
#endif // __WXMSW__

    wxString valueOld;
    if ( m_text )
    {
        valueOld = m_text->GetValue();

        m_sizerText->Detach( m_text );
        delete m_text;
    }
    else
    {
        valueOld = "Hello, Universe!";
    }

    m_text = new WidgetsTextCtrl(m_sizerText->GetStaticBox(), TextPage_Textctrl, valueOld, flags);

    NotifyWidgetRecreation(m_text);

#if 0
    if ( m_chkFilename->GetValue() )
        ;
#endif // TODO

    // cast to int needed to silence gcc warning about different enums
    m_sizerText->Add(m_text, 1, wxALL |
                     (flags & wxTE_MULTILINE ? (int)wxGROW
                                             : wxALIGN_TOP), 5);
    m_sizerText->Layout();
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void TextWidgetsPage::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    // update all info texts

    if ( m_textPosCur )
    {
        long posCur = m_text->GetInsertionPoint();
        if ( posCur != m_posCur )
        {
            m_textPosCur->Clear();
            m_textRowCur->Clear();
            m_textColCur->Clear();

            long col, row;
            m_text->PositionToXY(posCur, &col, &row);

            *m_textPosCur << posCur;
            *m_textRowCur << row;
            *m_textColCur << col;

            m_posCur = posCur;
        }
    }

    if ( m_textPosLast )
    {
        long posLast = m_text->GetLastPosition();
        if ( posLast != m_posLast )
        {
            m_textPosLast->Clear();
            *m_textPosLast << posLast;

            m_posLast = posLast;
        }
    }

    if ( m_textLineLast )
    {
        m_textLineLast->SetValue(
                wxString::Format("%d", m_text->GetNumberOfLines()) );
    }

    if ( m_textSelFrom && m_textSelTo )
    {
        long selFrom, selTo;
        m_text->GetSelection(&selFrom, &selTo);
        if ( selFrom != m_selFrom )
        {
            m_textSelFrom->Clear();
            *m_textSelFrom << selFrom;

            m_selFrom = selFrom;
        }

        if ( selTo != m_selTo )
        {
            m_textSelTo->Clear();
            *m_textSelTo << selTo;

            m_selTo = selTo;
        }
    }

    if ( m_textRange )
    {
        wxString range = m_text->GetRange(10, 20);
        if ( range != m_range10_20 )
        {
            m_range10_20 = range;
            m_textRange->SetValue(range);
        }
    }
}

void TextWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateText();
}

void TextWidgetsPage::OnButtonSet(wxCommandEvent& WXUNUSED(event))
{
    m_text->SetValue(m_text->GetWindowStyle() & wxTE_MULTILINE
                        ? "Here,\nthere and\neverywhere"
                        : "Yellow submarine");

    m_text->SetFocus();
}

void TextWidgetsPage::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    if ( m_text->GetWindowStyle() & wxTE_MULTILINE )
    {
        m_text->AppendText("We all live in a\n");
    }

    m_text->AppendText("Yellow submarine");
}

void TextWidgetsPage::OnButtonInsert(wxCommandEvent& WXUNUSED(event))
{
    m_text->WriteText("Is there anybody going to listen to my story");
    if ( m_text->GetWindowStyle() & wxTE_MULTILINE )
    {
        m_text->WriteText("\nall about the girl who came to stay");
    }
}

void TextWidgetsPage::OnButtonClear(wxCommandEvent& WXUNUSED(event))
{
    m_text->Clear();
    m_text->SetFocus();
}

void TextWidgetsPage::OnButtonLoad(wxCommandEvent& WXUNUSED(event))
{
    // search for the file in several dirs where it's likely to be
    wxPathList pathlist;
    pathlist.Add(".");
    pathlist.Add("..");
    pathlist.Add("../widgets");
    pathlist.Add("../../../samples/widgets");

    wxString filename = pathlist.FindValidPath("textctrl.cpp");
    if ( filename.empty() )
    {
        wxLogError("File textctrl.cpp not found.");
    }
    else // load it
    {
        wxStopWatch sw;
        if ( !m_text->LoadFile(filename) )
        {
            // this is not supposed to happen ...
            wxLogError("Error loading file.");
        }
        else
        {
            long elapsed = sw.Time();
            wxLogMessage("Loaded file '%s' in %lu.%us",
                         filename, elapsed / 1000,
                         (unsigned int) elapsed % 1000);
        }
    }
}

void TextWidgetsPage::OnUpdateUIClearButton(wxUpdateUIEvent& event)
{
    event.Enable(!m_text->GetValue().empty());
}

void TextWidgetsPage::OnUpdateUIWrapLinesRadiobox(wxUpdateUIEvent& event)
{
    event.Enable( !IsSingleLine() );
}

void TextWidgetsPage::OnUpdateUIPasswordCheckbox(wxUpdateUIEvent& event)
{
    // can't put multiline control in password mode
    event.Enable( IsSingleLine() );
}

void TextWidgetsPage::OnUpdateUINoVertScrollbarCheckbox(wxUpdateUIEvent& event)
{
    // Vertical scrollbar creation can be blocked only in multiline control
    event.Enable( !IsSingleLine());
}

void TextWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( (m_radioTextLines->GetSelection() != DEFAULTS.textLines) ||
#ifdef __WXMSW__
                  (m_radioKind->GetSelection() != DEFAULTS.textKind) ||
#endif // __WXMSW__
                  (m_chkPassword->GetValue() != DEFAULTS.password) ||
                  (m_chkReadonly->GetValue() != DEFAULTS.readonly) ||
                  (m_chkProcessEnter->GetValue() != DEFAULTS.processEnter) ||
                  (m_chkProcessTab->GetValue() != DEFAULTS.processTab) ||
                  (m_chkFilename->GetValue() != DEFAULTS.filename) ||
                  (m_chkNoVertScrollbar->GetValue() != DEFAULTS.noVertScrollbar) ||
                  (m_radioWrap->GetSelection() != DEFAULTS.wrapStyle) );
}

void TextWidgetsPage::OnText(wxCommandEvent& event)
{
    if ( !IsUsingLogWindow() )
        return;

    // Replace middle of long text with ellipsis just to avoid filling up the
    // log control with too much unnecessary stuff.
    wxLogMessage("Text control value changed (now '%s')",
                 wxControl::Ellipsize
                 (
                    event.GetString(),
                    wxClientDC(this),
                    wxELLIPSIZE_MIDDLE,
                    GetTextExtent('W').x*100
                 ));
}

void TextWidgetsPage::OnTextEnter(wxCommandEvent& event)
{
    wxLogMessage("Text entered: '%s'", event.GetString());
    event.Skip();
}

void TextWidgetsPage::OnTextPasted(wxClipboardTextEvent& event)
{
    wxLogMessage("Text pasted from clipboard.");
    event.Skip();
}

void TextWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& event)
{
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXOSX__)
    // We should be able to change text alignment
    // dynamically, without recreating the control.
    if( event.GetEventObject() == m_radioAlign )
    {
        long flags = m_text->GetWindowStyle();
        flags &= ~(wxTE_LEFT|wxTE_CENTER|wxTE_RIGHT);

        switch ( event.GetSelection() )
        {
            case Align_Left:
                flags |= wxTE_LEFT;
                break;
            case Align_Center:
                flags |= wxTE_CENTER;
                break;
            case Align_Right:
                flags |= wxTE_RIGHT;
                break;
            default:
                wxFAIL_MSG( "unexpected alignment style radio box selection" );
                return;
        }

        m_text->SetWindowStyle(flags);
        m_text->Refresh();

        flags = m_text->GetWindowStyle();
        wxLogMessage(wxString::Format("Text alignment: %s",
               (flags & wxTE_RIGHT) ? "Right" :
               (flags & wxTE_CENTER) ? "Center" : "Left"));
    }
    else
#else
    wxUnusedVar(event);
#endif // WXMSW || WXGTK || WXOSX
    {
        CreateText();
    }
}

void TextWidgetsPage::OnStreamRedirector(wxCommandEvent& WXUNUSED(event))
{
#if wxHAS_TEXT_WINDOW_STREAM
    wxStreamToTextRedirector redirect(m_text);
    wxString str( "Outputed to cout, appears in wxTextCtrl!" );
    std::cout << str << std::endl;
#else
    wxMessageBox("This wxWidgets build does not support wxStreamToTextRedirector");
#endif
}
