/////////////////////////////////////////////////////////////////////////////
// Created:     2017/04/06
// Copyright:   (c) 2017 Yacoin Developers
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
#include <wx/bitz.h>
#include <wx/dcbuffer.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
/***********************************************************************

            wxBitzBitmap

***********************************************************************/
wxBitzBitmap::wxBitzBitmap() : wxBitmap(), m_radius(0) {}


void wxBitzBitmap::Create ( wxOrientation pOrientation,
                            int pRadius,
                            int pDistance,
                            const wxBrush* pBackgroundBrush,
                            const wxBrush* pPenBrush,
                            const wxPen* pBitBorderPen,
                            const wxPen* pBitLinePen )
{
    m_vbrush.push_back(std::make_pair( pBackgroundBrush, pPenBrush ));
    m_vpen.push_back(std::make_pair( pBitBorderPen, pBitLinePen ));

    Create(pOrientation, pRadius, pDistance);
}

void wxBitzBitmap::Create ( wxOrientation pOrientation,
                            int pRadius,
                            int pDistance )
{
    // defaults if not provided
    if ( m_vbrush.size() == 0 )
    {
        m_vbrush.push_back(std::make_pair( wxWHITE_BRUSH, wxBLACK_BRUSH ));

        while ( m_vpen.size() < m_vbrush.size() )
            m_vpen.push_back(std::make_pair( wxGREY_PEN, wxBLACK_PEN ));
    }

    m_orientation = pOrientation;
    m_brush_cnt = m_vbrush.size();
    m_radius = pRadius;
    m_distance = pDistance;
    m_offset = pDistance;

    if ( m_orientation == wxVERTICAL )
    {
        m_basic_width = m_offset+256*(m_radius+m_distance);
        m_basic_height = m_offset+8*(m_radius+m_distance)+8+m_distance-m_radius;
        wxBitmap::Create(m_basic_width, m_basic_height*m_brush_cnt);
    }
    else
    {
        m_basic_width = m_offset+8*(m_radius+m_distance)+8+m_distance-m_radius;
        m_basic_height = m_offset+256*(m_radius+m_distance);
        wxBitmap::Create(m_basic_width*m_brush_cnt, m_basic_height);
    }

    wxMemoryDC mdc;
    mdc.SelectObject(*this);

    for (m_brush_idx = 0; m_brush_idx < m_brush_cnt; m_brush_idx++)
    {

        mdc.SetPen(*wxTRANSPARENT_PEN); // bg rect border
        mdc.SetBrush( *(m_vbrush[m_brush_idx].first) ); //*brush_pair.first ); // background rect color

        if ( m_orientation == wxVERTICAL )
            mdc.DrawRectangle (wxPoint(0,m_brush_idx*(m_basic_height)), wxSize(m_basic_width,m_basic_height));
        else
            mdc.DrawRectangle (wxPoint(m_brush_idx*(m_basic_width),0), wxSize(m_basic_width,m_basic_height));

        mdc.SetPen(*wxGREY_PEN); // bit border color or type(wxBLACK_DASHED_PEN)

        if (m_orientation == wxVERTICAL)
        {
            // Circles
            for (int i=0; i<=255; i++)
            {
                    if ( i & (1<<0) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 7*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<1) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 6*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<2) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 5*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<3) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 4*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<4) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 3*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<5) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 2*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<6) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 1*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
                    if ( i & (1<<7) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( m_offset+i*(m_radius+m_distance), 0*(m_radius+m_distance)+(m_brush_idx*m_basic_height)+m_offset, m_radius);
            }

            // Lines
            int lineY = 8*(m_radius+m_distance)+m_offset+(m_brush_idx*m_basic_height);

            for (int i=0; i<=255; i++)
            {
                    if ( i & (1<<0) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+7, m_offset+i*(m_radius+m_distance)+m_radius, lineY+7);
                    if ( i & (1<<1) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+6, m_offset+i*(m_radius+m_distance)+m_radius, lineY+6);
                    if ( i & (1<<2) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+5, m_offset+i*(m_radius+m_distance)+m_radius, lineY+5);
                    if ( i & (1<<3) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+4, m_offset+i*(m_radius+m_distance)+m_radius, lineY+4);
                    if ( i & (1<<4) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+3, m_offset+i*(m_radius+m_distance)+m_radius, lineY+3);
                    if ( i & (1<<5) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+2, m_offset+i*(m_radius+m_distance)+m_radius, lineY+2);
                    if ( i & (1<<6) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+1, m_offset+i*(m_radius+m_distance)+m_radius, lineY+1);
                    if ( i & (1<<7) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(m_offset+i*(m_radius+m_distance)-m_radius, lineY+0, m_offset+i*(m_radius+m_distance)+m_radius, lineY+0);
            }
        }
        else    // wxHORIZONTAL
        {
            // Circles
            for (int i=0; i<=255; i++)
            {
                    if ( i & (1<<0) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 7*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<1) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 6*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<2) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 5*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<3) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 4*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<4) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 3*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<5) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 2*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<6) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 1*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
                    if ( i & (1<<7) ) mdc.SetBrush( *(m_vbrush[m_brush_idx].second) );
                    else mdc.SetBrush( *wxTRANSPARENT_BRUSH );
                    mdc.DrawCircle( 0*(m_radius+m_distance)+(m_brush_idx*m_basic_width)+m_offset, m_offset+i*(m_radius+m_distance), m_radius);
            }

            // Lines
            int lineX = 8*(m_radius+m_distance)+m_offset+(m_brush_idx*m_basic_width);

            for (int i=0; i<=255; i++)
            {
                    if ( i & (1<<0) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+7, m_offset+i*(m_radius+m_distance)-m_radius, lineX+7, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<1) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+6, m_offset+i*(m_radius+m_distance)-m_radius, lineX+6, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<2) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+5, m_offset+i*(m_radius+m_distance)-m_radius, lineX+5, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<3) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+4, m_offset+i*(m_radius+m_distance)-m_radius, lineX+4, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<4) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+3, m_offset+i*(m_radius+m_distance)-m_radius, lineX+3, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<5) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+2, m_offset+i*(m_radius+m_distance)-m_radius, lineX+2, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<6) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+1, m_offset+i*(m_radius+m_distance)-m_radius, lineX+1, m_offset+i*(m_radius+m_distance)+m_radius);
                    if ( i & (1<<7) ) mdc.SetPen( *(m_vpen[m_brush_idx].second) );
                    else mdc.SetPen(*wxTRANSPARENT_PEN);
                    mdc.DrawLine(lineX+0, m_offset+i*(m_radius+m_distance)-m_radius, lineX+0, m_offset+i*(m_radius+m_distance)+m_radius);
            }
        }

    }

    mdc.SelectObject(wxNullBitmap);
}



/***********************************************************************

            wxBitzBitmapWindow

***********************************************************************/

wxBitzBitmapWindow::wxBitzBitmapWindow(wxWindow* parent, wxBitzBitmap* pbitmap)
{
    if ( pbitmap->m_radius == 0 )
    {
        wxMessageBox("Aborting from wxBitzBitmapWindow.\nDid you forget wxBitzBitmap::Create?");
        wxAbort();
    }

    m_bmp = wxStaticCast(pbitmap,wxBitmap);
    m_bdc = new wxBufferedDC(NULL, *m_bmp);

    wxWindow::Create(parent,wxID_ANY, wxDefaultPosition, m_bmp->GetSize(), wxSTATIC_BORDER);
    Bind(wxEVT_PAINT,&wxBitzBitmapWindow::OnPaint, this);
    SetBackgroundStyle( wxBG_STYLE_PAINT  ); // Only use it if you repaint the whole window in your paint handler
}

wxSize wxBitzBitmapWindow::DoGetBestSize() const { return m_bmp->GetSize(); };

void wxBitzBitmapWindow::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Blit(0,0,m_bmp->GetSize().GetWidth(),m_bmp->GetSize().GetHeight(),m_bdc,0,0);
}


/***********************************************************************

            wxBitz

***********************************************************************/

wxBitz::wxBitz(wxWindow* parent, const char* pPattern, wxBitzBitmap* pbitmap, std::vector<unsigned char> pvch)
    : m_pattern(pPattern), m_bbmp(pbitmap), m_height(0), m_width(0), m_has_dash(false), m_vch(pvch)
{

    if ( pbitmap->m_radius == 0 )
    {
        wxMessageBox("Aborting from wxBitz.\nDid you forget wxBitzBitmap::Create?");
        wxAbort();
    }


    m_size_bytes = strlen(m_pattern);

    if (pbitmap->m_orientation == wxVERTICAL)
    {
        // for easier paint calculations
        if (pbitmap->m_radius<3)
        {
            m_width = pbitmap->m_radius*2+4;
            m_diff = pbitmap->m_distance-pbitmap->m_radius-1;
        }
        else
        {
            m_width = pbitmap->m_radius*2+6;
            m_diff = pbitmap->m_distance-pbitmap->m_radius-2;
        }

        // size adjustment
        for (int b=0; b<m_size_bytes; b++)
        {
            if ( m_pattern[b] == 'o' )
            {
                m_height += 8*(pbitmap->m_radius+pbitmap->m_distance);
            }
            else
            {
                m_height += 8;
                m_has_dash = true;
            }
        }

        m_height+=pbitmap->m_radius;

        if (m_has_dash)
            m_height+=pbitmap->m_radius;
    }
    else // wxHORIZONTAL
    {
        // for easier paint calculations
        if (pbitmap->m_radius<3)
        {
            m_height = pbitmap->m_radius*2+4;
            m_diff = pbitmap->m_distance-pbitmap->m_radius-1;
        }
        else
        {
            m_height = pbitmap->m_radius*2+6;
            m_diff = pbitmap->m_distance-pbitmap->m_radius-2;
        }

        // size adjustment
        for (int b=0; b<m_size_bytes; b++)
        {
            if ( m_pattern[b] == 'o' )
            {
                m_width += 8*(pbitmap->m_radius+pbitmap->m_distance);
            }
            else
            {
                m_width += 8;
                m_has_dash = true;
            }
        }

        m_width+=pbitmap->m_radius;

        if (m_has_dash)
            m_width+=pbitmap->m_radius;

        // horizontal width adjustment
        if (pbitmap->m_radius<4)
            m_width+=2;
    }

    //m_bdc = new wxBufferedDC(NULL, *m_bmp);
    m_bdc = new wxBufferedDC(NULL, *wxStaticCast(pbitmap,wxBitmap));
    wxWindow::Create(parent, wxID_ANY, wxDefaultPosition, wxSize(m_width,m_height), wxSTATIC_BORDER);
    SetBackgroundStyle( wxBG_STYLE_PAINT  ); // Only use it if you repaint the whole window in your paint handler




    m_idx = 0;

    if (m_vch.capacity() < m_size_bytes)
    {
        m_vch.reserve(m_size_bytes);
    }

    hvec.resize(pbitmap->m_brush_cnt);
    wvec.resize(pbitmap->m_brush_cnt);

    for (int i = 0; i < pbitmap->m_brush_cnt; i++ )
    {
        hvec[i] = m_bbmp->m_basic_height*i;
        wvec[i] = m_bbmp->m_basic_width*i;
    }

    m_radius_plus_distance = pbitmap->m_radius + pbitmap->m_distance;
    m_basic_height_minus_diff_minus8 = pbitmap->m_basic_height - m_diff - 8;
    m_basic_width_minus_diff_minus8 = pbitmap->m_basic_width - m_diff - 8;

    Bind(wxEVT_PAINT, &wxBitz::OnPaint, this);
}

wxSize wxBitz::DoGetBestSize() const { return wxSize( m_width, m_height ); };

void wxBitz::SetBrushPenCombo(int pBrushIndex) { m_idx = pBrushIndex; };

void wxBitz::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);

    if (m_bbmp->m_orientation == wxVERTICAL)
    {
        // paint begin and end rect
        if (m_has_dash)
        {
            dc.Blit(0/*destX*/,0/*destY*/,m_width, 8, m_bdc,/*sourceX*/0,/*sourceY (brush idx)*/m_bbmp->m_basic_height - 8 + hvec[m_idx]);
            dc.Blit(0/*destX*/,m_height-8/*destY*/,m_width, 8, m_bdc,/*sourceX*/0,/*sourceY (brush idx)*/m_bbmp->m_basic_height - 8 + hvec[m_idx]);
        }

        int m_hwm = 0;
        for (int b=0; b<m_size_bytes; b++)
        {
            int byte_val = m_vch[m_size_bytes-1-b];

            if ( m_pattern[b] == 'o' )
            {
                dc.Blit(0/*destX*/,
                        m_hwm/*destY*/,
                            m_width-1, m_basic_height_minus_diff_minus8, m_bdc,
                        /*sourceX*/(byte_val)*(m_radius_plus_distance)+m_diff,
                        /*sourceY (brush idx)*/1+hvec[m_idx]);
                m_hwm += 8*(m_radius_plus_distance);
            }
            else
            {
                if ( b == 0 || m_pattern[b-1] == 'o' )
                    m_hwm += m_bbmp->m_radius;

                dc.Blit(0/*destX*/,
                        m_hwm/*destY*/,
                            m_width-1, 8, m_bdc,
                        /*sourceX*/(byte_val)*(m_radius_plus_distance)+m_diff,
                        8*(m_radius_plus_distance)+m_bbmp->m_distance+hvec[m_idx]);
                m_hwm += 8;//8*(m_bbmp->m_distance+m_bbmp->m_radius);
            }
        }
    }
    else // wxHORIZONTAL
    {
        // paint begin and end rect
        if (m_has_dash)
        {
            dc.Blit(0/*destX*/,0/*destY*/,8, m_height, m_bdc,/*sourceX (brush idx)*/m_bbmp->m_basic_width - 8 + wvec[m_idx],/*sourceY*/0);
            dc.Blit(m_width-8/*destX*/,0/*destY*/,8, m_height, m_bdc,/*sourceX  (brush idx)*/m_bbmp->m_basic_width - 8 + wvec[m_idx],/*sourceY*/0);
        }

        int m_hwm = 0;
        for (int b=0; b<m_size_bytes; b++)
        {
            int byte_val = m_vch[m_size_bytes-1-b];

            if ( m_pattern[b] == 'o' )
            {
                dc.Blit(m_hwm/*destX*/,
                        0/*destY*/,
                            m_basic_width_minus_diff_minus8, m_height-1, m_bdc,
                        /*sourceX*/1+wvec[m_idx],
                        /*sourceY (brush idx)*/(byte_val)*(m_radius_plus_distance)+m_diff);
                m_hwm += 8*(m_radius_plus_distance);
            }
            else
            {
                if ( b == 0 || m_pattern[b-1] == 'o' )
                    m_hwm += m_bbmp->m_radius;

                dc.Blit(m_hwm/*destX*/,
                        0/*destY*/,
                            8, m_height-1, m_bdc,
                        /*sourceX*/8*(m_radius_plus_distance)+m_bbmp->m_distance+wvec[m_idx],
                        /*sourceY*/(byte_val)*(m_radius_plus_distance)+m_diff);
                m_hwm += 8;//8*(m_bbmp->m_distance+m_bbmp->m_radius);
            }
        }
    }
};
