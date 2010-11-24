#include "util.h"

#include "debug.h"

#ifdef wtfDEBUG_UTIL_H
#include "wx/log.h"
#endif // wtfDEBUG_UTIL_H

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wtfArrayFNC);
WX_DEFINE_OBJARRAY(wtfRenamerProcess);

IMPLEMENT_ABSTRACT_CLASS(wtfFileNameComponents, wxObject)

wtfFileNameComponents::wtfFileNameComponents(const wxString& filename, const wxString& ignore)
{
    size_t i = 0;
    size_t len;

    m_len = 0;
    m_id = (size_t) wxNOT_FOUND;
    while (i < filename.Len())
    {
        if (ignore.Find(filename.Mid(i, 1)) != wxNOT_FOUND)
        {
            i++;
            continue;
        }
        m_offsets[m_len] = m_filename.Len();
        len = 0;
        if ((filename.Mid(i, 1) >= wxT("0")) && (filename.Mid(i, 1) <= wxT("9")))
        {
            while ((i < filename.Len()) && (filename.Mid(i, 1) == wxT("0")))
            {
                i++;
            }
            if (i >= filename.Len())
            {
                i--;
            }
            while ((i + len < filename.Len()) && (filename.Mid(i + len, 1) >= wxT("0")) && (filename.Mid(i + len, 1) <= wxT("9")))
            {
                m_filename += filename.Mid(i + len, 1);
                len++;
            }
            m_len += 1;
        } else {
            while ((i + len < filename.Len()) && ((filename.Mid(i + len, 1) < wxT("0")) || (filename.Mid(i + len, 1) > wxT("9"))))
            {
                m_filename += filename.Mid(i + len, 1);
                len++;
                m_offsets[m_len + len] = m_filename.Len();
            }
            m_len += len;
        }
        i += len;
    }
    m_offsets[m_len] = m_filename.Len();
}

wtfFileNameComponents::~wtfFileNameComponents()
{
    m_filename.Clear();
    m_len = 0;
}

size_t wtfFileNameComponents::Len() const
{
    return m_len;
}

wxString wtfFileNameComponents::GetPart(const size_t from, const size_t len)
{
    wxString res;

    if (len == 0 || from >= m_len)
    {
        return res;
    }

    size_t to = (from + len > m_len) ? m_len : from + len;

    res = m_filename.Mid(m_offsets[from], m_offsets[to] - m_offsets[from]);
    return res;
}

size_t wtfFileNameComponents::FindPart(const wxString& part) const
{
    return m_filename.Find(part);
}

void wtfFileNameComponents::SetId(size_t id)
{
    m_id = id;
}

size_t wtfFileNameComponents::GetId() const
{
    return m_id;
}


bool Disambiguate(wxArrayString& files, wxArrayString& names, CorrM *corr, const wxString& ignore)
{
//    wxLogDebug(_("Йа Креведко"));
    if ((files.GetCount() == 0) || (names.GetCount() == 0) || (corr == NULL))
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Files list and/or names list is empty"));
#endif
        return false;
    }
    if ((files.GetCount() == 1) && (names.GetCount() == 1))
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Files list (and names list) contains only one element"));
#endif
        corr->clear();
        (*corr)[0] = 0;
        return true;
    }
/*
    if ((files.GetCount() == 1) || (names.GetCount() == 1))
    {
        return false;
    }
*/

    wxArrayString a_files, a_names;
    wxFileName fname;
    wxString str;
    wtfArrayNames sfiles;
    size_t i, j, cnt, fcnt, ncnt;

#ifdef wtfDEBUG_DISAMBIGUATE
    wxLogMessage(wxT("Disambiguate: Normalizing files list..."));
#endif
    cnt = files.GetCount();
    fcnt = 0;
    for (i = 0; i < cnt; i++)
    {
        j = 0;
        str = wxT("");
        while (j < files[i].Len())
        {
            if (ignore.Find(files[i][j]) == wxNOT_FOUND)
            {
                str += files[i][j];
            }
            j++;
        }
        if (str.Len() > 0)
        {
            fname.Assign(str);
            fname.Normalize(wxPATH_NORM_TILDE);
            str = fname.GetFullPath();
        }
        a_files.Add(str);
        if (str.Len() > 0)
        {
            if (sfiles.count(str))
            {
                return false;
            }
            sfiles[str] = i;
            fcnt++;
        }
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate:   ") + files[i] + wxT(" => ") + str);
#endif
    }
    sfiles.clear();
    if (fcnt == 0)
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Files list contains no non-empty elements"));
#endif
        return false;
    }
#ifdef wtfDEBUG_DISAMBIGUATE
    wxLogMessage(wxT("Disambiguate: Normalizing names list..."));
#endif
    cnt = names.GetCount();
    ncnt = 0;
    for (i = 0; i < cnt; i++)
    {
        j = 0;
        str = wxT("");
        while (j < names[i].Len())
        {
            if (ignore.Find(names[i][j]) == wxNOT_FOUND)
            {
                str += names[i][j];
            }
            j++;
        }
        if (str.Len() > 0)
        {
            fname.Assign(str);
            fname.Normalize(wxPATH_NORM_TILDE);
            str = fname.GetFullPath();
        }
        a_names.Add(str);
        if (str.Len() > 0)
        {
            if (sfiles.count(str))
            {
                return false;
            }
            sfiles[str] = i;
            ncnt++;
        }
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate:   ") + names[i] + wxT(" => ") + str);
#endif
    }
    sfiles.clear();
    if (ncnt == 0)
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Names list contains no non-empty elements"));
#endif
        return false;
    }
    if (fcnt == 1 && ncnt == 1)
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Files list (and names list) contains only one non-empty elements"));
#endif
        i = 0;
        while (a_files[i].Len() == 0)
        {
            i++;
        }
        j = 0;
        while (a_names[j].Len() == 0)
        {
            j++;
        }
        corr->clear();
        (*corr)[i] = j;
        return true;
    }

    wtfArrayFNC afnc_files, afnc_names;
    wtfFileNameComponents *fnc;
    CorrM::iterator cit;
    size_t flen, nlen;

#ifdef wtfDEBUG_DISAMBIGUATE
    wxLogMessage(wxT("Disambiguate: Start disambiguation by name..."));
#endif
    cnt = a_files.GetCount();
    for (i = 0; i < cnt; i++)
    {
        if (a_files[i].Len() == 0)
        {
            continue;
        }
        fname.Assign(a_files[i]);
        str = fname.GetFullName();
        if (sfiles.count(str))
        {
            sfiles[str] = (size_t) wxNOT_FOUND;
        } else {
            sfiles[str] = i;
        }
    }

    corr->clear();
    cnt = a_names.GetCount();
    nlen = 0;
    for (i = 0; i < cnt; i++)
    {
        if (a_names[i].Len() == 0)
        {
            continue;
        }
        fname.Assign(a_names[i]);
        str = fname.GetFullName();
        if ((sfiles.count(str)) && (sfiles[str] != (size_t) wxNOT_FOUND))
        {
            if (corr->count(sfiles[str]))
            {
                fnc = WXDEBUG_NEW wtfFileNameComponents(a_names[i], wxT(""));
                fnc->SetId(i);
                afnc_names.Add(fnc);
                (*corr)[ sfiles[str] ] = (size_t) wxNOT_FOUND;
                if (fnc->Len() > nlen)
                {
                    nlen = fnc->Len();
                }
            } else {
                (*corr)[ sfiles[str] ] = i;
            }
        } else {
            fnc = WXDEBUG_NEW wtfFileNameComponents(a_names[i], ignore);
            fnc->SetId(i);
            afnc_names.Add(fnc);
            if (fnc->Len() > nlen)
            {
                nlen = fnc->Len();
            }
#ifdef wtfDEBUG_DISAMBIGUATE
            wxLogMessage(wxT("Disambiguate: There is no file with name '") + str + wxT("' in files list"));
#endif
        }
    }
    sfiles.clear();

    cnt = a_files.GetCount();
    flen = 0;
    for (i = 0; i < cnt; i++)
    {
        if (a_files[i].Len() == 0)
        {
            continue;
        }
        if ((!corr->count(i)) || ((*corr)[i] == (size_t) wxNOT_FOUND))
        {
            fnc = WXDEBUG_NEW wtfFileNameComponents(a_files[i], wxT(""));
            fnc->SetId(i);
            afnc_files.Add(fnc);
            if (corr->count(i))
            {
                corr->erase(i);
            }
            if (fnc->Len() > flen)
            {
                flen = fnc->Len();
            }
#ifdef wtfDEBUG_DISAMBIGUATE
        } else {
            wxLogMessage(wxT("Disambiguate: Found: '") + a_files[i] + wxT("' => '") + a_names[ (*corr)[i] ] + wxT("'"));
#endif
        }
    }

    if ((afnc_files.GetCount() == 0) || (afnc_names.GetCount() == 0))
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: There is no more files or names"));
#endif
        return true;
    }
    if ((afnc_files.GetCount() == 1) && (afnc_names.GetCount() == 1))
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: There is only one file and only one name"));
#endif
        (*corr)[ afnc_files[0].GetId() ] = afnc_names[0].GetId();
        return true;
    }

    wtfArrayNames snames;
    wtfArrayNames::iterator it;
    size_t s1, l1, s2, l2;
    bool found = false;
    size_t flag;

    fcnt = afnc_files.GetCount();
    ncnt = afnc_names.GetCount();
#ifdef wtfDEBUG_DISAMBIGUATE
    wxLogMessage(wxT("Disambiguate: Start main disambiguation process\nFiles: %d (len = %d); Names: %d (len = %d)"), fcnt, flen, ncnt, nlen);
#endif
    s1 = 0;
    while (!found && s1 < flen)
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Searching common part for files..."));
#endif
        l1 = 0;
        str = afnc_files[0].GetPart(s1, l1 + 1);
        sfiles[str] = 1;
        flag = 1;
        while (flag && s1 + l1 < flen)
        {
            i = 1;
            while (i < cnt && flag)
            {
                str = afnc_files[i].GetPart(s1, l1 + 1);
                if (!sfiles.count(str))
                {
                    flag = 0;
                }
                i++;
            }
            if (flag)
            {
                l1++;
#ifdef wtfDEBUG_DISAMBIGUATE
                wxLogMessage(wxT("Disambiguate: All files have the same part (%d, %d) in names"), s1, l1);
#endif
                str = afnc_files[0].GetPart(s1, l1 + 1);
                sfiles[str] = 1;
            }
        }
        s1 += l1;
        sfiles.clear();
        if (s1 >= flen)
        {
            break;
        }
        l1 = (flen - s1 > nlen) ? nlen : (flen - s1);
        flag = 0;
        while (!found && l1 > 0)
        {
            for (i = 0; i < fcnt; i++)
            {
                str = afnc_files[i].GetPart(s1, l1);
                if (str.Len() == 0)
                {
                    continue;
                }
                if (sfiles.count(str))
                {
#ifdef wtfDEBUG_DISAMBIGUATE
                    wxLogMessage(wxT("Disambiguate: Part (%d, %d) is not unique for files (two or more '") + str + wxT("')"), s1, l1);
#endif
                    flag = 1;
                    break;
                }
                sfiles[str] = i;
            }
            if (flag)
            {
                l1--;
                sfiles.clear();
                break;
            }
#ifdef wtfDEBUG_DISAMBIGUATE
            wxLogMessage(wxT("Disambiguate: Part (%d, %d) is unique for files"), s1, l1);
#endif
            l2 = l1;
            s2 = 0;
            while (!found && (s2 + l2 <= nlen))
            {
#ifdef wtfDEBUG_DISAMBIGUATE
                wxLogMessage(wxT("Disambiguate: Trying part (%d, %d) for names list..."), s2, l2);
#endif
                for (i = 0; i < ncnt; i++)
                {
                    str = afnc_names[i].GetPart(s2, l2);
                    if (sfiles.count(str))
                    {
                        found = true;
                        if (snames.count(str))
                        {
                            found = false;
                            break;
                        }
                        snames[str] = i;
                    }
                }
                if (found)
                {
                    break;
                }
                snames.clear();
                s2++;
            }
            if (found)
            {
                break;
            }
            sfiles.clear();
            l1--;
        }
        if (found)
        {
            break;
        }
        s1++;
    }
    if (found)
    {
#ifdef wtfDEBUG_DISAMBIGUATE
        wxLogMessage(wxT("Disambiguate: Disambiguated"));
        wxLogMessage(wxT("Disambiguate: The unique part for names list is (%d, %d)"), s2, l2);
#endif
        for (it = snames.begin(); it != snames.end(); ++it)
        {
            i = sfiles[ it->first ];
            i = afnc_files[i].GetId();
            j = afnc_names[ it->second ].GetId();
#ifdef wtfDEBUG_DISAMBIGUATE
            wxLogMessage(wxT("Disambiguate:   '") + a_files[i] + wxT("' => '") + a_names[j] + wxT("'"));
#endif
            (*corr)[i] = j;
        }
#ifdef wtfDEBUG_DISAMBIGUATE
    } else {
        wxLogMessage(wxT("Disambiguate: Cannot disambiguate"));
#endif
    }
    return found;
}

void wtfMakeFilesList(const wxString& text, wxArrayString *list)
{
    size_t i = 0;
    size_t j;
    wxString filename;

    while (i < text.Len())
    {
        while ((text.Mid(i, 1) == wxT(" ")) || (text.Mid(i, 1) == wxT(",")))
        {
            i++;
        }
        if (text.Mid(i, 1) == wxT("\""))
        {
            i++;
            if (i >= text.Len())
            {
                break;
            }
            j = text.Mid(i).Find(wxT("\""));
        } else {
            j = text.Mid(i).Find(wxT(","));
        }
        if (j == (size_t) wxNOT_FOUND)
        {
            filename = text.Mid(i);
            i = text.Len();
        } else {
            filename = text.Mid(i, j);
            i += j + 1;
        }
        filename.Trim(false);
        filename.Trim(true);
        if (filename.Len() > 0)
        {
            list->Add(filename);
        }
    }
    filename.Clear();
}


IMPLEMENT_ABSTRACT_CLASS(wtfRenamerAction, wxObject)

wtfRenamerAction::wtfRenamerAction(const wtfRenamerActionType type, const wtfRenamerActionData& data)
	: wxObject(),
	m_type(type),
	m_error(0)
{
	m_data.path = data.path;
	m_data.newpath = data.newpath;
}

wtfRenamerAction::~wtfRenamerAction()
{
	if (m_data.path != NULL)
	{
		delete(m_data.path);
	}
	if (m_data.newpath != NULL)
	{
		delete(m_data.newpath);
	}
}

int wtfRenamerAction::PerformAction()
{
	wxASSERT_MSG(m_data.path != NULL, wxT("No arguments"));
	wxArrayString dirs;
	wxFileName fn;
	int rv;
	size_t i, cnt;
	switch (m_type)
	{
		case wtfRenamerActionCreateDir:
			wxLogDebug(wxT("create dir '%s'"), m_data.path->c_str());
			fn.AssignDir( *(m_data.path) );
			if (!fn.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_LONG | wxPATH_NORM_TILDE))
			{
				return W_RENAMER_ERROR_NOT_PERMITTED;
			}
			if (fn.GetDirCount() == 0)
			{
				return W_RENAMER_ERROR_SYS_ERROR;
			}
			if (fn.DirExists())
			{
				return W_RENAMER_ERROR_FILE_EXISTS;
			}
			dirs = fn.GetDirs();
			cnt = dirs.GetCount();
			for (i = 0; i < cnt; i++)
			{
				fn.RemoveLastDir();
			}
			i = 0;
			while ((i < cnt - 1) && fn.IsDirReadable())
			{
				fn.AppendDir( dirs.Item(i) );
				i++;
			}
			if (!fn.IsDirReadable())
			{
				if (!fn.DirExists())
				{
					return (wxFileExists( fn.GetFullPath() )) ? W_RENAMER_ERROR_NOT_DIR : W_RENAMER_ERROR_DIR_NOT_EXISTS;
				}
				return W_RENAMER_ERROR_NOT_PERMITTED;
			}
			if (!fn.IsDirWritable())
			{
				return W_RENAMER_ERROR_NOT_PERMITTED;
			}
			fn.AppendDir( dirs.Item(cnt - 1) );
			if (!wxMkdir( fn.GetFullPath() ))
			{
				return W_RENAMER_ERROR_SYS_ERROR;
			}
			return 0;
			break;
		case wtfRenamerActionRemoveDir:
			wxLogDebug(wxT("remove dir '%s'"), m_data.path->c_str());
			rv = wtfRenamer::doGetFileName( *(m_data.path) + wxFileName::GetPathSeparator(), &fn );
			if (rv != 0)
			{
				return rv;
			}
			if (fn.GetDirCount() == 0)
			{
				return W_RENAMER_ERROR_SYS_ERROR;
			}
			fn.RemoveLastDir();
			if (!fn.IsDirWritable())
			{
				return W_RENAMER_ERROR_NOT_PERMITTED;
			}
			if (!wxRmdir( *(m_data.path) ))
			{
				return W_RENAMER_ERROR_SYS_ERROR;
			}
			return 0;
			break;
		case wtfRenamerActionCopyFile:
			wxASSERT_MSG(m_data.newpath != NULL, wxT("Too less arguments"));
			wxLogDebug(wxT("copy file '%s' -> '%s'"), m_data.path->c_str(), m_data.newpath->c_str());
			return W_RENAMER_ERROR_NOT_SUPPORTED;
			break;
		case wtfRenamerActionRenameFile:
			wxASSERT_MSG(m_data.newpath != NULL, wxT("Too less arguments"));
			return wtfRenamer::doRename(*(m_data.path), *(m_data.newpath));
			break;
		case wtfRenamerActionCreateHardLink:
			wxASSERT_MSG(m_data.newpath != NULL, wxT("Too less arguments"));
			return wtfRenamer::doCreateHardlink(*(m_data.path), *(m_data.newpath));
			break;
		case wtfRenamerActionCreateSymlink:
			wxASSERT_MSG(m_data.newpath != NULL, wxT("Too less arguments"));
			return wtfRenamer::doCreateSymlink(*(m_data.path), *(m_data.newpath));
			break;
		case wtfRenamerActionUnlinkFile:
			wxLogDebug(wxT("remove file '%s'"), m_data.path->c_str());
			rv = wtfRenamer::doGetFileName( *(m_data.path), &fn );
			if (rv != 0)
			{
				return rv;
			}
			if (!fn.IsDirWritable())
			{
				return W_RENAMER_ERROR_NOT_PERMITTED;
			}
			if (!wxRemoveFile( fn.GetFullPath() ))
			{
				return W_RENAMER_ERROR_SYS_ERROR;
			}
			return 0;
			break;
		default:
			wxLogDebug(wxT("unknown action: %d"), m_type);
			return W_RENAMER_ERROR_NOT_SUPPORTED;
			break;
	}
	return W_RENAMER_ERROR_NOT_SUPPORTED;
}


#ifdef __WXMSW__
bool wtfRenamer::m_hasHLS = false;
wxDynamicLibrary *wtfRenamer::m_lib = NULL;
bool wtfRenamer::m_HLShasW = false;
bool wtfRenamer::m_HLShasA = false;
#endif

IMPLEMENT_ABSTRACT_CLASS(wsdgRenamerOptions, wxObject)

wsdgRenamerOptions::wsdgRenamerOptions()
	: wxObject(),
	m_flags(0)
{
}

wsdgRenamerOptions::wsdgRenamerOptions(long flags)
	: wxObject(),
	m_flags(flags)
{
}

wtfRenamerAnswer wsdgRenamerOptions::OnError(wtfRenamerAction *WXUNUSED(action), int WXUNUSED(error), const wxString& WXUNUSED(msg))
{
	return wtfRenamerAnswerStop;
}


IMPLEMENT_ABSTRACT_CLASS(wtfRenamer, wxObject)

wtfRenamer::wtfRenamer(const wxArrayString& files, const wxArrayString& names)
	: wxObject(),
	m_files(),
	m_names(),
	m_opts(NULL),
	m_process(),
	m_error(0),
	m_undo()
{
	wxString str;
	size_t i, cnt;

	cnt = files.GetCount();
	for (i = 0; i < cnt; i++)
	{
		str = files.Item(i);
		m_files.Add(str);
	}

	cnt = names.GetCount();
	for (i = 0; i < cnt; i++)
	{
		str = names.Item(i);
		m_names.Add(str);
	}

	m_opts = new wsdgRenamerOptions();
}

wtfRenamer::~wtfRenamer()
{
	m_files.Clear();
	m_names.Clear();
	m_process.Clear();
	m_undo.Clear();
}

int wtfRenamer::doGetFileName(const wxString& name, wxFileName* filename)
{
	wxArrayString dirs;
	size_t i, cnt;

	// name is considered here as abstract path, i.e. string
	// that interpreted by the current OS as valid path
	// note that the path does not necessarily exist
	filename->Assign(name);

	// make the filename to be absolute (current working directory is used)
	if (!filename->Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_LONG | wxPATH_NORM_TILDE))
	{
		return W_RENAMER_ERROR_NOT_PERMITTED;
	}

	// now lets check existence and permissions
	dirs = filename->GetDirs();
	cnt = dirs.GetCount();

	for (i = 0; i < cnt; i++)
	{
		filename->RemoveLastDir();
	}

	i = 0;
	while (i < cnt && filename->IsDirReadable())
	{
		filename->AppendDir(dirs.Item(i));
		i++;
	}

	if (filename->IsDirReadable())
	{
		return 0;
	}

	if (!wxDirExists(filename->GetPath()))
	{
		return (wxFileExists(filename->GetPath())) ? W_RENAMER_ERROR_NOT_DIR : W_RENAMER_ERROR_DIR_NOT_EXISTS;
	}

	return W_RENAMER_ERROR_NOT_PERMITTED;
}

bool wtfRenamer::HasHardlinkSupport()
{
#ifdef __WXMSW__
	int major, minor;
	wxOperatingSystemId sys_id = wxGetOsVersion(&major, &minor);
	
	
	if (sys_id != wxOS_WINDOWS_NT)
	{
		return false;
	}

	if (wtfRenamer::m_hasHLS)
	{
#	ifdef _UNICODE
		return wtfRenamer::m_HLShasW || wtfRenamer::m_HLShasA;
#	else
		return wtfRenamer::m_HLShasA;
#	endif
	}
	wtfRenamer::m_hasHLS = true;
	wtfRenamer::m_lib = WXDEBUG_NEW wxDynamicLibrary(_T("kernel32"));

	if (!wtfRenamer::m_lib->IsLoaded())
	{
		delete(wtfRenamer::m_lib);
		wtfRenamer::m_lib = NULL;
		return false;
	}
	wtfRenamer::m_HLShasW = wtfRenamer::m_lib->HasSymbol(_T("CreateHardLinkW"));
	wtfRenamer::m_HLShasA = wtfRenamer::m_lib->HasSymbol(_T("CreateHardLinkA"));

#	ifdef __WXDEBUG__
	if (wtfRenamer::m_HLShasW)
	{
		wxLogDebug(wxT("Found CreateHardLinkW"));
	}
	if (wtfRenamer::m_HLShasA)
	{
		wxLogDebug(wxT("Found CreateHardLinkA"));
	}
#	endif

	if
#	ifdef _UNICODE
		(!wtfRenamer::m_HLShasW && !wtfRenamer::m_HLShasA)
#	else
		(!wtfRenamer::m_HLShasA)
#	endif
	{
		delete(wtfRenamer::m_lib);
		wtfRenamer::m_lib = NULL;
		return false;
	}
	return true;
#else
#	ifdef __UNIX__
	return false;
#	else
	return false;
#	endif
#endif
}

bool wtfRenamer::CanCreateHardlink(const wxString& oldname, const wxString& newname)
{
	if (!wtfRenamer::HasHardlinkSupport())
	{
		wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): no hard link support"));
		return false;
	}

	wxFileName foldname, fnewname;
	int rv = wtfRenamer::doGetFileName(oldname, &foldname);
	wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): oldname: '%s'"), foldname.GetFullPath().c_str());
	if (rv != 0)
	{
		wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): cannot find oldname"));
		return false;
	}

	rv = wtfRenamer::doGetFileName(newname, &fnewname);
	wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): newname: '%s'"), fnewname.GetFullPath().c_str());
	if (rv == W_RENAMER_ERROR_DIR_NOT_EXISTS && fnewname.GetDirCount())
	{
		fnewname.RemoveLastDir();
	} else if (rv != 0) {
		wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): cannot resolve newname (%d)"), rv);
		return false;
	}

	if (!fnewname.IsDirWritable())
	{
		wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): no write permissions on target dir"));
		return false;
	}

	wxString path;
	int num = 0;
	if (fnewname.IsDir())
	{
		fnewname.Assign(fnewname.GetFullPath(), wxT("tnn00"));
		num++;
	}
	while (fnewname.FileExists())
	{
		path = wxString::Format(wxT("tnn%02x"), num);
		fnewname.SetName(path);
		num++;
	}

	bool rm_on = false;
	wxFile file;
	wxDir dir;
	if (!foldname.FileExists())
	{
		if (foldname.IsDir())
		{
			dir.Open(foldname.GetFullPath());
		} else {
			dir.Open(foldname.GetPath());
		}
		if (dir.IsOpened() && dir.HasFiles() && dir.GetFirst(&path, wxT(""), wxDIR_FILES | wxDIR_HIDDEN))
		{
			if (foldname.IsDir())
			{
				foldname.Assign(foldname.GetFullPath(), path);
			} else {
				foldname.Assign(foldname.GetPath(), path);
			}
		} else if (foldname.IsDir()) {
			return false;
		} else {
			rm_on = true;
			if (!foldname.IsDirWritable() || !file.Create( foldname.GetFullPath() ))
			{
				wxLogDebug(wxT("wtfRenamer::CanCreateHardlink(): cannot create temp file '%s'"), foldname.GetFullPath().c_str());
				return false;
			}
			if (file.IsOpened())
			{
				file.Close();
			}
		}
	}
	path = wxT("wtfRenamer::CanCreateHardLink(): oldpath: '");
	path += foldname.GetFullPath();
	path += wxT("', newpath: '");
	path += fnewname.GetFullPath();
	path += wxT("'");
	path.Replace(wxT("%"), wxT("%%"));
	wxLogDebug(path);
	rv = doCreateHardlink(foldname.GetFullPath(), fnewname.GetFullPath());
	if (rm_on)
	{
		wxLogDebug(wxT("wtfRenamer::CanCreateHardLink(): remove old file"));
		wxRemoveFile( foldname.GetFullPath() );
	}
	wxLogDebug(wxT("wtfRenamer::CanCreateHardLink(): wtfRenamer::deCreateHardLink returned %d; newname %sexists"), rv, (fnewname.FileExists()) ? wxT("") : wxT("not "));
	if (rv != 0 || !fnewname.FileExists())
	{
		return false;
	}
	wxRemoveFile( fnewname.GetFullPath() );
	return true;
}

int wtfRenamer::doRename(const wxString& oldname, const wxString& newname)
{
	wxLogDebug(wxT("rename file '%s' -> '%s'"), oldname.c_str(), newname.c_str());
	wxFileName foldname, fnewname;
	int rv = wtfRenamer::doGetFileName(oldname, &foldname);
	if (rv != 0)
	{
		return rv;
	}
	rv = wtfRenamer::doGetFileName(newname, &fnewname);
	if (rv != 0)
	{
		return rv;
	}
	if (foldname.IsDir())
	{
		wxLogDebug(wxT("'%s' is a directory: rename is not supported"), foldname.GetFullPath().c_str());
		return W_RENAMER_ERROR_NOT_SUPPORTED;
	}
	if (fnewname.IsDir() || wxDirExists(fnewname.GetFullPath()))
	{
		wxLogDebug(wxT("target '%s' is a directory"), fnewname.GetFullPath().c_str());
		fnewname.Assign(fnewname.GetFullPath(), foldname.GetName(), foldname.GetExt());
		if (!fnewname.IsDirWritable())
		{
			wxLogDebug(wxT("rename failed: permission denied"));
			return W_RENAMER_ERROR_NOT_PERMITTED;
		}
	}
	if (!foldname.FileExists())
	{
		wxLogDebug(wxT("rename failed: '%s' not found"), foldname.GetFullPath().c_str());
		return W_RENAMER_ERROR_FILE_NOT_EXISTS;
	}
	if (fnewname.FileExists())
	{
		wxLogDebug(wxT("rename: '%s' already exists"), fnewname.GetFullPath().c_str());
		return W_RENAMER_ERROR_FILE_EXISTS;
	}
	if (!foldname.IsDirWritable() || !fnewname.IsDirWritable())
	{
		wxLogDebug(wxT("rename failed: permission denied to either '%s' or '%s'"), foldname.GetFullPath().c_str(), fnewname.GetFullPath().c_str());
		return W_RENAMER_ERROR_NOT_PERMITTED;
	}

	wxASSERT_MSG(foldname.FileExists(), wxT("Oldname not exists"));
	wxASSERT_MSG(!fnewname.FileExists(), wxT("Newname exists"));

	if (!wxRenameFile(foldname.GetFullPath(), fnewname.GetFullPath(), false) || !fnewname.FileExists())
	{
		return W_RENAMER_ERROR_SYS_ERROR;
	}
	return 0;
}

int wtfRenamer::doCreateHardlink(const wxString& oldname, const wxString& newname)
{
#ifdef __WXMSW__
	if (!wtfRenamer::HasHardlinkSupport())
	{
		wxLogDebug(wxT("wtfRenamer::doCreateHardlink(): no hard link support"));
		return W_RENAMER_ERROR_NOT_SUPPORTED;
	}
	wxFileName foldname, fnewname;

	int rc = wtfRenamer::doGetFileName(oldname, &foldname); 
	if (rc != 0)
	{
		return rc;
	}
	if (!foldname.FileExists())
	{
		wxLogDebug(wxT("wtfRenamer::doCreateHardlink(): file oldname not found"));
		return W_RENAMER_ERROR_FILE_NOT_EXISTS;
	}

	rc = wtfRenamer::doGetFileName(newname, &fnewname);
	if (rc != 0)
	{
		return rc;
	}
	if (fnewname.FileExists())
	{
		wxLogDebug(wxT("wtfRenamer::doCreateHardlink(): file newname already exists"));
		return W_RENAMER_ERROR_FILE_EXISTS;
	}

	wxASSERT_MSG(foldname.FileExists(), wxT("Oldname not exists"));
	wxASSERT_MSG(!fnewname.FileExists(), wxT("Newname exists"));

	BOOL rv = FALSE;
#	ifdef _UNICODE
	wtfRenamer::m_HLSfuncW chlW;
	if (wtfRenamer::m_HLShasW)
	{
		chlW = (wtfRenamer::m_HLSfuncW) m_lib->GetSymbol(wxT("CreateHardLinkW"));
		if (chlW == NULL)
		{
			wtfRenamer::m_HLShasW = false;
			wxLogDebug(wxT("Cannot get symbol CreateHardLinkW in wtfRenamer::doCreateHardlink()"));
		} else {
			rv = chlW((LPCWSTR) fnewname.GetFullPath().c_str(), (LPCWSTR) foldname.GetFullPath().c_str(), NULL);
			if (!rv)
			{
				wxLogDebug(wxT("Cannot create hard link"));
			}
		}
	}
#	endif
	wtfRenamer::m_HLSfuncA chlA;
	LPCSTR lpFileNameA, lpExistingFileNameA;
	if (!rv && wtfRenamer::m_HLShasA)
	{
		chlA = (wtfRenamer::m_HLSfuncA) m_lib->GetSymbol(wxT("CreateHardLinkA"));
		if (chlA == NULL)
		{
			wtfRenamer::m_HLShasA = false;
			wxLogDebug(wxT("Cannot get symbol CreateHardLinkA in wtfRenamer::doCreateHardlink()"));
		} else {
			rv = TRUE;//chlA(lpFileNameA, lpExistingFileNameA, NULL);
		}
	}
	unsigned long last_error = wxSysErrorCode();
	int err;
	if (!rv || !fnewname.FileExists())
	{
		switch (last_error)
		{
			case ERROR_INVALID_FUNCTION:
				err = W_RENAMER_ERROR_NOT_SUPPORTED;
				break;
			case ERROR_NOT_SAME_DEVICE:
				err = W_RENAMER_ERROR_CROSS_FS;
				break;
			case ERROR_ACCESS_DENIED:
				err = W_RENAMER_ERROR_NOT_PERMITTED;
				break;
			default:
				err = W_RENAMER_ERROR_SYS_ERROR;
				wxLogDebug(wxT("CreateHardLink function returned error code: %d; %s"), last_error, wxSysErrorMsg(last_error));
				break;
		}
		return err;
	}
	return 0;
#else
#	ifdef __UNIX__
	return W_RENAMER_ERROR_NOT_SUPPORTED;
#	else
	return W_RENAMER_ERROR_NOT_SUPPORTED;
#	endif
#endif
}

void wtfRenamer::SetOptions(wsdgRenamerOptions* opts)
{
	if (m_opts != NULL)
	{
		delete(m_opts);
	}
	m_opts = opts;
}

bool wtfRenamer::Rename(int mode, long flags)
{
	m_undo.Clear();
	m_error = DoRename(mode, flags, false);
	return (m_error == 0);
}

bool wtfRenamer::Undo()
{
	m_error = DoRename(0, 0, true);
	return (m_error == 0);
}

int wtfRenamer::DoRename(int mode, long flags, bool undo)
{
	int rc = 0;
	int rv;
	size_t i, cnt;

	wtfRenamerAction *action;
	wtfRenamerActionType action_type;
	wtfRenamerActionData action_data;

	wtfRenamerAnswer ans;
	bool flag;

	if (undo)
	{
		wxLogDebug(wxT("Start undoing rename process"));
		cnt = m_undo.GetCount();
	} else {
		wxLogDebug(wxT("Start rename process"));
		m_undo.Clear();
		rv = CreateRenameProcess(mode, flags);
		if (rv == W_RENAMER_ERROR_CONFLICT)
		{
			return rv;
		}
		if (rv != 0)
		{
			ans = wtfRenamerAnswerRetry;
			while (rv != 0 && ans == wtfRenamerAnswerRetry)
			{
				rv = CreateRenameProcess(mode, flags);
				if (rv != 0)
				{
					ans = m_opts->OnError(NULL, rv);
				}
			}
			if (rv != 0)
			{
				return rv;
			}
		}
		cnt = m_process.GetCount();
	}
	i = 0;
	while (i < cnt)
	{
		if (undo)
		{
			action = &m_undo.Item(i);
		} else {
			action = &m_process.Item(i);
		}
		ans = wtfRenamerAnswerRetry;
		rv = 1;
		while (rv != 0 && ans == wtfRenamerAnswerRetry)
		{
			rv = action->PerformAction();
			if (rv != 0)
			{
				ans = m_opts->OnError(action, rv);
			}
		}
		if (rv != 0 && ans != wtfRenamerAnswerSkip)
		{
			return rv;
		}
		i++;
		if (undo || ans == wtfRenamerAnswerSkip)
		{
			if (ans = wtfRenamerAnswerSkip)
			{
				rc = rv;
			}
			continue;
		}
		flag = true;
		switch (action->m_type)
		{
			case wtfRenamerActionCreateDir:
				action_type = wtfRenamerActionRemoveDir;
				action_data.path = WXDEBUG_NEW wxString();
				action_data.newpath = NULL;
				*(action_data.path) = *(action->m_data.path);
				wxLogDebug(wxT("undo action: remove dir '%s'"), action_data.path->c_str());
				break;
			case wtfRenamerActionRemoveDir:
				action_type = wtfRenamerActionCreateDir;
				action_data.path = WXDEBUG_NEW wxString();
				action_data.newpath = NULL;
				*(action_data.path) = *(action->m_data.path);
				wxLogDebug(wxT("undo action: create dir '%s'"), action_data.path->c_str());
				break;
			case wtfRenamerActionRenameFile:
				action_type = wtfRenamerActionRenameFile;
				action_data.path = WXDEBUG_NEW wxString();
				action_data.newpath = WXDEBUG_NEW wxString();
				*(action_data.path) = *(action->m_data.newpath);
				*(action_data.newpath) = *(action->m_data.path);
				wxLogDebug(wxT("undo action: '%s' -> '%s'"), action_data.path->c_str(), action_data.newpath->c_str());
				break;
			case wtfRenamerActionCopyFile:
			case wtfRenamerActionCreateHardLink:
			case wtfRenamerActionCreateSymlink:
				action_type = wtfRenamerActionUnlinkFile;
				action_data.path = WXDEBUG_NEW wxString();
				action_data.newpath = NULL;
				*(action_data.path) = *(action->m_data.newpath);
				wxLogDebug(wxT("undo action: unlink '%s'"), action_data.path->c_str());
				break;
			default:
				wxLogDebug(wxT("undo action: no action"));
				flag = false;
				break;
		}
		if (flag)
		{
			action = WXDEBUG_NEW wtfRenamerAction(action_type, action_data);
			m_undo.Add(action);
		}
	}
	if (undo)
	{
		m_undo.Clear();
	}
	return rc;
}

int wtfRenamer::CreateRenameProcess(int mode, long flags)
{
	wtfArrayNames existing_files, dirs_done;
	wtfRenamerProcess delayed;

	wxString oldname, newname;
	wxFileName foldname, fnewname;

	wtfRenamerAction *action;
	wtfRenamerActionData action_data;
	wtfRenamerActionType action_type;

	wxArrayString dirs;
	size_t i, j, cnt, d, dcnt;

	wxLogDebug(wxT("Start creating rename process"));
	wxASSERT_MSG(m_files.GetCount() == m_names.GetCount(), wxT("different count of files and names"));
	cnt = m_files.GetCount();
	
	switch (mode)
	{
		case W_RENAMER_CREATE_HARDLINK:
			action_type = wtfRenamerActionCreateHardLink;
			break;
		case W_RENAMER_CREATE_SYMLINK:
			action_type = wtfRenamerActionCreateSymlink;
			break;
		default:
			action_type = wtfRenamerActionRenameFile;
			break;
	}

	for (i = 0; i < cnt; i++)
	{
		newname = m_names.Item(i);
		if (existing_files.count(newname) && m_files.Item(i).CmpNoCase( m_files.Item(existing_files[newname]) ))
		{
			wxLogDebug(wxT("Duplicate name '%s' for files '%s' and '%s'"), newname.c_str(), m_files.Item(i).c_str(), m_files.Item( existing_files[newname] ).c_str());
			return W_RENAMER_ERROR_CONFLICT;
		}
		existing_files[newname] = i;
	}
	existing_files.clear();

	for (i = 0; i < cnt; i++)
	{
		oldname = m_files.Item(i);
		if (existing_files.count(oldname) && m_names.Item(i).CmpNoCase( m_names.Item(existing_files[oldname]) ))
		{
			wxLogDebug(wxT("Conflict found: there two different names '%s' and '%s' for file '%s'"), m_names.Item(i).c_str(), m_names.Item( existing_files[oldname] ).c_str(), oldname.c_str());
			return W_RENAMER_ERROR_CONFLICT;
		}
		existing_files[oldname] = i;
	}

	if (mode != W_RENAMER_RENAME)
	{
		existing_files.clear();
	}

	j = 0;
	action = NULL;
	action_data.path = NULL;
	action_data.newpath = NULL;
	for (i = 0; i < cnt; i++)
	{
		oldname = m_files.Item(i);
		newname = m_names.Item(i);
		foldname.Assign(oldname);
		fnewname.Assign(newname);
		if (!foldname.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_LONG | wxPATH_NORM_TILDE)
			|| !fnewname.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_LONG | wxPATH_NORM_TILDE)
			|| wxDirExists(fnewname.GetFullPath()))
		{
			return W_RENAMER_ERROR_CONFLICT;
		}
		if (fnewname.FileExists())
		{
			if (existing_files.count(newname) == 0)
			{
				return W_RENAMER_ERROR_FILE_EXISTS;
			}
			action_data.newpath = WXDEBUG_NEW wxString( fnewname.GetFullPath() );
			while (fnewname.FileExists())
			{
				newname = wxString::Format(wxT("wtf%02x"), j);
				fnewname.SetName(newname);
				j++;
			}
			action_data.path = WXDEBUG_NEW wxString( fnewname.GetFullPath() );
			action = WXDEBUG_NEW wtfRenamerAction(action_type, action_data);
			delayed.Add(action);
			action_data.path = NULL;
			action_data.newpath = NULL;
			action = NULL;
		} else if ((flags & W_RENAMER_OPTION_CREATE_DIRS) && !( wxDirExists(fnewname.GetPath()) )) {
			dirs = fnewname.GetDirs();
			dcnt = dirs.GetCount();
			for (d = 0; d < dcnt; d++)
			{
				fnewname.RemoveLastDir();
			}
			for (d = 0; d < dcnt; d++)
			{
				fnewname.AppendDir(dirs.Item(d));
				if (!wxDirExists( fnewname.GetPath() ) && dirs_done.count( fnewname.GetPath() ) == 0)
				{
					action_data.path = WXDEBUG_NEW wxString();
					*(action_data.path) = fnewname.GetPath();
					wxLogDebug(wxT("action: create dir '%s'"), action_data.path->c_str());
					action = WXDEBUG_NEW wtfRenamerAction(wtfRenamerActionCreateDir, action_data);
					m_process.Add(action);
					dirs_done[ fnewname.GetPath() ] = i;
				}
			}
			action = NULL;
			action_data.path = NULL;
		}
		action_data.path = WXDEBUG_NEW wxString(foldname.GetFullPath());
		action_data.newpath = WXDEBUG_NEW wxString(fnewname.GetFullPath());
		wxLogDebug(wxT("action: %s -> %s"), action_data.path->c_str(), action_data.newpath->c_str());
		action = WXDEBUG_NEW wtfRenamerAction(action_type, action_data);
		m_process.Add(action);
		action_data.path = NULL;
		action_data.newpath = NULL;
		action = NULL;
	}

	cnt = delayed.GetCount();
	for (i = cnt; i > 0; i--)
	{
		action = delayed.Detach(i - 1);
		m_process.Add(action);
		wxLogDebug(wxT("action: %s -> %s"), action->m_data.path->c_str(), action->m_data.newpath->c_str());
	}
	return 0;
}

void wtfRenamer::Free()
{
#ifdef __WXMSW__
	if (wtfRenamer::m_lib != NULL)
	{
		delete(wtfRenamer::m_lib);
		wtfRenamer::m_lib = NULL;
	}
#endif
}

