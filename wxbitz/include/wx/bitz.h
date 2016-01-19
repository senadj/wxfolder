/////////////////////////////////////////////////////////////////////////////
// Created:     2017/04/06
// Copyright:   (c) 2017 Yacoin Developers
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <wx/dlimpexp.h>
#include <wx/bitmap.h>
#include <wx/window.h>

#ifdef WXMAKINGDLL_BITZ
    #define WXDLLIMPEXP_BITZ                  WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_BITZ                  WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_BITZ
#endif

class wxBufferedDC;

/***********************************************************************

            class wxBitzBitmap

***********************************************************************/

class WXDLLIMPEXP_BITZ wxBitzBitmap : public wxBitmap
{
    friend class wxBitzBitmapWindow;
    friend class wxBitz;
public:

	wxBitzBitmap();

    void Create (   wxOrientation pOrientation,
                    int pRadius,
                    int pDistance,
                    const wxBrush* pBackgroundBrush, // = wxWHITE_BRUSH, //const_cast<wxBrush*>(CIRCLE_BG_BRUSH),
                    const wxBrush* pPenBrush, // = wxBLACK_BRUSH, //const_cast<wxBrush*>(CIRCLE_FG_BRUSH),
                    const wxPen* pBitBorderPen, // = wxGREY_PEN,
                    const wxPen* pBitLinePen ); //  = wxBLACK_PEN );

    void Create (   wxOrientation pOrientation,
                    int pRadius,
                    int pDistance );

    wxVector<std::pair<const wxBrush*, const wxBrush*>> m_vbrush;
    wxVector<std::pair<const wxPen*, const wxPen*>> m_vpen;

protected:

    wxOrientation m_orientation;
    int m_offset;
    int m_radius;
    int m_distance;
    int m_basic_width;
    int m_basic_height;
    int m_brush_cnt;        // number of brushes (different color compositions) in bmp. Value must be calculated in constructor!
    int m_brush_idx;        // bmp brush index, points to current color combination
};



/***********************************************************************

            class wxBitzBitmapWindow

***********************************************************************/

class WXDLLIMPEXP_BITZ wxBitzBitmapWindow : public wxWindow
{
public:

    wxBitzBitmapWindow(wxWindow* parent, wxBitzBitmap* pbitmap);
    wxSize DoGetBestSize() const;
    void OnPaint(wxPaintEvent& event);

protected:

    wxBitmap* m_bmp;        // source bmp
    wxBufferedDC* m_bdc;    // backup buff DC
};


/***********************************************************************

            class wxBitz

***********************************************************************/

class WXDLLIMPEXP_BITZ wxBitz : public wxWindow
{
public:

    std::vector<unsigned char> m_vch;   // generate bits from this data

    wxBitz(wxWindow* parent, const char* pPattern, wxBitzBitmap* pbitmap, std::vector<unsigned char> pvch = std::vector<unsigned char>());
    wxSize DoGetBestSize() const;
    void OnPaint(wxPaintEvent& event);
    void Repaint();
    void SetBrushPenCombo(int pBrushIndex); // switch between multiple brush/pen combinations

protected:

    wxBitzBitmap* m_bbmp;
    wxBufferedDC* m_bdc;    // backup buff DC
    int m_width;            // width
    int m_height;           // height
    int m_diff;
    bool m_has_dash;    // draw lines too, not just circles
    int m_size_bytes;   // derived from pattern length
    const char* m_pattern; // oooo----

    int m_idx;  // index to current brush/pen set

    // helper members to speed up OnPaint calculations
    int m_radius_plus_distance;
    int m_basic_height_minus_diff_minus8;
    int m_basic_width_minus_diff_minus8;
    wxVector<int> hvec;
    wxVector<int> wvec;
};
