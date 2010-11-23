#include "genc.h"

IMPLEMENT_ABSTRACT_CLASS(wtfEncConv, wxObject)

wtfEncConv::wtfEncConv(const wxString& encoding)
{
    wxString enc(encoding);
    m_conv = NULL;
    m_multibyte = false;
    m_delete = false;

    if ((encoding.Len() > 4) && (encoding.Left(4).CmpNoCase(wxT("UCS-")) == 0))
    {
        if (encoding.Mid(4, 1) == wxT("2"))
        {
            enc = wxT("UTF-16");
        } else if (encoding.Mid(4, 1) == wxT("4")) {
            enc = wxT("UTF-32");
        } else {
            enc = encoding.Left(5);
        }
        if (encoding.Len() > 5)
        {
            enc += encoding.Mid(5);
        }
    }
    if ((encoding.Len() > 3) && (encoding.Left(3).CmpNoCase(wxT("utf")) == 0) && (wxIsdigit(encoding[3])))
    {
        enc = wxT("UTF-") + encoding.Mid(3);
    }
    if ((enc.Len() > 4) && (enc.Left(4).CmpNoCase(wxT("UTF-")) == 0))
    {
        if (enc.Len() == 5)
        {
            if (enc.Right(1) == wxT("7"))
            {
                m_conv = (wxMBConv *) &wxConvUTF7;
                m_multibyte = true;
            } else if (enc.Right(1) == wxT("8")) {
                m_conv = (wxMBConv *) &wxConvUTF8;
                m_multibyte = true;
            }
        } else if (enc.Mid(4, 2) == wxT("16")) {
            if (enc.Len() == 8)
            {
                if (enc.Right(2).CmpNoCase(wxT("BE")) == 0)
                {
                    m_conv = (wxMBConv *) WXDEBUG_NEW wxMBConvUTF16BE();
                    m_multibyte = true;
                    m_delete = true;
                } else if (enc.Right(2).CmpNoCase(wxT("LE")) == 0) {
                    m_conv = (wxMBConv *) WXDEBUG_NEW wxMBConvUTF16LE();
                    m_multibyte = true;
                    m_delete = true;
                }
            } else if (enc.Len() == 6) {
                m_conv = (wxMBConv *) WXDEBUG_NEW wxMBConvUTF16();
                m_multibyte = true;
                m_delete = true;
            }
        } else if (enc.Mid(4, 2) == wxT("32")) {
            if (enc.Len() == 8)
            {
                if (enc.Right(2).CmpNoCase(wxT("BE")) == 0)
                {
                    m_conv = (wxMBConv *) WXDEBUG_NEW wxMBConvUTF32BE();
                    m_multibyte = true;
                    m_delete = true;
                } else if (enc.Right(2).CmpNoCase(wxT("LE")) == 0) {
                    m_conv = (wxMBConv *) WXDEBUG_NEW wxMBConvUTF32LE();
                    m_multibyte = true;
                    m_delete = true;
                }
            } else if (enc.Len() == 6) {
                m_conv = (wxMBConv *) WXDEBUG_NEW wxMBConvUTF32();
                m_multibyte = true;
                m_delete = true;
            }
        }
    }
    if (m_conv == NULL)
    {
        m_conv = (wxMBConv *) WXDEBUG_NEW wxCSConv(enc);
        m_delete = true;
    }
    enc.Clear();
}

wtfEncConv::~wtfEncConv()
{
    if (m_delete && m_conv != NULL)
    {
        delete(m_conv);
    }
    m_multibyte = false;
    m_delete = false;
    m_conv = NULL;
}

bool wtfEncConv::IsMultibyte()
{
    return m_multibyte;
}

wxMBConv& wtfEncConv::GetConv()
{
    return *m_conv;
}

IMPLEMENT_ABSTRACT_CLASS(wtfGEncoding, wxObject)

wtfGEncoding::wtfGEncoding()
{
#ifdef wtfDEBUG_GENC
    wxLogMessage(wxT("Creating wtfGEncoding instance"));
#endif // wtfDEBUG_GENC
    sample = NULL;
    m_count = 0;
    m_size = 0;
    m_error = 0;
    m_convCache = WXDEBUG_NEW wtfConvCache();
}

wtfGEncoding::wtfGEncoding(const wxString& filename, const wxString& encoding)
{
#ifdef wtfDEBUG_GENC
    wxLogMessage(wxT("Creating wtfGEncoding instance"));
#endif // wtfDEBUG_GENC
    sample = NULL;
    m_count = 0;
    m_size = 0;
    m_error = 0;
    m_convCache = WXDEBUG_NEW wtfConvCache();
    LoadData(filename, encoding);
}

wtfGEncoding::~wtfGEncoding()
{
#ifdef wtfDEBUG_GENC
    wxLogMessage(wxT("Deleting wtfGEncoding instance"));
#endif // wtfDEBUG_GENC
    size_t i;
    wtfConvCache::iterator it;
    if (sample != NULL)
    {
        for (i = 0; i < m_count; i++)
        {
            delete(sample[i].name);
            delete(sample[i].lang);
            delete(sample[i].p);
        }
        delete [] sample;
        sample = NULL;
    }
    for (it = m_convCache->begin(); it != m_convCache->end(); ++it)
    {
        delete(it->second);
        (*m_convCache)[it->first] = NULL;
    }
    m_convCache->clear();
    delete(m_convCache);
    m_convCache = NULL;
    m_count = 0;
    m_size = 0;
}

void wtfGEncoding::AddEncoding(const wxString& name, const wxString& lang, unsigned long* p)
{
    gencData *data;
    size_t i;
    if (m_size == m_count)
    {
        data = WXDEBUG_NEW gencData[m_count + wtfGEncodingDelta];
        for (i = 0; i < m_count; i++)
        {
            data[i] = sample[i];
            sample[i].name = NULL;
            sample[i].lang = NULL;
            sample[i].p = NULL;
        }
        delete [] sample;
        sample = data;
        m_size = m_count + wtfGEncodingDelta;
    }
    sample[m_count].name = WXDEBUG_NEW wxString(name);
    sample[m_count].lang = WXDEBUG_NEW wxString(lang);
    sample[m_count].p = p;
    sample[m_count].bcount = 0;
    for (i = 0; i < 256; i++)
    {
        sample[m_count].bcount += p[i];
    }
    m_count++;
}

bool wtfGEncoding::LoadData(const wxString& filename, const wxString& encoding)
{
    wxXmlDocument *xmldoc = WXDEBUG_NEW wxXmlDocument(filename, encoding);

    if ( !xmldoc->IsOk() )
    {
        m_error = 1;
        delete(xmldoc);
        return false;
    }

    if (( xmldoc->GetRoot()->GetName() != wxT("encodings-data") ))
    {
        m_error = 2;
        delete(xmldoc);
        wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": Invalid document type. The root element must be 'encodings-data'."));
        return false;
    }

    wxXmlNode *enc = xmldoc->GetRoot()->GetChildren();
    while (enc)
    {
        if ((enc->GetType() != wxXML_ELEMENT_NODE) || (enc->GetName() != wxT("encoding")))
        {
            enc = enc->GetNext();
            continue;
        }
        if (!CheckEncoding(enc, filename))
        {
            delete(xmldoc);
            return false;
        }
        enc = enc->GetNext();
    }

    delete(xmldoc);
    return true;
}

bool wtfGEncoding::CheckEncoding(wxXmlNode *enc, const wxString& filename)
{
    wxString lang, name;
    wxXmlNode *node = enc->GetChildren();
    wxXmlNode *chld;
    unsigned long *p = NULL;
    size_t i;

    while (node)
    {
        if (node->GetType() != wxXML_ELEMENT_NODE)
        {
            node = node->GetNext();
            continue;
        }
        if (node->GetName() == wxT("language"))
        {
            chld = node->GetChildren();
            if (chld->GetType() != wxXML_TEXT_NODE)
            {
                lang.Clear();
                name.Clear();
                m_error = 11;
                wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": 'language' element's content must be text only"));
                return false;
            }
            lang = chld->GetContent();
        } else if (node->GetName() == wxT("name")) {
            chld = node->GetChildren();
            if (chld->GetType() != wxXML_TEXT_NODE)
            {
                lang.Clear();
                name.Clear();
                m_error = 12;
                wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": 'name' element's content must be text only"));
                return false;
            }
            name = chld->GetContent();
        } else if (node->GetName() == wxT("data")) {
            p = WXDEBUG_NEW unsigned long[256];
            for (i = 0; i < 256; i++)
            {
                p[i] = 0;
            }
            if (!CheckEncData(node, p, filename))
            {
                lang.Clear();
                name.Clear();
                delete [] p;
                return false;
            }
        }
        node = node->GetNext();
    }
    if ((lang.Len() > 0) && (name.Len() > 0) && (p != NULL))
    {
        AddEncoding(name, lang, p);
    }

    lang.Clear();
    name.Clear();
    return true;
}

bool wtfGEncoding::CheckEncData(wxXmlNode *data, unsigned long *p, const wxString& filename)
{
    wxXmlNode *node = data->GetChildren();
    wxString value;
    unsigned long val, *cur;
    wxXmlNode *chld;

    while (node)
    {
        if ((node->GetType() != wxXML_ELEMENT_NODE) || (node->GetName() != wxT("byte")))
        {
            node = node->GetNext();
            continue;
        }
        if (!node->GetPropVal(wxT("id"), &value))
        {
            m_error = 33;
            wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": Attribute 'id' having positive numeric value must be specified for each 'byte' element."));
        } else if (!value.ToULong(&val)) {
            m_error = 34;
            wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": Attribute 'id' having positive numeric value must be specified for each 'byte' element."));
        } else if (val > 255) {
            m_error = 35;
            wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": Attribute 'id' having positive numeric value must be specified for each 'byte' element."));
        }
        if (m_error != 0)
        {
            value.Clear();
            return false;
        }
        cur = p + val;
        chld = node->GetChildren();
        if (chld->GetType() != wxXML_TEXT_NODE)
        {
            value.Clear();
            m_error = 21;
            wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": 'byte' element's content must be text only."));
            return false;
        }
        value = chld->GetContent();
        if ((value.Len() == 0) || (!value.ToULong(&val)))
        {
            value.Clear();
            m_error = 22;
            wxLogError(wxT("wtfGEncoding: ") + filename + wxT(": 'byte' element's content must be non-negative number."));
            return false;
        }
        *cur = val;
        node = node->GetNext();
    }

    value.Clear();
    return true;
}

int wtfGEncoding::error() const
{
    return m_error;
}

size_t wtfGEncoding::GetCount()
{
    return m_count;
}

wxString& wtfGEncoding::GetLanguage(size_t i)
{
    return(*(sample[i].lang));
}

wxString& wtfGEncoding::GetEncodingName(size_t i)
{
    return(*(sample[i].name));
}

wxMBConv& wtfGEncoding::GetConv(size_t i)
{
    if (m_convCache->count(*(sample[i].name)) == 0)
    {
        (*m_convCache)[*(sample[i].name)] = WXDEBUG_NEW wtfEncConv(*(sample[i].name));
    }

    return ((*m_convCache)[*(sample[i].name)]->GetConv());
}

wxMBConv& wtfGEncoding::GetConv(const wxString& encoding)
{
    if (m_convCache->count(encoding) == 0)
    {
        (*m_convCache)[encoding] = WXDEBUG_NEW wtfEncConv(encoding);
    }

    return ((*m_convCache)[encoding]->GetConv());
}

wtfEncConv& wtfGEncoding::GetConvInfo(size_t i)
{
    if (m_convCache->count(*(sample[i].name)) == 0)
    {
        (*m_convCache)[*(sample[i].name)] = WXDEBUG_NEW wtfEncConv(*(sample[i].name));
    }

    return (*( (*m_convCache)[*(sample[i].name)] ));
}

wtfEncConv& wtfGEncoding::GetConvInfo(const wxString& encoding)
{
    if (m_convCache->count(encoding) == 0)
    {
        (*m_convCache)[encoding] = WXDEBUG_NEW wtfEncConv(encoding);
    }

    return (*( (*m_convCache)[encoding] ));
}

size_t wtfGEncoding::Guess(const char *seq, const size_t len)
{
    size_t res = (size_t) -1;
    size_t i, j, l;
    gencData data;
    double min_relen, relen;//, p, q;
    bool inf;

    data.p = WXDEBUG_NEW unsigned long[256];
    data.bcount = 0;
    for (i = 0; i < 256; i++)
    {
        data.p[i] = 0;
    }

    i = 0;
    l = 0;
    while (((len != 0) || *(seq + i)) && (l < len))
    {
        data.p[(unsigned char) *(seq + i)]++;
        data.bcount++;
        i++;
        l++;
    }

    if (data.bcount == 0)
    {
        delete [] data.p;
        return res;
    }

    min_relen = 0;
#ifdef wtfDEBUG_GENC
    wxLogMessage(wxT("----------"));
    wxLogMessage(wxT("Sequence: '%s'"), seq);
#endif // wtfDEBUG_GENC
    for (i = 0; i < m_count; i++)
    {
        relen = 0;
        inf = false;
        for (j = 0; j < 256; j++)
        {
#ifdef wtfDEBUG_GENC_CALC
            if (i == 0 && data.p[j] > 0)
            {
                wxLogMessage(wxT("%d: seq(%d / %d); sample(%d / %d)"), j, data.p[j], data.bcount, sample[i].p[j], sample[i].bcount);
            }
#endif // wtfDEBUG_GENC_CALC
            if (!inf && data.p[j] > 0)
            {
                if (sample[i].p[j] > 0)
                {
#ifdef wtfDEBUG_GENC_CALC
                    wxLogMessage(wxT("Summand %d = %e"), j, data.p[j] * log(data.p[j] * sample[i].bcount * 1.0 / (sample[i].p[j] * data.bcount)) / (data.bcount * log(2)));
#endif // wtfDEBUG_GENC_CALC
                    relen += data.p[j] * log(data.p[j] * sample[i].bcount * 1.0 / (sample[i].p[j] * data.bcount)) / (data.bcount * log(2));
                } else {
                    inf = true;
#ifdef wtfDEBUG_GENC
                    wxLogMessage(wxT("No byte %d in '%s.%s' sample"), j, *(sample[i].lang), *(sample[i].name));
#endif // wtfDEBUG_GENC
                }
            }
        }
#ifdef wtfDEBUG_GENC
        wxLogMessage(wxT("-----"));
        if (inf)
        {
            wxLogMessage(wxT("Relative enthropy for encoding '%s.%s' is infinity"), *(sample[i].lang), *(sample[i].name));
        } else {
            wxLogMessage(wxT("Relative enthropy for encoding '%s.%s' is %g"), *(sample[i].lang), *(sample[i].name), relen);
        }
#endif // wtfDEBUG_GENC
        if (!inf && (min_relen <= 0 || relen < min_relen))
        {
            min_relen = relen;
            res = i;
        }
    }
#ifdef wtfDEBUG_GENC
    if (res < m_count)
    {
        wxLogMessage(wxT("Encoding: '") + *(sample[res].name) + wxT("'"));
    } else {
        wxLogMessage(wxT("Encoding: unknown"));
    }
#endif // wtfDEBUG_GENC

    delete [] data.p;

    return res;
}

