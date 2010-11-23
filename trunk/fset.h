#ifndef wtfFILES_SET_H
#define wtfFILES_SET_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/xml/xml.h"
#endif //precompiled headers

#include "nset.h"

WX_DECLARE_HASH_MAP( int, NamesSet*, wxIntegerHash, wxIntegerEqual, NamesSetsH );

class FilesSet : public wxObject
{
DECLARE_ABSTRACT_CLASS(FilesSet)
private:
    NamesSetsH nsets;
    int Id;
    int MainId;
    int NextNSId;
    wxString Title;
    
public:
    FilesSet();
    ~FilesSet();

// methods for iterating names sets
    NamesSet* GetFirstNS(int *id);
    NamesSet* GetNextNS(int *id);
    NamesSet* GetLastNS(int *id);
    NamesSet* GetPrevNS(int *id);

// getting properties
    int GetId();
    wxString GetTitle();
    int GetMainId();
    int GetNextNSId();

// setting properties
    bool SetId(int id);
    bool SetTitle(const wxString& title);
    bool SetMainId(int mid);

// manipulating with names sets
    NamesSet *GetNamesSet(int nsid);
    bool AddNamesSet(NamesSet* nset);
    bool DeleteNamesSet(int nsid);
    bool DeleteAllNamesSets();
    bool AddName(int nsid, int nid, const wxString& name);
    bool DeleteName(int nsid, int nid);

// accessing to main names set
    NamesSet *GetFilesSet();
    wxString GetSourceDir();
    bool SetSourceDir(const wxString& dir);
    bool AddFile(int fid, const wxString& name);
    bool DeleteFile(int fid);
};

#endif // #ifndef wtfFILES_SET_H

