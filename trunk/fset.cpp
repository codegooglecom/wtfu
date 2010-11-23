#include "fset.h"

IMPLEMENT_ABSTRACT_CLASS(FilesSet, wxObject)

FilesSet::FilesSet()
{
    Id = 0;
    NextNSId = 1;
    MainId = 0;
}

FilesSet::~FilesSet()
{
    DeleteAllNamesSets();
    Title.Clear();
}

NamesSet* FilesSet::GetFirstNS(int *id)
{
    int it_id = 1;
    while ( (it_id < NextNSId) && ( !nsets.count(it_id) ) )
    {
        it_id++;
    }
    if (it_id >= NextNSId)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return nsets[it_id];
}

NamesSet* FilesSet::GetLastNS(int *id)
{
    int it_id = NextNSId - 1;
    while ( (it_id > 0) && ( !nsets.count(it_id) ) )
    {
        it_id--;
    }
    if (it_id <= 0)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return nsets[it_id];
}

NamesSet* FilesSet::GetNextNS(int *id)
{
    if (*id <= 0)
    {
        *id = 0;
        return nsets[*id];
    }
    int it_id = *id;
    it_id++;
    while ( (it_id < NextNSId) && ( !nsets.count(it_id) ) )
    {
        it_id++;
    }
    if (it_id >= NextNSId)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return nsets[it_id];
}

NamesSet* FilesSet::GetPrevNS(int *id)
{
    if (*id >= NextNSId)
    {
        *id = 0;
        return nsets[*id];
    }
    int it_id = *id;
    it_id--;
    while ( (it_id > 0) && ( !nsets.count(it_id) ) )
    {
        it_id--;
    }
    if (it_id <= 0)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return nsets[it_id];
}

int FilesSet::GetId()
{
    return Id;
}

wxString FilesSet::GetTitle()
{
    return Title;
}

int FilesSet::GetMainId()
{
    return MainId;
}

int FilesSet::GetNextNSId()
{
    return NextNSId;
}

NamesSet *FilesSet::GetNamesSet(int nsid)
{
    if ( !nsets.count(nsid) )
    {
        return NULL;
    }
    return nsets[nsid];
}

bool FilesSet::SetId(int id)
{
    Id = id;
    return true;
}

bool FilesSet::SetTitle(const wxString& title)
{
    Title = title;
    return true;
}

bool FilesSet::SetMainId(int mid)
{
    MainId = mid;
    return true;
}

bool FilesSet::AddNamesSet(NamesSet* nset)
{
    if (nset == NULL)
    {
        return false;
    }
    int id = nset->GetId();
    if (id <= 0)
    {
        id = NextNSId;
    }
    nset->SetId(id);
    if ( nsets.count(id) )
    {
        return false;
    }
    nsets[id] = nset;
    if (id >= NextNSId)
    {
        NextNSId = id + 1;
    }
    return true;
}

bool FilesSet::AddName(int nsid, int nid, const wxString& name)
{
    NamesSet *ns = GetNamesSet(nsid);

    if (ns == NULL)
    {
        return false;
    }
    return ns->AddName(nid, name);
}

bool FilesSet::DeleteName(int nsid, int nid)
{
    NamesSet *ns = GetNamesSet(nsid);

    if (ns == NULL)
    {
        return false;
    }
    return ns->DeleteName(nid);
}

NamesSet *FilesSet::GetFilesSet()
{
    return GetNamesSet( GetMainId() );
}

wxString FilesSet::GetSourceDir()
{
    NamesSet *fs = GetFilesSet();

    if (fs == NULL)
    {
        return wxEmptyString;
    }
    return fs->GetDir();
}

bool FilesSet::SetSourceDir(const wxString& dir)
{
    NamesSet *fs = GetFilesSet();

    if (fs == NULL)
    {
        return false;
    }
    return fs->SetDir(dir);
}

bool FilesSet::AddFile(int fid, const wxString& name)
{
    NamesSet *fs = GetFilesSet();

    if (fs == NULL)
    {
        return false;
    }
    return fs->AddName(fid, name);
}

bool FilesSet::DeleteFile(int fid)
{
    NamesSet *fs = GetFilesSet();

    if (fs == NULL)
    {
        return false;
    }
    return fs->DeleteName(fid);
}

bool FilesSet::DeleteNamesSet(int nsid)
{
    NamesSet *ns = GetNamesSet(nsid);

    if (ns == NULL)
    {
        return false;
    }
    delete(ns);
    return nsets.erase(nsid);
}

bool FilesSet::DeleteAllNamesSets()
{
    NamesSetsH::iterator it;
    NamesSet *ns;

    for (it = nsets.begin(); it != nsets.end(); ++it)
    {
        ns = it->second;
        delete(ns);
    }
    nsets.clear();
    NextNSId = 1;
    return true;
}

