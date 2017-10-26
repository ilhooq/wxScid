//////////////////////////////////////////////////////////////////////
//
//  FILE:       filter.h
//              Filter and CompressedFilter Classes
//
//  Part of:    Scid (Shane's Chess Information Database)
//  Version:    1.4
//
//  Notice:     Copyright (c) 2000  Shane Hudson.  All rights reserved.
//
//  Author:     Shane Hudson (sgh@users.sourceforge.net)
//
//////////////////////////////////////////////////////////////////////


#ifndef SCID_FILTER_H
#define SCID_FILTER_H

#include "common.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
//
// Filter class:
//
//    Holds the results of a database search: one byte per game,
//    indicating whether that game is included in the filter or not.
//    A value of 0 indicates the game is excluded, or 1-255 indicates
//    the game is included, and what position to show when the game
//    is loaded: 1 means the start position, 2 means the position after
//    Whites first move, etc.
class Filter
{
  private:
    gamenumT FilterSize;     // Number of values in filter.
    gamenumT FilterCount;    // Number of nonzero values in filter.
    gamenumT Capacity;       // Number of values allocated for Data[].
    byte *   Data;           // The actual filter data.

    Filter(const Filter&);
    Filter& operator=(const Filter&);
    void Allocate();
    void Free();
    void SetCapacity (gamenumT size);
    friend class CompressedFilter;
    
  public:
    Filter (gamenumT size) :Data(NULL) { Init (size); }
    ~Filter() { Free(); }

    void    Init (gamenumT size);
    uint    Size() const { return FilterSize; }
    uint    Count () const { return FilterCount; }
    bool    isWhole () const { return FilterCount == FilterSize; }
    void    Set (gamenumT index, byte value); // Sets the value at index.
    byte    Get (gamenumT index) const;       // Gets the value at index.
    void    Fill (byte value);                // Sets all values.
    void    Append (byte value);              // Appends one value.
};

//////////////////////////////////////////////////////////////////////
//
// CompressedFilter class:
//    Holds the same data as a filter, in compressed format.
//    Random access to individual values is not possible.
//    A CompressedFilter is created from, or restored to, a regular
//    filter with the methods CompressFrom() and UncompressTo().
class CompressedFilter
{
  private:

    uint    CFilterSize;
    uint    CFilterCount;
    uint    CompressedLength;
    byte *  CompressedData;

  public:
    CompressedFilter (void)     { Init(); }
    ~CompressedFilter (void) {
        if (CompressedData != NULL) delete[] CompressedData;
    }

    inline void Clear();

    uint Size() const { return CFilterSize; }
    uint Count() { return CFilterCount; }

    errorT Verify (Filter * filter);

    void CompressFrom (Filter * filter);
    errorT UncompressTo(Filter * filter) const;

  private:
    CompressedFilter(const CompressedFilter&);
    void operator=(const CompressedFilter&);

    inline void Init();
};


inline void Filter::Init (uint size) {
    Free();
    FilterSize = size;
    FilterCount = size;
    Capacity = size;
    Data = NULL;
}

inline void Filter::Allocate()
{
    Free();
    Capacity = FilterSize > Capacity ? FilterSize : Capacity;
    Data = new byte [Capacity];
    byte * pb = Data;
    for (uint i=0; i < FilterSize; i++) { *pb++ = 1; }
}

inline void Filter::Free()
{
    if(Data != NULL) {
        delete[] Data;
        Data = NULL;
    }
}

inline void Filter::Set (uint index, byte value)
{
    ASSERT (index < FilterSize);
    if (Data == NULL) {
        if (value == 1) return;
        Allocate();
    }
    if (Data[index] != 0) FilterCount--;
    if (value != 0) FilterCount++;
    Data[index] = value;
}

inline byte Filter::Get (uint index) const
{
    ASSERT (index < FilterSize);
    byte res = (Data == NULL) ? 1 : Data[index];
    return res;
}

inline void Filter::Fill (byte value)
{
    if (value == 1) {
        if (Data != NULL) Free();
        FilterCount = FilterSize;
    } else {
        if (Data == NULL) Allocate();
        FilterCount = (value != 0) ? FilterSize : 0;
        std::fill(Data, Data + FilterSize, value);
    }
}

inline void Filter::Append (byte value)
{
    if (value != 0) FilterCount++;
    if (value != 1 && Data == NULL) Allocate();
    if (Data != NULL) {
        if (FilterSize >= Capacity) {
            SetCapacity(FilterSize > 512 ? FilterSize * 2 : 1024);
        }
        Data[FilterSize] = value;
    }
    FilterSize++;
}

inline void Filter::SetCapacity(gamenumT size)
{
    if (size > Capacity) {
        Capacity = size;
        if (Data != NULL) {
            byte * newData = new byte [Capacity];
            for (uint i=0; i < FilterSize; i++) newData[i] = Data[i];
            delete[] Data;
            Data = newData;
        }
    }
}

inline void
CompressedFilter::Init ()
{
    CFilterSize = 0;
    CFilterCount = 0;
    CompressedLength = 0;
    CompressedData = NULL;
}

inline void
CompressedFilter::Clear ()
{
    if (CompressedData != NULL) { delete[] CompressedData; }
    Init();
}


#endif  // #ifndef SCID_FILTER_H

//////////////////////////////////////////////////////////////////////
//  EOF: filter.h
//////////////////////////////////////////////////////////////////////
