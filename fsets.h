#ifndef wtfFILES_SETS_H
#define wtfFILES_SETS_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/xml/xml.h"
#endif //precompiled headers

#include "fset.h"

WX_DECLARE_HASH_MAP( int, FilesSet*, wxIntegerHash, wxIntegerEqual, FilesSetsH );

class FilesSets : public wxObject
{
DECLARE_ABSTRACT_CLASS(FilesSets)
private:
    FilesSetsH fsets;
    int NextId;
    int m_error;
    
    int CheckNamesSet(wxXmlNode *set, NamesSet *nset, const wxString& filename);
    int CheckFilesSet(wxXmlNode *set, FilesSet *fset, const wxString& filename);
public:
    FilesSets();
    FilesSets(const wxString& filename);

    bool Load(const wxString& filename);
    bool Save(const wxString& filename);

    int Error();

    FilesSet* GetFirstFS(int *id);
    FilesSet* GetLastFS(int *id);
    FilesSet* GetNextFS(int *id);
    FilesSet* GetPrevFS(int *id);

    bool AddSet(FilesSet *fset);
    FilesSet *GetSet(int id);
    bool DeleteSet(int id);
    bool DeleteAllSets();

    int GetNextId();

    ~FilesSets();
};

#endif // #ifndef wtfFILES_SETS_H

