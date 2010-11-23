#include "nset.h"

IMPLEMENT_ABSTRACT_CLASS(NamesSet, wxObject)

NamesSet::NamesSet()
{
    Id = 0;
    NextId = 1;
}

NamesSet::~NamesSet()
{
    DeleteAllNames();
    Title.Clear();
    path.Clear();
}

wxString NamesSet::GetFirstName(int *id)
{
    int it_id = 1;
    while ( (it_id < NextId) && ( !names.count(it_id) ) )
    {
        it_id++;
    }
    if (it_id >= NextId)
    {
        *id = 0;
        return wxEmptyString;
    }
    *id = it_id;
    return names[*id];
}

wxString NamesSet::GetLastName(int *id)
{
    int it_id = NextId - 1;
    while ( (it_id > 0) && ( !names.count(it_id) ) )
    {
        it_id--;
    }
    if (it_id <= 0)
    {
        *id = 0;
        return wxEmptyString;
    }
    *id = it_id;
    return names[*id];
}

wxString NamesSet::GetNextName(int *id)
{
    if ((*id <= 0) || (*id >= NextId))
    {
        *id = 0;
        return names[*id];
    }
    int it_id = *id;
    it_id++;
    while ( (it_id < NextId) && ( !names.count(it_id) ) )
    {
        it_id++;
    }
    if (it_id >= NextId)
    {
        *id = 0;
        return wxEmptyString;
    }
    *id = it_id;
    return names[*id];
}

wxString NamesSet::GetPrevName(int *id)
{
    if ((*id <= 0) || (*id >= NextId))
    {
        *id = 0;
        return names[*id];
    }
    int it_id = *id;
    it_id--;
    while ( (it_id > 0) && ( !names.count(it_id) ) )
    {
        it_id--;
    }
    if (it_id <= 0)
    {
        *id = 0;
        return wxEmptyString;
    }
    *id = it_id;
    return names[*id];
}

int NamesSet::GetId()
{
    return Id;
}

wxString NamesSet::GetTitle()
{
    return Title;
}

wxString NamesSet::GetDir()
{
    return path;
}

bool NamesSet::IsEmpty()
{
    return names.empty();
}

bool NamesSet::SetId(const int id)
{
    Id = id;
    return true;
}

bool NamesSet::SetTitle(const wxString& title)
{
    Title = title;
    return true;
}

bool NamesSet::SetDir(const wxString& dir)
{
    path = dir;
    return true;
}

bool NamesSet::AddName(const int nid, const wxString& name)
{
    int newId = (nid <= 0) ? GetNextId() : nid;

    if (names.count(newId))
    {
        return false;
    }
    if (newId >= NextId)
    {
        NextId = newId + 1;
    }
    names[newId] = name;
    ids[name] = newId;

    return true;
}

bool NamesSet::DeleteName(int nid)
{
    if ( !names.count(nid) )
    {
        return false;
    }
    ids.erase( names[nid] );
    names.erase(nid);
    return true;
}

bool NamesSet::DeleteAllNames()
{
    ids.clear();
    names.clear();
    NextId = 1;
    return true;
}

wxString NamesSet::GetName(int nid)
{
    return ((names.count(nid)) ? names[nid] : wxString());
}

int NamesSet::GetNextId()
{
    return NextId;
}

bool NamesSet::NameExists(int nid)
{
    return names.count(nid);
}

int NamesSet::NameExists(const wxString& name)
{
    return ((ids.count(name)) ? ids[name] : 0);
}

