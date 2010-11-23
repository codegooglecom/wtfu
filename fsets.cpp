#include "fsets.h"

IMPLEMENT_ABSTRACT_CLASS(FilesSets, wxObject)

FilesSets::FilesSets()
{
    m_error = 0;
    NextId = 1;
}

FilesSets::FilesSets(const wxString& filename)
{
    FilesSets();
    Load(filename);
}

FilesSets::~FilesSets()
{
    DeleteAllSets();
}

int FilesSets::Error()
{
    return m_error;
}

FilesSet* FilesSets::GetFirstFS(int *id)
{
    int it_id = 1;
    while ( (it_id < NextId) && ( !fsets.count(it_id) ) )
    {
        it_id++;
    }
    if (it_id >= NextId)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return fsets[it_id];
}

FilesSet* FilesSets::GetLastFS(int *id)
{
    int it_id = NextId - 1;
    while ( (it_id > 0) && ( !fsets.count(it_id) ) )
    {
        it_id--;
    }
    if (it_id <= 0)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return fsets[it_id];
}

FilesSet* FilesSets::GetNextFS(int *id)
{
    if (*id <= 0)
    {
        *id = 0;
        return fsets[*id];
    }
    int it_id = *id;
    it_id++;
    while ( (it_id < NextId) && ( !fsets.count(it_id) ) )
    {
        it_id++;
    }
    if (it_id >= NextId)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return fsets[it_id];
}

FilesSet* FilesSets::GetPrevFS(int *id)
{
    if (*id >= NextId)
    {
        *id = 0;
        return fsets[*id];
    }
    int it_id = *id;
    it_id--;
    while ( (it_id > 0) && ( !fsets.count(it_id) ) )
    {
        it_id--;
    }
    if (it_id <= 0)
    {
        *id = 0;
        return NULL;
    }
    *id = it_id;
    return fsets[it_id];
}

int FilesSets::CheckNamesSet(wxXmlNode *set, NamesSet *nset, const wxString& filename)
{
    wxXmlNode *node = set->GetChildren();
    wxXmlNode *node2;
    wxString name;
    int count1 = 0;
    int count2 = 0;
    long lid;
    wxString value;

    while (node != NULL)
    {
        if ( node->GetType() != wxXML_ELEMENT_NODE )
        {
            node = node->GetNext();
            continue;
        }
        name = node->GetName();
        if ( name.Cmp(wxT("title")) == 0 )
        {
            node2 = node->GetChildren();
            while (node2 != NULL)
            {
                if (( node2->GetType() != wxXML_TEXT_NODE ) || (count1 > 0))
                {
                    wxLogError(filename + wxT(": 'title' element's content must be text only."));
                    return 9;
                }
                nset->SetTitle( node2->GetContent() );
                count1++;
                node2 = node2->GetNext();
            }
        } else if ( name.Cmp(wxT("path")) == 0 ) {
            node2 = node->GetChildren();
            while (node2 != NULL)
            {
                if (( node2->GetType() != wxXML_TEXT_NODE ) || (count2 > 0))
                {
                    wxLogError(filename + wxT(": 'path' element's content must be text only."));
                    return 10;
                }
                nset->SetDir( node2->GetContent() );
                count2++;
                node2 = node2->GetNext();
            }
        } else if ( name.Cmp(wxT("item")) == 0 ) {
            if (( !node->GetPropVal(wxT("id"), &value) ) ||
                ( !value.ToLong(&lid) ) ||
                (lid <= 0))
            {
                wxLogError(filename + wxT(": Attribute 'id' having positive numeric value must be specified for each element 'item'."));
                return 11;
            }
            node2 = node->GetChildren();
            while (node2 != NULL)
            {
                if ( node2->GetType() != wxXML_TEXT_NODE )
                {
                    wxLogError(filename + wxT(": 'item' element's content must be text only."));
                    return 14;
                }
                if ( !nset->AddName( (int) lid, node2->GetContent() ) )
                {
                    wxLogError(filename + wxT(": Duplicate name for (%d, %d)."), nset->GetId(), lid);
                    return -1;
                }
                node2 = node2->GetNext();
            }
//        } else {
//            return 15;
        }
        node = node->GetNext();
    }
    return 0;
}

int FilesSets::CheckFilesSet(wxXmlNode *set, FilesSet *fset, const wxString& filename)
{
    wxXmlNode *node = set->GetChildren();
    wxXmlNode *node2;
    int count1 = 0;
    int count2 = 0;
    int error;
    long lid;
    NamesSet *ns;
    wxString value;
    wxString name;

    while (node != NULL)
    {
        if ( node->GetType() != wxXML_ELEMENT_NODE )
        {
            node = node->GetNext();
            continue;
        }
        name = node->GetName();
        if ( name.Cmp(wxT("title")) == 0 )
        {
            node2 = node->GetChildren();
            while (node2 != NULL)
            {
                if (( node2->GetType() != wxXML_TEXT_NODE ) || (count1 > 0))
                {
                    wxLogError(filename + wxT(": 'title' element's content must be text only."));
                    return 4;
                }
                fset->SetTitle( node2->GetContent() );
                count1++;
                node2 = node2->GetNext();
            }
        } else if ( name.Cmp(wxT("files-set")) == 0 ) {
            node2 = node->GetChildren();
            while (node2 != NULL)
            {
                if (( count2 > 0 ) || ( node2->GetType() != wxXML_TEXT_NODE ) ||
                    ( !node2->GetContent().ToLong(&lid) ) || ( lid <= 0 ))
                {
                    wxLogError(filename + wxT(": 'files-set' element's content must be text only representing positive number."));
                    return 5;
                }
                fset->SetMainId((int) lid);
                count2++;
                node2 = node2->GetNext();
            }
        } else if ( name.Cmp(wxT("names-set")) == 0 ) {
            if (( !node->GetPropVal(wxT("id"), &value) ) ||
                ( !value.ToLong(&lid) ) || ( lid <= 0 ))
            {
                wxLogError(filename + wxT(": Attribute 'id' having positive numeric value must be specified for each element 'names-set'."));
                return 6;
            }
            ns = WXDEBUG_NEW NamesSet();
            ns->SetId((int) lid);
            error = CheckNamesSet(node, ns, filename);
            if (error)
            {
                delete(ns);
                return error;
            }
            if ( !fset->AddNamesSet(ns) )
            {
                delete(ns);
                wxLogError(filename + wxT(": Duplicate names set for (%d, %d)."), fset->GetId(), lid);
                return -2;
            }
//        } else {
//            return 7;
        }
        node = node->GetNext();
    }
    return 0;
}

bool FilesSets::Load(const wxString& filename)
{
    DeleteAllSets();

    wxXmlDocument *xmldoc = WXDEBUG_NEW wxXmlDocument(filename);
    wxXmlNode *root;

    if ( !xmldoc->IsOk() )
    {
        m_error = 100;
        delete(xmldoc);
        return false;
    }

// checking data
    root = xmldoc->GetRoot();
    if (( root->GetType() != wxXML_ELEMENT_NODE ) ||
        ( root->GetName().Cmp(wxT("sets")) != 0 ))
    {
        m_error = 1;
        delete(xmldoc);
        wxLogError(filename + wxT(": Invalid document type. The root node must be 'sets'."));
        return false;
    }
    wxXmlNode *set = root->GetChildren();
    wxString value;
    long lid;
    FilesSet *fset;

    while (set != NULL)
    {
        if (( set->GetType() == wxXML_ELEMENT_NODE ) &&
            ( set->GetName().Cmp(wxT("set")) == 0 ) &&
            (( !set->GetPropVal(wxT("id"), &value) ) ||
            ( !value.ToLong(&lid) ) || ( lid <= 0 )))
        {
            m_error = 2;
            delete(xmldoc);
            wxLogError(filename + wxT(": Attribute 'id' having positive numeric value must be specified for each element 'set'."));
            return false;
        }
        if (( set->GetType() != wxXML_ELEMENT_NODE ) ||
            ( set->GetName().Cmp(wxT("set")) != 0 ))
        {
            set = set->GetNext();
            continue;
        }
        fset = WXDEBUG_NEW FilesSet();
        fset->SetId( (int) lid );
        m_error = CheckFilesSet(set, fset, filename);
        if (m_error)
        {
            delete(fset);
            delete(xmldoc);
            return false;
        }
        if ( !AddSet(fset) )
        {
            m_error = -3;
            delete(fset);
            delete(xmldoc);
            wxLogError(filename + wxT(": Duplicate set for id = %d."), lid);
            return false;
        }
        set = set->GetNext();
    }

    delete(xmldoc);
    return true;
}

bool FilesSets::Save(const wxString& filename)
{
    wxXmlDocument *xmldoc = WXDEBUG_NEW wxXmlDocument();
    wxXmlNode *root = WXDEBUG_NEW wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("sets"));
    wxXmlNode *set, *node, *set2;
    wxString value;
    wxString eol = wxT("\012");
    int id1, id2, id3;
    FilesSet *fset;
    NamesSet *nset;

//    root->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));

    fset = GetLastFS(&id1);
    while ( (id1 > 0) && (id1 < GetNextId()) )
    {
        value.Empty();
        value.Printf(wxT("%d"), id1);
        set = WXDEBUG_NEW wxXmlNode(root, wxXML_ELEMENT_NODE, wxT("set"), wxEmptyString, WXDEBUG_NEW wxXmlProperty(wxT("id"), value));
//        set->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
        nset = fset->GetLastNS(&id2);
        while ( (id2 > 0) && (id2 < fset->GetNextNSId()) )
        {
            value.Empty();
            value.Printf(wxT("%d"), id2);
            set2 = WXDEBUG_NEW wxXmlNode(set, wxXML_ELEMENT_NODE, wxT("names-set"), wxEmptyString, WXDEBUG_NEW wxXmlProperty(wxT("id"), value));
//            set2->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
            value = nset->GetLastName(&id3);
            while ( (id3 > 0) && (id3 < nset->GetNextId()) )
            {
                node = WXDEBUG_NEW wxXmlNode(set2, wxXML_ELEMENT_NODE, wxT("item"));
                node->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, value));
                value.Empty();
                value.Printf(wxT("%d"), id3);
                node->SetProperties(WXDEBUG_NEW wxXmlProperty(wxT("id"), value));
//                set2->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
                value = nset->GetPrevName(&id3);
            }
            node = WXDEBUG_NEW wxXmlNode(set2, wxXML_ELEMENT_NODE, wxT("path"));
            node->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, nset->GetDir()));
//            set2->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
            node = WXDEBUG_NEW wxXmlNode(set2, wxXML_ELEMENT_NODE, wxT("title"));
            node->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, nset->GetTitle()));
//            set2->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
            nset = fset->GetPrevNS(&id2);
        }
//        set->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
        node = WXDEBUG_NEW wxXmlNode(set, wxXML_ELEMENT_NODE, wxT("files-set"));
        value.Empty();
        value.Printf(wxT("%d"), fset->GetMainId());
        node->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, value));
//        set->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
        node = WXDEBUG_NEW wxXmlNode(set, wxXML_ELEMENT_NODE, wxT("title"));
        node->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, fset->GetTitle()));
//        set->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
//        root->AddChild(WXDEBUG_NEW wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString, eol));
        fset = GetPrevFS(&id1);
    }
    xmldoc->SetRoot(root);

    bool rv = xmldoc->Save(filename, 1);

    delete(xmldoc);

    return rv;
}

int FilesSets::GetNextId()
{
    return NextId;
}

bool FilesSets::AddSet(FilesSet *fset)
{
    int id = fset->GetId();

    if (id <= 0)
    {
        id = NextId;
        fset->SetId(id);
    }

// check exists
    if ( fsets.count(id) )
    {
        return false;
    }

// adding
    fsets[id] = fset;

// updating NextId
    if (id >= NextId)
    {
        NextId = id + 1;
    }

    return true;
}

FilesSet *FilesSets::GetSet(int id)
{
    if ( !fsets.count(id) )
    {
        return NULL;
    }
    return fsets[id];
}

bool FilesSets::DeleteSet(int id)
{
    if ( !fsets.count(id) )
    {
        return false;
    }
    fsets.erase(id);
    return true;
}

bool FilesSets::DeleteAllSets()
{
    FilesSetsH::iterator it;
    FilesSet *fset;

    for (it = fsets.begin(); it != fsets.end(); ++it)
    {
        fset = it->second;
        delete(fset);
    }
    fsets.clear();
    NextId = 1;
    return true;
}

