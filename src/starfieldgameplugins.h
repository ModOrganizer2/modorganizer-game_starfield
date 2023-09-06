#ifndef _STARFIELDGAMEPLUGINS_H
#define _STARFIELDGAMEPLUGINS_H

#include <creationgameplugins.h>

#include <QObject>
#include <QtGlobal>

class StarfieldGamePlugins : public CreationGamePlugins
{

public:
  StarfieldGamePlugins(MOBase::IOrganizer* organizer);

protected:
  virtual bool overridePluginsAreSupported() override;
};

#endif  // _STARFIELDGAMEPLUGINS_H