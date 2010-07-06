#pragma once

#include "Luna/API.h"
#include "Application/Inspect/Widgets/Canvas.h"

namespace Luna
{
  class PropertiesManager;

  class LSelectionPropertiesPanel : public wxPanel 
  {
  public:
    wxRadioButton* m_Intersection;
    wxRadioButton* m_Union;

    wxButton* m_ExpandAll;
    wxButton* m_CollapseAll;

    PropertiesManager* m_PropertyManager;
    Inspect::CanvasWindow* m_PropertyCanvas;

    LSelectionPropertiesPanel( PropertiesManager* propertyManager, wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 243,335 ), int style = wxTAB_TRAVERSAL );

    void OnIntersection(wxCommandEvent& event);
    void OnUnion(wxCommandEvent& event);
    void OnExpandAll(wxCommandEvent& event);
    void OnCollapseAll(wxCommandEvent& event);
  };
}