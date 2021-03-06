#include "Precompile.h"
#include "Editor/Graph/XML.h"

#include "Editor/Graph/Debug.h"

namespace XML
{
	wxXmlNode *
	FindChild(const wxXmlNode& node, const wxString& name)
	{
		wxXmlNode *child = node.GetChildren();
		while (child != NULL)
		{
			if (child->GetName() == name)
			{
				return child;
			}
			child = child->GetNext();
		}
		return NULL;
	}

	void
	DeleteChild(wxXmlNode& node, const wxString& name)
	{
		wxXmlNode *child = FindChild(node, name);
		if (child != NULL)
		{
			node.RemoveChild(child);
			DESTROY(child);
		}
	}

	void
	DeleteChild(wxXmlNode& node, wxXmlNodeType type)
	{
		wxXmlNode *child = node.GetChildren();
		while (child != NULL)
		{
			if (child->GetType() == type)
			{
				node.RemoveChild(child);
				DESTROY(child);
				return;
			}
			child = child->GetNext();
		}
	}

	wxXmlAttribute *
	FindAttribute(const wxXmlNode& node, const wxString& name)
	{
        wxXmlAttribute *attrs = node.GetAttributes();
		while ( attrs != NULL )
		{
			if ( attrs->GetName() == name )
			{
				return attrs;
			}
			attrs = attrs->GetNext();
		}
		return NULL;
	}

	void
	DeleteAttribute(wxXmlNode& node, const wxString& name)
	{
		wxXmlAttribute *attr = node.GetAttributes();
		wxXmlAttribute *prev = NULL;
		while ( attr != NULL )
		{
			if ( attr->GetName() == name )
			{
				if ( prev == NULL )
				{
					node.SetAttributes( attr->GetNext() );
				}
				else
				{
					prev->SetNext( attr->GetNext() );
				}
				DESTROY( attr );
				return;
			}
			attr = attr->GetNext();
		}
	}

	wxString
	GetStringAttribute(const wxXmlNode& node, const wxString& name, const wxString& def)
	{
		wxString value;
		if ( !node.GetAttribute( name, &value ) )
		{
			if ( def == wxEmptyString )
			{
				THROW(TXT("Couldn't find attribute \"%s\" in element <%s>."), name.c_str(), node.GetName().c_str());
			}
			value = def;
		}
		return value;
	}

	bool
	GetBoolAttribute(const wxXmlNode& node, const wxString& name, const wxString& def)
	{
		wxString value = GetStringAttribute(node, name, def);
		if (value == wxT("true"))
		{
			return true;
		}
		else if (value == wxT("false"))
		{
			return false;
		}
		else
		{
			THROW(TXT("Attribute \"%s\" in element <%s> is not a boolean."), name.c_str(), node.GetName().c_str());
		}
	}

	int
	GetIntAttribute(const wxXmlNode& node, const wxString& name, const wxString& def)
	{
		wxString value = GetStringAttribute(node, name, def);
		long l;
		if (!value.ToLong(&l, 0))
		{
			THROW(TXT("Attribute \"%s\" in element <%s> is not an integer."), name.c_str(), node.GetName().c_str());
		}
		return l;
	}
};
