#ifndef wtfNAMES_SET_H
#define wtfNAMES_SET_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#endif //precompiled headers

WX_DECLARE_STRING_HASH_MAP(int, IdsH);
WX_DECLARE_HASH_MAP(int, wxString, wxIntegerHash, wxIntegerEqual, NamesH);


class NamesSet : public wxObject
{
DECLARE_ABSTRACT_CLASS(NamesSet)
private:
    NamesH names;
    IdsH ids;
    wxString Title;
    wxString path;
    int NextId;
    int Id;
    
public:
    NamesSet();
    ~NamesSet();

// methods for iterating through names
    wxString GetFirstName(int *id);
    wxString GetNextName(int *id);
    wxString GetLastName(int *id);
    wxString GetPrevName(int *id);

// getting properties
    int GetId();
    wxString GetTitle();
    wxString GetDir();
    int GetNextId();
    bool IsEmpty();

// setting properties
    bool SetId(const int id);
    bool SetTitle(const wxString& title = wxT(""));
    bool SetDir(const wxString& dir = wxT(""));

// manipulating names
    wxString GetName(int nid);
    bool AddName(const int nid, const wxString& name);
    bool DeleteName(int nid);
    bool DeleteAllNames();

// checking for name existance
    bool NameExists(int nid);
    int NameExists(const wxString& name);
};

#endif // #ifndef wtfNAMES_SET_H

