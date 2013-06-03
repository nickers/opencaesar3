// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_overlays_menu.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_event.hpp"
#include "oc3_guienv.hpp"
#include <vector>

class OverlaysMenu::Impl
{
public:
  typedef std::vector< PushButton* > SubmenuButtons;
  SubmenuButtons buttons;

oc3_signals public:
  Signal1<int> onSelectOverlayTypeSignal;
};

OverlaysMenu::OverlaysMenu( Widget* parent, const Rect& rectangle, int id )
  : Widget( parent, id, rectangle ), _d( new Impl )
{ 
  _addButtons( OV_COUNT );
}

void OverlaysMenu::_addButtons( const OverlayType type )
{
  Point startPos;
  Point offset( 0, 27 );
  if( Widget* wdg = findChild( type ) )
  {
    startPos = Point( -1 * (int)getWidth() - 10, wdg->getTop() );
  }

  switch( type )
  {
  case OV_COUNT:
    _addButton( OV_NOTHING, "##ovrm_nothing##", startPos );
    _addButton( OV_WATER, "##ovrm_water##", startPos+=offset );
    _addButton( OV_RISK, "##ovrm_risk##", startPos+=offset );
    _addButton( OV_ENTERTAINMENT, "##ovrm_entertainment##", startPos+=offset );
    _addButton( OV_EDUCATION, "##ovrm_education##", startPos+=offset );
    _addButton( OV_HEALTH, "##ovrm_health##", startPos+=offset );
    _addButton( OV_COMMERCE, "##ovrm_commerce##", startPos+=offset );
    _addButton( OV_RELIGION, "##ovrm_religion##", startPos+=offset );
    setHeight( 8 * offset.getY() );
    break;

  case OV_RISK:
    _addButton( OV_RISK_FIRE, "##ovrm_fire##", startPos );
    _addButton( OV_RISK_DAMAGE, "##ovrm_damage##", startPos+=offset );
    _addButton( OV_RISK_CRIME, "##ovrm_crime##", startPos+=offset );
    _addButton( OV_RISK_ABORIGEN, "##ovrm_aborigen##", startPos+=offset );
    _addButton( OV_RISK_TROUBLES, "##ovrm_troubles##", startPos+=offset );
    break;

  case OV_ENTERTAINMENT:
    _addButton( OV_ENTERTAINMENT_ALL, "##ovrm_entr_all##", startPos );
    _addButton( OV_ENTERTAINMENT_THEATRES, "##ovrm_theatres##", startPos+=offset );
    _addButton( OV_ENTERTAINMENT_AMPHITHEATRES, "##ovrm_amtheatres##", startPos+=offset );
    _addButton( OV_ENTERTAINMENT_COLLISEUM, "##ovrm_colliseum##", startPos+=offset );
    _addButton( OV_ENTERTAINMENT_HIPPODROME, "##ovrm_hpdrome##", startPos+=offset );
    break;

  case OV_EDUCATION:
    _addButton( OV_EDUCATION_ALL, "##ovrm_edct_all##", startPos );
    _addButton( OV_EDUCATION_SCHOOL, "##ovrm_school##", startPos+=offset );
    _addButton( OV_EDUCATION_LIBRARY, "##ovrm_library##", startPos+=offset );
    _addButton( OV_EDUCATION_ACADEMY, "##ovrm_academy##", startPos+=offset );
    break;

  case OV_HEALTH:
    _addButton( OV_HEALTH_BARBER, "##ovrm_barber##", startPos );
    _addButton( OV_HEALTH_BATH, "##ovrm_bath##", startPos+=offset );
    _addButton( OV_HEALTH_CLINIC, "##ovrm_clinic##", startPos+=offset );
    _addButton( OV_HEALTH_HOSPITAL, "##ovrm_hospital##", startPos+=offset );
    break;

  case OV_COMMERCE:
    _addButton( OV_COMMERCE_TAX, "##ovrm_tax##", startPos );
    _addButton( OV_COMMERCE_FOOD, "##ovrm_food##", startPos+=offset );
    _addButton( OV_COMMERCE_PRESTIGE, "##ovrm_prestige##", startPos+=offset );
    break;
  }  
}

void OverlaysMenu::_addButton(const OverlayType ovType, const std::string& name, const Point& offset )
{
  PushButton* btn = new PushButton( this, Rect( 0, 0, getWidth(), 22 ) + offset, name, ovType );

  btn->setNotClipped( true );
  if( offset.getX() != 0 )
    _d->buttons.push_back( btn );
}

bool OverlaysMenu::isPointInside( const Point& point ) const
{
  Rect clickedRect = const_cast< OverlaysMenu* >( this )->getEnvironment()->getRootWidget()->getAbsoluteRect();
  return clickedRect.isPointInside( point );
}

bool OverlaysMenu::onEvent( const NEvent& event )
{
  if( event.EventType == OC3_GUI_EVENT )
  {
    switch( event.GuiEvent.EventType )
    {
    case OC3_ELEMENT_HOVERED:
      switch( event.GuiEvent.Caller->getID() )
      {
      case OV_RISK:
      case OV_ENTERTAINMENT:
      case OV_EDUCATION:
      case OV_HEALTH:
      case OV_COMMERCE:
        {
          for( Impl::SubmenuButtons::iterator it=_d->buttons.begin(); it != _d->buttons.end(); it++ )
              (*it)->deleteLater();
          _d->buttons.clear();

          _addButtons( (OverlayType)event.GuiEvent.Caller->getID() );
          return true;
        }
      break;

      case OV_NOTHING:
      case OV_WATER:
      case OV_RELIGION:
        {
          for( Impl::SubmenuButtons::iterator it=_d->buttons.begin(); it != _d->buttons.end(); it++ )
            (*it)->deleteLater();
          _d->buttons.clear();
          return true;
        }
      break;
      }
    break;

    case OC3_ELEMENT_LEFT:
    break;

    case OC3_BUTTON_CLICKED:
      {
        for( Impl::SubmenuButtons::iterator it=_d->buttons.begin(); it != _d->buttons.end(); it++ )
          (*it)->deleteLater();
        _d->buttons.clear();
                
        _d->onSelectOverlayTypeSignal.emit( event.GuiEvent.Caller->getID() );
        hide();
      }
    break;

    default:
    break;
    }
  } 

  if( event.EventType == OC3_MOUSE_EVENT && event.MouseEvent.Event == OC3_RMOUSE_LEFT_UP )
  {
    hide();
    return true;
  }

  return Widget::onEvent( event );
}

Signal1<int>& OverlaysMenu::onSelectOverlayType()
{
  return _d->onSelectOverlayTypeSignal;
}