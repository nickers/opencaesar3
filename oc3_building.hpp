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
//
// Copyright 2012-2013 Gregoire Athanase, gathanase@gmail.com


#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <string>
#include <map>
#include <list>
#include <set>

#include "oc3_tilemap.hpp"
#include "oc3_enums.hpp"
#include "oc3_good.hpp"
#include "oc3_scopedptr.hpp"
#include "oc3_animation.hpp"
#include "oc3_referencecounted.hpp"
#include "oc3_predefinitions.hpp"

class Widget;
class GuiInfoBox;

class LandOverlay : public Serializable, public ReferenceCounted
{
public:
  LandOverlay( const BuildingType type, const Size& size=Size(1));
  virtual ~LandOverlay();

  Tile& getTile() const;  // master tile, in case of multi-tile area
  TilePos getTilePos() const;
  Size getSize() const;  // size in tiles (1=1x1, 2=2x2, ...)
  void setSize( const Size& size );
  
  bool isDeleted() const;  // returns true if the overlay should be forgotten
  void deleteLater();
  
  virtual bool isWalkable() const;
  virtual void setTerrain( TerrainTile& terrain ) = 0;
  virtual void build( const TilePos& pos );
  virtual void destroy();  // handles the delete
  virtual Point getOffset( const Point& subpos ) const;
  virtual void timeStep(const unsigned long time);  // perform one simulation step

  // graphic
  void setPicture(Picture &picture);
  Picture& getPicture();
  std::vector<Picture*>& getForegroundPictures();

  std::string getName();  // landoverlay debug name
  void setName( const std::string& name );

  BuildingType getType() const;
  BuildingClass getClass() const;
  void setType(const BuildingType buildingType);

  virtual void save( VariantMap& stream) const;
  virtual void load( const VariantMap& stream );

protected:
  std::vector<Picture*> _fgPictures;

  Animation& _getAnimation();

  class Impl;
  ScopedPtr< Impl > _d;
};

class Construction : public LandOverlay
{
public:
  Construction( const BuildingType type, const Size& size );

  virtual bool canBuild(const TilePos& pos ) const;  // returns true if it can be built there
  virtual void build( const TilePos& pos );
  virtual void burn();
  virtual void collapse();
  virtual bool isNeedRoadAccess() const;
  virtual const PtrTilesList& getAccessRoads() const;  // return all road tiles adjacent to the construction
  virtual void computeAccessRoads();  
  virtual int  getMaxDistance2Road() const; // virtual because HOUSE has different behavior
  virtual char getDesirabilityInfluence() const;
  virtual unsigned char getDesirabilityRange() const;
  virtual char getDesirabilityStep() const;
  virtual void destroy();

protected:
  PtrTilesList _accessRoads;
  
  typedef enum { duPositive=true, duNegative=false } DsbrlUpdate;
  void _updateDesirabilityInfluence( const DsbrlUpdate type );
};

class ServiceWalker;
class Building : public Construction
{
public:
   Building(const BuildingType type, const Size& size=Size(1) );
   virtual void setTerrain(TerrainTile &terrain);

   virtual void timeStep(const unsigned long time);
   virtual void storeGoods(GoodStock &stock, const int amount = -1);
   // evaluate the given service
   virtual float evaluateService(ServiceWalkerPtr walker);
   // handle service reservation
   void reserveService(const ServiceType service);
   void cancelService(const ServiceType service);
   virtual void applyService( ServiceWalkerPtr walker);
   // evaluate the need for the given trainee
   virtual float evaluateTrainee(const WalkerTraineeType traineeType);  // returns >0 if trainee is needed
   void reserveTrainee(const WalkerTraineeType traineeType); // trainee will come
   void cancelTrainee(const WalkerTraineeType traineeType);  // trainee will not come
   void applyTrainee(const WalkerTraineeType traineeType); // trainee arrives

   float getDamageLevel();
   void  setDamageLevel(const float value);
   float getFireLevel();
   void  setFireLevel(const float value);

   void save( VariantMap& stream) const;
   void load( const VariantMap& stream);

protected:
   float _damageLevel;  // >100 => building is destroyed
   float _fireLevel;    // >100 => building catch fire
   float _damageIncrement;
   float _fireIncrement;
   std::set<ServiceType> _reservedServices;  // a serviceWalker is on the way
   std::map<WalkerTraineeType, int> _traineeMap;  // current level of trainees working in the building (0..200)
   std::set<WalkerTraineeType> _reservedTrainees;  // a trainee is on the way
};

//operator need for std::reset
inline bool operator<(BuildingPtr v1, BuildingPtr v2)
{
  return v1.object() < v2.object();
}

class SmallStatue : public Building
{
public:
  SmallStatue();
  bool isNeedRoadAccess() const;
};

class MediumStatue : public Building
{
public:
  MediumStatue();
  bool isNeedRoadAccess() const;
};

class BigStatue : public Building
{
public:
  BigStatue();
  bool isNeedRoadAccess() const;
};

class Shipyard : public Building
{
public:
  Shipyard();
};

class TriumphalArch : public Building
{
public:
  TriumphalArch();
};

class Dock : public Building
{
public:
  Dock();
  void timeStep(const unsigned long time);
};

#endif
