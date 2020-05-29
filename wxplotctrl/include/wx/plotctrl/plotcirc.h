/////////////////////////////////////////////////////////////////////////////
// Name:        plotcirc.h
// Purpose:     wxPlotCircularBuffer
// Author:      Scrard Developers
// Created:     30.5.2020
// Copyright:   (c) Scrard Developers
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <wx/plotctrl/plotdata.h>
#include <random>

static std::random_device rd;
static std::mt19937 mt(rd());

template<unsigned int MINY,unsigned int MAXY>
class wxPlotCircularBuffer : public wxPlotData
{
    static constexpr double minY = static_cast<double>(MINY);
    static constexpr double heightY = static_cast<double>(MAXY);

    int m_cfwd;  // offset from m_begin for limited items in window
    double* m_ainit1;   // start of x array
    double* m_alast1;   // end of x array
    double* m_ainit2;   // same as above, but for y
    double* m_alast2;   // ...

public:

    const int m_size; // max active items
    int m_count; // currently contained items
    int m_limit; // limited items number (last n items)

    double* m_index1;   // moving pointer to last active x
    double* m_begin1;   // moving pointer to first active x
    double* m_begin11;  // moving pointer to first active x in limited scope
	
    double* m_index2;   // same as above, but for y
    double* m_begin2;   // ...
    double* m_begin22;  // ...	
	
	wxPlotCircularBuffer(int size) : wxPlotData(), m_size(size), m_limit(size), m_count(0), m_cfwd(0)
	{
		size_t bytesize = static_cast<size_t>( 2 * m_size * sizeof(double) );
		m_index1 = m_ainit1 = (double*)malloc(bytesize);
		m_index2 = m_ainit2 = (double*)malloc(bytesize);
		memset(m_ainit1, 0, bytesize);
		memset(m_ainit2, 0, bytesize);
		m_alast1 = &m_ainit1[2*m_size-1];
		m_alast2 = &m_ainit2[2*m_size-1];
		//size_t some_rnd = 4; // zero to max(size-1)
		static std::uniform_int_distribution<int> dist(0, m_size-1);
		std::advance( m_index1, dist(mt) ); // random index start
		std::advance( m_index2, dist(mt) ); // random index start
		m_begin1 = m_index1;
		m_begin2 = m_index2;
		m_begin1++;
		m_begin2++;
		Create( m_ainit1, m_ainit2, 1, true );
	}

	void push_back( double valuex, double valuey )
	{
		if ( m_count < m_size )
		{
			m_count++;
			if ( m_count > m_limit )
			{
				m_begin11++;
				m_begin22++;
				m_cfwd = m_count - m_limit;
			}
		}
		else
		{
			m_begin1++;
			m_begin11++;
			m_begin2++;
			m_begin22++;
		}

		m_index1++;
		if ( m_index1 > m_alast1 )
		{
			m_begin11 = m_begin1 = (double*)memmove( (void*)m_ainit1, (const void*)&m_ainit1[m_size+1], (m_size-1)*sizeof(double));
			m_index1 = &m_ainit1[m_size-1]; // end of first part
			if ( m_cfwd > 0 )
				std::advance( m_begin11, m_cfwd );
		}

		m_index2++;
		if ( m_index2 > m_alast2 )
		{
			m_begin22 = m_begin2 = (double*)memmove( (void*)m_ainit2, (const void*)&m_ainit2[m_size+1], (m_size-1)*sizeof(double));
			m_index2 = &m_ainit2[m_size-1]; // end of first part
			if ( m_cfwd > 0 )
				std::advance( m_begin22, m_cfwd );
		}

		*m_index1 = valuex;
		*m_index2 = valuey;

		ResetPlotDataParameters();
		//CalcBoundingRect();
	}

	void ResetPlotDataParameters()
	{
		if ( m_limit < m_size )
		{
			SetXYPointsData( m_begin11, m_begin22, m_count - m_cfwd );
			SetBoundingRect( wxRect2DDouble(*m_begin11, minY, *m_index1 - *m_begin11, heightY) );
		}
		else
		{
			SetXYPointsData( m_begin1, m_begin2, m_count );
			SetBoundingRect( wxRect2DDouble(*m_begin1, minY, *m_index1 - *m_begin1, heightY) );
		}
	}

	void SetWindow( int last_n )
	{
		if ( last_n < 1 )
		{
			m_limit = m_size;
			m_begin11 = m_begin1;
			m_begin22 = m_begin2;
			m_cfwd = 0;
			return;
		}

		if ( last_n != m_limit && last_n <= m_size )
		{
			m_limit = last_n;
			m_begin11 = m_begin1;
			m_begin22 = m_begin2;
			m_cfwd = (m_count > m_limit) ? m_count - m_limit : 0;
			if ( m_cfwd > 0  )
			{
				std::advance( m_begin11, m_cfwd );
				std::advance( m_begin22, m_cfwd );
			}
		}

		if ( m_count > 0 )
			ResetPlotDataParameters();
	}

	double& backx() { return *m_index1; }
	double& backy() { return *m_index2; }

	~wxPlotCircularBuffer()
	{
		if (m_ainit1) free(m_ainit1);
		if (m_ainit2) free(m_ainit2);
	}
};

//typedef wxPlotCircularBuffer<0,255> xPlotData255;
