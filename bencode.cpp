#include "bencode.h"


IMPLEMENT_ABSTRACT_CLASS(wbenData, wtfBencode)

wbenData::wbenData(wbenDataType dtype)
{
    m_type = dtype;
    m_error = 0;
    m_parent = NULL;
    m_item = this;
    m_data.m_int = NULL;
    m_data.m_str = NULL;
    m_data.m_list = NULL;
    m_data.m_dict = NULL;

    switch (dtype)
    {
        case wbenDATA_INTEGER       :
        case wbenDATA_STRING        :
        case wbenDATA_LIST          :
        case wbenDATA_DICTIONARY    :
        case wbenDATA_UNDEFINED     :
            break;

        default:
            m_type = wbenDATA_UNDEFINED;
            m_error = W_BENCODE_ERROR_UNKNOWN_DATA_TYPE;
            break;
    }
}

wbenData::wbenData(const wbenData& data)
{
    m_type = data.GetType();
    m_parent = data.GetParent();
    m_item = this;
    m_error = data.GetError();
    wxLogDebug(wxT("copy %x of wbenData %x"), this, &data);

    wbenDataValue v = data.GetValue();
    wbenDataDictionary::iterator it;
    size_t i;
    wbenData *item;
    switch (m_type)
    {
        case wbenDATA_INTEGER       :
            m_data.m_int = (wbenDataInteger *) malloc(sizeof(wbenDataInteger));
            *(m_data.m_int) = *(v.m_int);
            break;
        case wbenDATA_STRING        :
            m_data.m_str = WXDEBUG_NEW wbenDataString();
            *(m_data.m_str) = *(v.m_str);
            break;
        case wbenDATA_LIST          :
            m_data.m_list = WXDEBUG_NEW wbenDataList();
            for (i = 0; i < v.m_list->GetCount(); i++)
            {
                item = WXDEBUG_NEW wbenData( v.m_list->Item(i) );
                item->SetParent(this);
                m_data.m_list->Add(item);
            }
            break;
        case wbenDATA_DICTIONARY    :
            m_data.m_dict = WXDEBUG_NEW wbenDataDictionary();
            for (it = v.m_dict->begin(); it != v.m_dict->end(); ++it)
            {
                item = WXDEBUG_NEW wbenData( *(it->second) );
                item->SetParent(this);
                (*m_data.m_dict)[it->first] = item;
            }
            break;
        default                     :
            m_type = wbenDATA_UNDEFINED;
            break;
    }
}

wbenData::wbenData(size_t size, char *str)
{
    wbenContext ctx;
    wbenCallbacks cbs;

    m_error = 0;
    m_parent = NULL;
    m_item = this;
    m_data.m_int = NULL;
    m_data.m_str = NULL;
    m_data.m_list = NULL;
    m_data.m_dict = NULL;
    if (size == 0)
    {
        m_type = wbenDATA_UNDEFINED;
        return;
    }

    ctx.type = wbenCtxRoot;
    ctx.path = WXDEBUG_NEW wbenContextPath;
    ctx.dtype = wbenDATA_UNDEFINED;
    ctx.str = str;
    ctx.size = size;
    ctx.dsize = 0;
    ctx.pos = 0;
    ctx.err = 0;
    ctx.sig = wbenSigContinue;

    cbs.obj = (void *) this;
    cbs.cb_int = wbenCbInteger(wbenData::wbenCreateInteger);
    cbs.cb_str = wbenCbString(wbenData::wbenCreateString);
    cbs.cb_list = wbenCbList(wbenData::wbenCreateList);
    cbs.cb_dict = wbenCbDictionary(wbenData::wbenCreateDictionary);
    cbs.cb_endtag = wbenCbEndTag(wbenData::wbenEndTag);

    wtfBencode::bdecode(&ctx, cbs);
    m_error = ctx.err;
    ctx.str = NULL;
    delete(ctx.path);
    ctx.path = NULL;
}

wbenData::~wbenData()
{
    Clear();
}

void wbenData::SetType(wbenDataType dtype)
{
    Clear();
    m_type = dtype;
    switch (dtype)
    {
        case wbenDATA_INTEGER       :
        case wbenDATA_STRING        :
        case wbenDATA_LIST          :
        case wbenDATA_DICTIONARY    :
        case wbenDATA_UNDEFINED     :
            break;

        default:
            m_type = wbenDATA_UNDEFINED;
            m_error = W_BENCODE_ERROR_UNKNOWN_DATA_TYPE;
            break;
    }
}

void wbenData::SetValue(wbenDataValue val)
{
    wbenDataType tp = m_type;
    Clear();
    m_type = tp;
    switch (m_type)
    {
        case wbenDATA_INTEGER       :
            m_data.m_int = val.m_int;
            break;
        case wbenDATA_STRING        :
            m_data.m_str = val.m_str;
            break;
        case wbenDATA_LIST          :
            m_data.m_list = val.m_list;
            break;
        case wbenDATA_DICTIONARY    :
            m_data.m_dict = val.m_dict;
            break;

        default:
            break;
    }
}

void wbenData::SetParent(wbenData *item)
{
    m_parent = item;
}

wbenDataType wbenData::GetType() const
{
    return m_type;
}

wbenDataValue wbenData::GetValue() const
{
    return m_data;
}

void wbenData::Clear()
{
    if (m_type == wbenDATA_UNDEFINED)
    {
        return;
    }
    wbenDataDictionary::iterator it;
    switch (m_type)
    {
        case wbenDATA_INTEGER       :
            if (m_data.m_int != NULL)
            {
                free(m_data.m_int);
                m_data.m_int = NULL;
            }
            break;
        case wbenDATA_STRING        :
            if (m_data.m_str != NULL)
            {
                m_data.m_str->Clear();
                delete(m_data.m_str);
                m_data.m_str = NULL;
            }
            break;
        case wbenDATA_LIST          :
            if (m_data.m_str != NULL)
            {
                m_data.m_list->Clear();
                delete(m_data.m_list);
                m_data.m_list = NULL;
            }
            break;
        case wbenDATA_DICTIONARY    :
            if (m_data.m_str != NULL)
            {
                for (it = m_data.m_dict->begin(); it != m_data.m_dict->end(); ++it)
                {
                    delete(it->second);
                    (*(m_data.m_dict))[it->first] = NULL;
                }
                m_data.m_dict->clear();
                delete(m_data.m_dict);
                m_data.m_dict = NULL;
            }
            break;

        default:
            break;
    }
    m_type = wbenDATA_UNDEFINED;
    m_item = this;
    m_data.m_int = NULL;
    m_data.m_str = NULL;
    m_data.m_list = NULL;
    m_data.m_dict = NULL;
}

void wbenData::Undef()
{
    switch (m_type)
    {
        case wbenDATA_INTEGER       :
            m_data.m_int = NULL;
            break;
        case wbenDATA_STRING        :
            m_data.m_str = NULL;
            break;
        case wbenDATA_LIST          :
            m_data.m_str = NULL;
            break;
        case wbenDATA_DICTIONARY    :
            m_data.m_str = NULL;
            break;

        default:
            break;
    }
    m_type = wbenDATA_UNDEFINED;
}

int wbenData::error()
{
    int rv = m_error;
    m_error = 0;

    return rv;
}

int wbenData::GetError() const
{
    return m_error;
}

wbenData *wbenData::GetParent() const
{
    return m_parent;
}

wbenData* wbenData::GetItem(size_t i)
{
    if ((m_type == wbenDATA_LIST) && (i < m_data.m_list->GetCount()))
    {
        return (&(m_data.m_list->Item(i)));
    }
    return NULL;
}

size_t wbenData::GetItemCount()
{
    if (m_type == wbenDATA_LIST)
    {
        return m_data.m_list->GetCount();
    }
    return 0;
}

wbenData* wbenData::GetItem(const wbenDataString& key)
{
    if ((m_type == wbenDATA_DICTIONARY) && (m_data.m_dict->count(key)))
    {
        return ((*m_data.m_dict)[key]);
    }
    return NULL;
}

wbenData* wbenData::AddItem(const wbenDataType dtype, const wbenDataValue& value)
{
    wbenData *item;

    if (m_type == wbenDATA_LIST)
    {
        item = WXDEBUG_NEW wbenData(dtype);
        item->SetValue(value);
        m_data.m_list->Add(item);
        return item;
    }
    return this;
}

bool wbenData::KeyExists(const wbenDataString& key)
{
    return ((m_type == wbenDATA_DICTIONARY) && (m_data.m_dict->count(key)));
}

void wbenData::RemoveKey(const wbenDataString& key)
{
    if ((m_type == wbenDATA_DICTIONARY) && (m_data.m_dict->count(key)))
    {
        delete((*m_data.m_dict)[key]);
        (*m_data.m_dict)[key] = NULL;
        m_data.m_dict->erase(key);
    }
}

wbenData* wbenData::AddKey(const wbenDataString& key)
{
    wbenData *item;
    if ((m_type == wbenDATA_DICTIONARY) && (m_data.m_dict->count(key) == 0))
    {
        item = WXDEBUG_NEW wbenData(wbenDATA_UNDEFINED);
        (*(m_data.m_dict))[key] = item;
        return item;
    }
    return this;
}

void wbenData::wbenCreateInteger(wbenContext *ctx, long value)
{
    wbenDataValue v;
    v.m_int = (wbenDataInteger *) malloc(sizeof(wbenDataInteger));
    *(v.m_int) = (wbenDataInteger) value;

    switch (ctx->type)
    {
        case wbenCtxRoot:
            SetType(wbenDATA_INTEGER);
            SetValue(v);
            break;
        case wbenCtxList:
            m_item->AddItem(wbenDATA_INTEGER, v);
            break;
        case wbenCtxDictKey:
            m_item->SetType(wbenDATA_INTEGER);
            m_item->SetValue(v);
            wbenEndTag(ctx);
            break;
        default:
            break;
    }

    ctx->sig = wbenSigContinue;
}

void wbenData::wbenCreateString(wbenContext *ctx)
{
    wbenDataValue v;
    wbenData *item;
    v.m_str = WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, wxConvUTF8, ctx->dsize);

    switch (ctx->type)
    {
        case wbenCtxRoot:
            SetType(wbenDATA_STRING);
            SetValue(v);
            break;
        case wbenCtxList:
            m_item->AddItem(wbenDATA_STRING, v);
            break;
        case wbenCtxDict:
            item = m_item->AddKey(*(v.m_str));
            item->SetParent(m_item);
            m_item = item;
            delete(v.m_str);
            break;
        case wbenCtxDictKey:
            m_item->SetType(wbenDATA_STRING);
            m_item->SetValue(v);
            wbenEndTag(ctx);
            break;
        default:
            break;
    }

    ctx->sig = wbenSigContinue;
}

void wbenData::wbenCreateList(wbenContext *ctx)
{
    wbenDataValue v;
    wbenData *item;
    v.m_list = WXDEBUG_NEW wbenDataList();

    switch (ctx->type)
    {
        case wbenCtxRoot:
            SetType(wbenDATA_LIST);
            SetValue(v);
            break;
        case wbenCtxList:
            item = m_item->AddItem(wbenDATA_LIST, v);
            item->SetParent(m_item);
            m_item = item;
            break;
        case wbenCtxDictKey:
            m_item->SetType(wbenDATA_LIST);
            m_item->SetValue(v);
            break;
        default:
            break;
    }

    ctx->sig = wbenSigContinue;
}

void wbenData::wbenCreateDictionary(wbenContext *ctx)
{
    wbenDataValue v;
    wbenData *item;
    v.m_dict = WXDEBUG_NEW wbenDataDictionary();

    switch (ctx->type)
    {
        case wbenCtxRoot:
            SetType(wbenDATA_DICTIONARY);
            SetValue(v);
            break;
        case wbenCtxList:
            item = m_item->AddItem(wbenDATA_DICTIONARY, v);
            item->SetParent(m_item);
            m_item = item;
            break;
        case wbenCtxDictKey:
            m_item->SetType(wbenDATA_DICTIONARY);
            m_item->SetValue(v);
            break;
        default:
            break;
    }

    ctx->sig = wbenSigContinue;
}

void wbenData::wbenEndTag(wbenContext *ctx)
{
    m_item = m_item->GetParent();
}


IMPLEMENT_ABSTRACT_CLASS(wtfBencode, wxObject)

long wtfBencode::_parseInt(wbenContext *ctx, bool issigned)
{
    long res = 0;
    char ch;

    if (ctx->pos >= ctx->size)
    {
        return res;
    }
    ch = *(ctx->str + ctx->pos);
    if ((ch < '0' || ch > '9') && (!issigned || ch != '-'))
    {
        return res;
    }
    if (issigned && (ch == '-'))
    {
        ctx->pos++;
        res = -1;
    } else {
        res = 1;
    }
    while ((ctx->pos < ctx->size) && (*(ctx->str + ctx->pos) == '0'))
    {
        ctx->pos++;
    }
    if (ctx->pos >= ctx->size)
    {
        return 0;
    }
    ch = *(ctx->str + ctx->pos);
    if ((ch > '0') && (ch <= '9'))
    {
        res *= ch - '0';
        ctx->pos++;
    } else {
        res = 0;
    }
    while ((ctx->pos < ctx->size) && (*(ctx->str + ctx->pos) >= '0') && (*(ctx->str + ctx->pos) <= '9'))
    {
        res *= 10;
        res += *(ctx->str + ctx->pos) - '0';
        ctx->pos++;
    }
    return res;
}

bool wtfBencode::bdecode(wbenContext *ctx, wbenCallbacks cbs)
{
    size_t i;
    long len, j;

    if (ctx->size == 0 || ctx->str == NULL)
    {
        return false;
    }
    while (ctx->pos < ctx->size)
    {
        switch (*(ctx->str + ctx->pos))
        {
            case 'i':
                if (ctx->type == wbenCtxDict)
                {
                    // key in dictionary
                    ctx->err = W_BENCODE_ERROR_INVALID_KEY;
                }
                if ((ctx->err == 0) && (ctx->pos + 1 >= ctx->size))
                {
                    ctx->err = W_BENCODE_ERROR_NO_END_TAG;
                }
                if (ctx->err == 0)
                {
                    ctx->pos++;
                    i = (*(ctx->str + ctx->pos) == '-') ? ctx->pos + 1 : ctx->pos;
                    if (i >= ctx->size)
                    {
                        ctx->err = W_BENCODE_ERROR_NO_END_TAG;
                    }
                }
                if ((ctx->err == 0) && ((*(ctx->str + i) < '0') || (*(ctx->str + i) > '9')))
                {
                    ctx->err = W_BENCODE_ERROR_UNEXPECTED_TOKEN;
                }
                if (ctx->err == 0)
                {
                    i = ctx->pos;
                    j = wtfBencode::_parseInt(ctx, true);
                    if (ctx->pos >= ctx->size)
                    {
                        ctx->err = W_BENCODE_ERROR_NO_END_TAG;
                    }
                }
                if ((ctx->err == 0) && (*(ctx->str + ctx->pos) != 'e'))
                {
                    ctx->err = W_BENCODE_ERROR_UNEXPECTED_TOKEN;
                }
                if (ctx->err != 0)
                {
                    ctx->dtype = wbenDATA_UNDEFINED;
                    return false;
                }
                len = ctx->dsize = ctx->pos - i;
                ctx->pos = i;
                if (cbs.obj != NULL && cbs.cb_int != NULL)
                {
                    (((wtfBencode *)(cbs.obj))->*(cbs.cb_int))(ctx, j);
                }
                ctx->pos += len + 1;
                // Now ctx->pos points to next token
                if (ctx->sig == wbenSigStop)
                {
                    // Stop signal recieved
                    return true;
                }
                if (ctx->type == wbenCtxDictKey)
                {
                    ctx->type = wbenCtxDict;
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                len = wtfBencode::_parseInt(ctx, false);
                if ((ctx->pos < ctx->size) && (*(ctx->str + ctx->pos) != ':'))
                {
                    ctx->err = W_BENCODE_ERROR_UNEXPECTED_TOKEN;
                }
                if ((ctx->err == 0) && (ctx->pos + len >= ctx->size))
                {
                    ctx->err = W_BENCODE_ERROR_NO_END_TAG;
                }
                if (ctx->err != 0)
                {
                    ctx->dtype = wbenDATA_UNDEFINED;
                    return false;
                }
                ctx->pos++;
                ctx->dsize = (size_t) len;
                // Now ctx->pos point to the beginnig of the string
                // ctx->dsize is the length of the string in bytes
                if (cbs.obj != NULL && cbs.cb_str != NULL)
                {
                    (((wtfBencode *)(cbs.obj))->*(cbs.cb_str))(ctx);
                }
                ctx->pos += len;
                // Now ctx->pos points to next token
                if (ctx->sig == wbenSigStop)
                {
                    // Stop signal recieved
                    return true;
                }
                switch (ctx->type)
                {
                    case wbenCtxDict:
                        ctx->type = wbenCtxDictKey;
                        break;
                    case wbenCtxDictKey:
                        ctx->type = wbenCtxDict;
                        break;
                    default:
                        break;
                }
                break;
            case 'l':
                if (ctx->type == wbenCtxDict)
                {
                    // key in dictionary
                    ctx->err = W_BENCODE_ERROR_INVALID_KEY;
                }
                ctx->pos++;
                if ((ctx->err != 0) || (ctx->pos >= ctx->size))
                {
                    if (ctx->err == 0)
                    {
                        ctx->err = W_BENCODE_ERROR_NO_END_TAG;
                    }
                    return false;
                }
                if (cbs.obj != NULL && cbs.cb_list != NULL)
                {
                    (((wtfBencode *)(cbs.obj))->*(cbs.cb_list))(ctx);
                }
                if (ctx->sig == wbenSigStop)
                {
                    // Stop signal recieved
                    return true;
                }
                ctx->type = wbenCtxList;
                ctx->path->Add(wbenCtxList);
                break;
            case 'd':
                if (ctx->type == wbenCtxDict)
                {
                    // key in dictionary
                    ctx->err = W_BENCODE_ERROR_INVALID_KEY;
                }
                ctx->pos++;
                if ((ctx->err != 0) || (ctx->pos >= ctx->size))
                {
                    if (ctx->err == 0)
                    {
                        ctx->err = W_BENCODE_ERROR_NO_END_TAG;
                    }
                    return false;
                }
                if (cbs.obj != NULL && cbs.cb_dict != NULL)
                {
                    (((wtfBencode *)(cbs.obj))->*(cbs.cb_dict))(ctx);
                }
                if (ctx->sig == wbenSigStop)
                {
                    // Stop signal recieved
                    return true;
                }
                ctx->type = wbenCtxDict;
                ctx->path->Add(wbenCtxDict);
                break;
            case 'e':
                if (ctx->path->GetCount() == 0)
                {
                    ctx->err = W_BENCODE_ERROR_UNEXPECTED_TOKEN;
                } else {
                    if (ctx->type == wbenCtxDictKey)
                    {
                        ctx->err = W_BENCODE_ERROR_ODD_ELEMENTS_IN_DICTIONARY;
                    }
                }
                if (ctx->err != 0)
                {
                    return false;
                }
                ctx->path->RemoveAt(ctx->path->GetCount() - 1);
                ctx->pos++;
                if (cbs.obj != NULL && cbs.cb_endtag != NULL)
                {
                    (((wtfBencode *)(cbs.obj))->*(cbs.cb_endtag))(ctx);
                }
                ctx->type = (ctx->path->GetCount() == 0) ? wbenCtxRoot : ctx->path->Last();
                break;
            default:
                ctx->err = W_BENCODE_ERROR_UNEXPECTED_TOKEN;
                ctx->dtype = wbenDATA_UNDEFINED;
                return false;
        }
    }
    if (ctx->path->GetCount() > 0)
    {
        ctx->err = W_BENCODE_ERROR_NO_END_TAG;
        return false;
    }
    return true;
}


IMPLEMENT_ABSTRACT_CLASS(wtfTorrentFile, wtfBencode)

wtfTorrentFile::wtfTorrentFile()
{
    m_data = NULL;
    m_path = NULL;
    m_item = NULL;
    m_files = WXDEBUG_NEW wbenDataList();
    m_error = 0;
    skip_key = 0;
    enc_key = 0;
    enc_key_i = 0;
    genc = NULL;
    genc_delete = false;
}

wtfTorrentFile::wtfTorrentFile(const wxString& filename, const wxString& encoding)
{
    m_error = 0;
    m_data = NULL;
    m_path = NULL;
    m_item = NULL;
    m_files = WXDEBUG_NEW wbenDataList();
    skip_key = 0;
    enc_key = 0;
    enc_key_i = 0;
    genc = NULL;
    genc_delete = false;
    Load(filename, encoding);
}

wtfTorrentFile::~wtfTorrentFile()
{
    m_encoding.Clear();
    if (m_data != NULL)
    {
        delete(m_data);
    }
    if (m_files != NULL)
    {
        m_files->Clear();
        delete(m_files);
    }
    if (genc_delete && genc != NULL)
    {
        delete(genc);
    }
    genc_delete = false;
    genc = NULL;
}

void wtfTorrentFile::SetGenc(wtfGEncoding *ge)
{
    if (genc_delete && genc != NULL)
    {
        delete(genc);
    }
    genc = ge;
    genc_delete = false;
}

size_t wtfTorrentFile::GetFilesCount()
{
    return ((m_files == NULL) ? 0 : m_files->GetCount());
}

wbenData* wtfTorrentFile::GetFileInfo(size_t i)
{
    return ((m_files == NULL) ? NULL : &(m_files->Item(i)));
}

wxString& wtfTorrentFile::GetFileName(size_t i)
{
    wbenData *path = m_files->Item(i).GetItem(wxT("path"));

    return (*(path->GetItem(path->GetItemCount() - 1)->GetValue().m_str));
}

wxString wtfTorrentFile::GetFileFullName(size_t i)
{
    wxString res;

    if (i >= m_files->GetCount())
    {
        return res;
    }

    wbenData *path = m_files->Item(i).GetItem(wxT("path"));
    size_t j;
    size_t cnt = path->GetItemCount();

    if (cnt == 0)
    {
        return res;
    }
    res = *(path->GetItem(0)->GetValue().m_str);

    for (j = 1; j < cnt; j++)
    {
        res += wxFileName::GetPathSeparator() + *(path->GetItem(j)->GetValue().m_str);
    }
    return res;
}

wxString& wtfTorrentFile::GetFileSize(size_t i)
{
    return (*(m_files->Item(i).GetItem(wxT("length"))->GetValue().m_str));
}

bool wtfTorrentFile::Load(const wxString& filename, const wxString& encoding)
{
    wxFile f;
    char *buf;
    int wben_error;
    wbenCallbacks cbs;
    wbenContext ctx;
    size_t i;

    m_error = 0;
    if (!wxFile::Exists(filename))
    {
        m_error = wtfFILE_NOT_EXISTS;
        wxLogError(wxT("File '") + filename + wxT("' does not exists."));
        return false;
    }
    if (!wxFile::Access(filename, wxFile::read))
    {
        m_error = wtfFILE_ACCESS_DENIED;
        wxLogError(wxT("Cannot open file '") + filename + wxT("' for reading: access denied."));
        return false;
    }
    f.Open(filename, wxFile::read);
    if (!f.IsOpened())
    {
        m_error = wtfFILE_CANNOT_OPEN;
        return false;
    }
    buf = (char *) malloc(f.Length() + 1);
    if (buf == NULL)
    {
        m_error = wtfCANNOT_ALLOCATE_MEMORY;
        f.Close();
        wxLogError(wxT("Cannot allocate memory"));
        return false;
    }
    if (f.Read(buf, f.Length()) != f.Length())
    {
        m_error = wtfFILE_CANNOT_READ_WHOLE;
        f.Close();
        free(buf);
        return false;
    }
    if (m_data != NULL)
    {
        delete(m_data);
    }
    if (m_files->GetCount() > 0)
    {
        m_files->Clear();
    }
    if (genc == NULL)
    {
        genc = WXDEBUG_NEW wtfGEncoding();
        genc_delete = true;
    }
    m_encoding = encoding;

    ctx.type = wbenCtxRoot;
    ctx.path = WXDEBUG_NEW wbenContextPath();
    ctx.dtype = wbenDATA_UNDEFINED;
    ctx.pos = 0;
    ctx.size = f.Length();
    ctx.dsize = 0;
    ctx.err = 0;
    ctx.str = buf;
    ctx.sig = wbenSigContinue;

    cbs.obj = (void *) this;
    cbs.cb_int = wbenCbInteger(wtfTorrentFile::wtfCreateInteger);
    cbs.cb_str = wbenCbString(wtfTorrentFile::wtfCreateString);
    cbs.cb_list = wbenCbList(wtfTorrentFile::wtfCreateList);
    cbs.cb_dict = wbenCbDictionary(wtfTorrentFile::wtfCreateDictionary);
    cbs.cb_endtag = wbenCbEndTag(wtfTorrentFile::wtfEndTag);

    m_path = WXDEBUG_NEW wbenDataList();
    m_data = WXDEBUG_NEW wbenData(wbenDATA_UNDEFINED);
    m_item = m_data;
    skip_key = 0;
    enc_key = 0;
    enc_key_i = 0;
    wtfBencode::bdecode(&ctx, cbs);
    f.Close();
    free(buf);
    delete(ctx.path);
    wben_error = m_data->error();
    switch (wben_error)
    {
        case 0:
            break;
        case W_BENCODE_ERROR_UNKNOWN_DATA_TYPE:
            m_error = wtfWBEN_UNKNOWN_DATA_TYPE;
            wxLogError(wxT("bencode error: Unknown data type"));
            break;
        case W_BENCODE_ERROR_UNEXPECTED_TOKEN:
            m_error = wtfWBEN_UNEXPECTED_TOKEN;
            wxLogError(wxT("bencode error: Unexpected token"));
            break;
        case W_BENCODE_ERROR_NO_END_TAG:
            m_error = wtfWBEN_NO_END_TAG;
            wxLogError(wxT("bencode error: No end tag"));
            break;
        case W_BENCODE_ERROR_ODD_ELEMENTS_IN_DICTIONARY:
            m_error = wtfWBEN_ODD_ELEMENTS;
            wxLogError(wxT("bencode error: Odd element in dictionary"));
            break;
        case W_BENCODE_ERROR_INVALID_KEY:
            m_error = wtfWBEN_INVALID_KEY;
            wxLogError(wxT("bencode error: Invalid key (must be a string)"));
            break;
        default:
            m_error = wtfWBEN_UNKNOWN_ERROR;
            wxLogError(wxT("bencode error: unknown error"));
            break;
    }
    delete(m_path);
    m_path = NULL;
    m_item = NULL;
    if (!m_data->KeyExists(wxT("info")))
    {
        m_error = wtfTF_NO_INFO;
        wxLogError(filename + wxT(": invalid data\nNo info section"));
    }
    if (m_data->GetItem(wxT("info"))->GetType() != wbenDATA_DICTIONARY)
    {
        m_error = wtfTF_INVALID_INFO_TYPE;
        wxLogError(filename + wxT(": invalid data\nInfo section must be of type dictionary"));
    }
    if (m_error != 0)
    {
        delete(m_data);
        m_data = NULL;
        return false;
    }

    wbenData *info_hash = m_data->GetItem(wxT("info"));
    wbenData *file = NULL;
    wbenDataDictionary::iterator it;
    wbenDataDictionary *node;
    wbenDataValue v;
    bool flag;
    size_t cnt, j, cnt2;

    if ((!info_hash->KeyExists(wxT("files"))) ||
        (info_hash->GetItem(wxT("files"))->GetItemCount() == 0))
    {
        file = WXDEBUG_NEW wbenData(wbenDATA_DICTIONARY);
        v.m_dict = WXDEBUG_NEW wbenDataDictionary();
        file->SetValue(v);
        file->AddKey(wxT("path"))->SetType(wbenDATA_LIST);
        v.m_list = WXDEBUG_NEW wbenDataList();
        file->GetItem(wxT("path"))->SetValue(v);
        node = info_hash->GetValue().m_dict;
        flag = true;
        for (it = node->begin(); it != node->end() && m_error == 0; ++it)
        {
            if (it->first == wxT("length"))
            {
                if (it->second->GetType() == wbenDATA_STRING)
                {
                    file->AddKey(wxT("length"))->SetType(wbenDATA_STRING);
                    v.m_str = WXDEBUG_NEW wbenDataString(*(it->second->GetValue().m_str));
                    file->GetItem(wxT("length"))->SetValue(v);
                } else {
                    m_error = wtfTF_INVALID_LENGTH_TYPE;
                    wxLogError(filename + wxT(": invalid data\nLength must be an integer"));
                }
            } else if ((flag) && (it->first == wxT("name"))) {
                if (it->second->GetType() == wbenDATA_STRING)
                {
                    v.m_str = WXDEBUG_NEW wbenDataString(*(it->second->GetValue().m_str));
                    file->GetItem(wxT("path"))->AddItem(wbenDATA_STRING, v);
                } else {
                    m_error = wtfTF_INVALID_NAME_TYPE;
                    wxLogError(filename + wxT(": invalid data\nName must be a string"));
                }
            } else if ((flag) && (it->first.Len() > 5) && (it->first.Left(5) == wxT("name."))) {
                if (it->second->GetType() == wbenDATA_STRING)
                {
                    if (file->GetItem(wxT("path"))->GetItemCount() > 0)
                    {
                        v = file->GetItem(wxT("path"))->GetItem(0)->GetValue();
                        *(v.m_str) = *(it->second->GetValue().m_str);
                    } else {
                        v.m_str = WXDEBUG_NEW wbenDataString(*(it->second->GetValue().m_str));
                        file->GetItem(wxT("path"))->AddItem(wbenDATA_STRING, v);
                    }
                } else {
                    m_error = wtfTF_INVALID_NAME_TYPE;
                    wxLogError(filename + wxT(": invalid data\nName must be a string"));
                }
                flag = false;
            }
        }
        if ((m_error == 0) && (!file->KeyExists(wxT("length"))))
        {
            m_error = wtfTF_NO_LENGTH;
            wxLogError(filename + wxT(": invalid data\nLength is not specifyed"));
        }
        if ((m_error == 0) && (file->GetItem(wxT("path"))->GetItemCount() == 0))
        {
            m_error = wtfTF_NO_NAME;
            wxLogError(filename + wxT(": invalid data\nNo name and no files"));
        }
        if (m_error == 0)
        {
            m_files->Add(file);
        } else {
            delete(file);
        }
    } else if (info_hash->GetItem(wxT("files"))->GetType() == wbenDATA_LIST) {
        cnt = info_hash->GetItem(wxT("files"))->GetItemCount();
        for (i = 0; i < cnt && m_error == 0; i++)
        {
            file = info_hash->GetItem(wxT("files"))->GetItem(i);
            if (file->GetType() != wbenDATA_DICTIONARY)
            {
                m_error = wtfTF_INVALID_FILES_ENTRY_TYPE;
                wxLogError(filename + wxT(": invalid data\nEach item in files section must be of type dictionary"));
                break;
            }
            node = file->GetValue().m_dict;
            flag = true;
            m_item = NULL;
            v.m_int = NULL;
            v.m_str = NULL;
            v.m_list = NULL;
            v.m_dict = NULL;
            for (it = node->begin(); it != node->end() && m_error == 0; ++it)
            {
                if (it->first == wxT("length"))
                {
                    if (it->second->GetType() != wbenDATA_STRING)
                    {
                        m_error = wtfTF_INVALID_LENGTH_TYPE;
                        wxLogError(filename + wxT(": invalid data\nLength must be an integer"));
                    }
                } else if (it->first == wxT("path")) {
                    if (it->second->GetType() != wbenDATA_LIST)
                    {
                        m_error = wtfTF_INVALID_PATH_TYPE;
                        wxLogError(filename + wxT(": invalid data\nPath for each file must be of type list"));
                    }
                    if (!flag)
                    {
                        (*node)[it->first]->Clear();
                    } else {
                        m_item = it->second;
                    }
                } else if ((it->first.Len() > 5) && (it->first.Left(5) == wxT("path."))) {
                    if (it->second->GetType() != wbenDATA_LIST)
                    {
                        m_error = wtfTF_INVALID_PATH_TYPE;
                        wxLogError(filename + wxT(": invalid data\nPath for each file must be of type list"));
                    }
                    if (flag)
                    {
                        m_item = WXDEBUG_NEW wbenData( *(it->second) );
                    }
                    (*node)[it->first]->Clear();
                    flag = false;
                } else {
                    (*node)[it->first]->Clear();
                }
            }
            if (m_error != 0)
            {
                break;
            }
            if (!flag)
            {
                file->RemoveKey(wxT("path"));
            } else if (!node->count(wxT("path"))) {
                m_error = wtfTF_NO_PATH;
                wxLogError(filename + wxT(": invalid data\nNo path specifyed for some file"));
                break;
            }
            (*node)[wxT("path")] = m_item;
            flag = true;
            while (flag)
            {
                flag = false;
                it = node->begin();
                while (!flag && it != node->end())
                {
                    if (it->second->GetType() == wbenDATA_UNDEFINED)
                    {
                        m_item = it->second;
                        node->erase(it);
                        delete(m_item);
                        flag = true;
                    } else {
                        ++it;
                    }
                }
            }
            cnt2 = (*node)[wxT("path")]->GetItemCount();
            for (j = 0; j < cnt2 && m_error == 0; j++)
            {
                if ((*node)[wxT("path")]->GetItem(j)->GetType() != wbenDATA_STRING)
                {
                    m_error = wtfTF_INVALID_PATH_ENTRY_TYPE;
                    wxLogError(filename + wxT(": invalid data\nEach item in path section must be a string"));
                }
            }
            if (m_error == 0)
            {
                m_files->Add(*file);
            }
        }
        if (m_error == 0)
        {
            info_hash->RemoveKey(wxT("files"));
        }
    } else {
        m_error = wtfTF_INVALID_FILES_TYPE;
        wxLogError(filename + wxT(": invalid data\nFiles section must be of type list"));
    }

    m_item = NULL;
    if (m_error != 0)
    {
        m_files->Clear();
        delete(m_data);
        m_data = NULL;
        return false;
    }
    return true;
}

int wtfTorrentFile::Error() const
{
    return m_error;
}

wxString wtfTorrentFile::GetPath()
{
    wxString res;
    size_t i;

    if (m_path == NULL || m_path->GetCount() == 0)
    {
        return res;
    }
    for (i = 0; i < m_path->GetCount(); i++)
    {
        switch (m_path->Item(i).GetType())
        {
            case wbenDATA_INTEGER:
                res.Printf(res + wxT("[%d]"), *(m_path->Item(i).GetValue().m_int));
                break;
            case wbenDATA_STRING:
                res += wxT("{") + *(m_path->Item(i).GetValue().m_str) + wxT("}");
                break;
            default:
                break;
        }
    }
    return res;
}

wbenData& wtfTorrentFile::GetData() const
{
    return *m_data;
}

void wtfTorrentFile::wtfCreateInteger(wbenContext *ctx, long value)
{
    wbenDataValue v;
    v.m_str =  WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, *wxConvCurrent, ctx->dsize);

    if ((ctx->type == wbenCtxDictKey) &&
        (m_path->GetCount() == 1) &&
        (m_path->Last().GetType() == wbenDATA_STRING) &&
        (m_path->Last().GetValue().m_str->CmpNoCase(wxT("codepage")) == 0))
    {
        m_encoding = wxT("cp") + *(v.m_str);
    }

    ctx->sig = wbenSigContinue;
    switch (ctx->type)
    {
        case wbenCtxRoot:
            m_error = wtfTF_INVALID_ROOT_TYPE;
            ctx->sig = wbenSigStop;
            break;
        case wbenCtxList:
            if (skip_key == 0)
            {
                m_item->AddItem(wbenDATA_STRING, v);
            }
            *(m_path->Last().GetValue().m_int) = *(m_path->Last().GetValue().m_int) + 1;
            break;
        case wbenCtxDictKey:
            if (skip_key == 0)
            {
                m_item->SetType(wbenDATA_STRING);
                m_item->SetValue(v);
            }
            wtfEndTag(ctx);
            break;
        default:
            break;
    }
}

int wtfTorrentFile::cmp_key(wbenContext *ctx, const size_t len, const char *name)
{
    size_t i, j;
    int res = 0;
    wxString msg;

    i = ctx->pos;
    j = 0;
    while (j < ctx->dsize && j < len && res == 0)
    {
        res = *(ctx->str + i) - *(name + j);
        j++;
        i++;
    }
    if (res == 0 && j < ctx->dsize)
    {
        res = 1;
    }
    if (res == 0 && j < len)
    {
        res = -1;
    }
    return res;
}

void wtfTorrentFile::wtfCreateString(wbenContext *ctx)
{
    wbenDataValue v;
    wbenData *item;
    wbenDataString *str = NULL;
    size_t enc;

    if (skip_key == 0 && ctx->type == wbenCtxDict)
    {
        if ((m_path->GetCount() == 1) &&
            (m_path->Last().GetType() == wbenDATA_STRING) &&
            (m_path->Last().GetValue().m_str->CmpNoCase(wxT("info")) == 0))
        {
            if (wtfTorrentFile::cmp_key(ctx, 6, "pieces") == 0)
            {
                skip_key = 1;
            }
        }
    }
/*
 * If the string is inside key value (e.g. is a key value) and
 * this key should not be processed, we don't need to create object
 * to store this string.
 *
 * If object should be created, wxMBConv instance should be provided
 * for proper encoding conversion.
 * The encoding of the string is determined by the following rules:
 *    - if the string is inside key value and this key has the form
 *        <name> '.' <encoding>, specifyed encoding is used;
 *    - otherwise, if m_encoding (specifyed in ctor or Load method)
 *        is multi-byte encoding, attempt to convert string using
 *        this encoding is made;
 *    - if previous failed, guessed encoding if any is used;
 *    - if conversion still not successful, m_encoding is used.
 */
    if (enc_key == 0 && ctx->type == wbenCtxDict)
    {
        if ((m_path->GetCount() == 1) &&
            (m_path->Last().GetType() == wbenDATA_STRING) &&
            (m_path->Last().GetValue().m_str->CmpNoCase(wxT("info")) == 0))
        {
            if (wtfTorrentFile::cmp_key(ctx, 5, "name.") == 0)
            {
                enc_key = 1;
                enc_key_i = 0;
            }
        }
        if ((m_path->GetCount() == 3) &&
            (m_path->Item(0).GetType() == wbenDATA_STRING) &&
            (m_path->Item(0).GetValue().m_str->CmpNoCase(wxT("info")) == 0) &&
            (m_path->Item(1).GetType() == wbenDATA_STRING) &&
            (m_path->Item(1).GetValue().m_str->CmpNoCase(wxT("files")) == 0) &&
            (m_path->Item(2).GetType() == wbenDATA_INTEGER))
        {
            if (wtfTorrentFile::cmp_key(ctx, 5, "path.") == 0)
            {
                enc_key = 1;
                enc_key_i = 2;
            }
        }
    }
    if (skip_key == 0 || ctx->type == wbenCtxDict)
    {
        if (ctx->type == wbenCtxDict)
        {
            str = WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, genc->GetConv(m_encoding), ctx->dsize);
        } else {
            if (enc_key > 0)
            {
                str = m_path->Item(enc_key_i).GetValue().m_str;
                if (str->Find(wxT(".")) == wxNOT_FOUND)
                {
                    str = NULL;
                } else {
                    v.m_str = WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, genc->GetConv(str->Mid(str->Find(wxT(".")) + 1)), ctx->dsize);
                    str = v.m_str;
                }
            } else if (genc->GetConvInfo(m_encoding).IsMultibyte()) {
                str = WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, genc->GetConv(m_encoding), ctx->dsize);
            } else {
                str = NULL;
            }
            if (str != NULL && ctx->dsize > 0 && str->Len() == 0)
            {
                delete(str);
                str = NULL;
            }
            if (str == NULL)
            {
                enc = genc->Guess(ctx->str + ctx->pos, ctx->dsize);
                if (enc < genc->GetCount())
                {
                    str = WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, wxCSConv(genc->GetEncodingName(enc)), ctx->dsize);//genc->GetConv(enc), ctx->dsize);
                } else {
                    str = WXDEBUG_NEW wbenDataString(ctx->str + ctx->pos, genc->GetConv(m_encoding), ctx->dsize);
                }
            }
        }
    }
    if ((ctx->type == wbenCtxDictKey) &&
        (m_path->GetCount() == 1) &&
        (m_path->Last().GetType() == wbenDATA_STRING))
    {
        v.m_str = m_path->Last().GetValue().m_str;
        if (v.m_str->CmpNoCase(wxT("encoding")) == 0)
        {
            m_encoding = *str;
        } else if (v.m_str->CmpNoCase(wxT("codepage")) == 0) {
            m_encoding = wxT("cp") + *str;
        }
    }
    v.m_str = str;

    ctx->sig = wbenSigContinue;
    switch (ctx->type)
    {
        case wbenCtxRoot:
            m_error = wtfTF_INVALID_ROOT_TYPE;
            ctx->sig = wbenSigStop;
            break;
        case wbenCtxList:
            if (skip_key == 0)
            {
                m_item->AddItem(wbenDATA_STRING, v);
            }
            *(m_path->Last().GetValue().m_int) = *(m_path->Last().GetValue().m_int) + 1;
            break;
        case wbenCtxDict:
            if (skip_key == 0)
            {
                item = m_item->AddKey(*str);
                item->SetParent(m_item);
                m_item = item;
            } else {
                if (skip_key > 1)
                {
                    skip_key++;
                }
            }
            if (enc_key > 1)
            {
                enc_key++;
            }
            item = WXDEBUG_NEW wbenData(wbenDATA_STRING);
            item->SetValue(v);
            m_path->Add(item);
            break;
        case wbenCtxDictKey:
            if (skip_key == 0)
            {
                m_item->SetType(wbenDATA_STRING);
                m_item->SetValue(v);
            }
            wtfEndTag(ctx);
            break;
        default:
            break;
    }
    return;
}

void wtfTorrentFile::wtfCreateList(wbenContext *ctx)
{
    wbenDataValue v;
    wbenData *item;
    v.m_list = WXDEBUG_NEW wbenDataList();

    switch (ctx->type)
    {
        case wbenCtxRoot:
            m_error = wtfTF_INVALID_ROOT_TYPE;
            ctx->sig = wbenSigStop;
            break;
        case wbenCtxList:
            if (skip_key == 0)
            {
                item = m_item->AddItem(wbenDATA_LIST, v);
                item->SetParent(m_item);
                m_item = item;
            } else {
                skip_key++;
            }
            if (enc_key > 0)
            {
                enc_key++;
            }
            item = WXDEBUG_NEW wbenData(wbenDATA_INTEGER);
            v.m_int = (wbenDataInteger *) malloc(sizeof(wbenDataInteger));
            *(v.m_int) = 0;
            item->SetValue(v);
            m_path->Add(item);
            break;
        case wbenCtxDictKey:
            if (skip_key == 0)
            {
                m_item->SetType(wbenDATA_LIST);
                m_item->SetValue(v);
            }
            item = WXDEBUG_NEW wbenData(wbenDATA_INTEGER);
            v.m_int = (wbenDataInteger *) malloc(sizeof(wbenDataInteger));
            *(v.m_int) = 0;
            item->SetValue(v);
            m_path->Add(item);
            break;
        default:
            break;
    }

    ctx->sig = wbenSigContinue;
}

void wtfTorrentFile::wtfCreateDictionary(wbenContext *ctx)
{
    wbenDataValue v;
    wbenData *item;
    v.m_dict = WXDEBUG_NEW wbenDataDictionary();

    ctx->sig = wbenSigContinue;
    switch (ctx->type)
    {
        case wbenCtxRoot:
            if (m_data->GetType() == wbenDATA_UNDEFINED)
            {
                m_data->SetType(wbenDATA_DICTIONARY);
                m_data->SetValue(v);
            } else {
                ctx->sig = wbenSigStop;
            }
            break;
        case wbenCtxList:
            if (skip_key == 0)
            {
                item = m_item->AddItem(wbenDATA_DICTIONARY, v);
                item->SetParent(m_item);
                m_item = item;
            } else {
                skip_key++;
            }
            if (enc_key > 0)
            {
                enc_key++;
            }
            break;
        case wbenCtxDictKey:
            if (skip_key == 0)
            {
                m_item->SetType(wbenDATA_DICTIONARY);
                m_item->SetValue(v);
            }
            break;
        default:
            break;
    }
}

void wtfTorrentFile::wtfEndTag(wbenContext *ctx)
{
    if (skip_key == 0)
    {
        m_item = m_item->GetParent();
    } else {
        skip_key--;
    }
    if (enc_key > 0)
    {
        enc_key--;
    }

    switch (ctx->type)
    {
        case wbenCtxList:
            m_path->RemoveAt(m_path->GetCount() - 1);
            if (ctx->path->GetCount())
            switch (ctx->path->Last())
            {
                case wbenCtxList:
                    *(m_path->Last().GetValue().m_int) = *(m_path->Last().GetValue().m_int) + 1;
                    break;
                case wbenCtxDict:
                    m_path->RemoveAt(m_path->GetCount() - 1);
                    break;
                default:
                    break;
            }
            break;
        case wbenCtxDict:
            if (ctx->path->GetCount())
            {
                switch (ctx->path->Last())
                {
                    case wbenCtxList:
                        *(m_path->Last().GetValue().m_int) = *(m_path->Last().GetValue().m_int) + 1;
                        break;
                    case wbenCtxDict:
                        m_path->RemoveAt(m_path->GetCount() - 1);
                        break;
                    default:
                        break;
                }
            }
            break;
        case wbenCtxDictKey:
            m_path->RemoveAt(m_path->GetCount() - 1);
            break;
        default:
            break;
    }
}

#if defined(WBEN_INCLUDE_REST)
WBEN_INCLUDE_REST()
#endif


