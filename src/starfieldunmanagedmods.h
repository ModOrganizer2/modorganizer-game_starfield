#ifndef STARFIELDUNMANAGEDMODS_H
#define STARFIELDUNMANAGEDMODS_H

#include "gamebryounmanagedmods.h"
#include "gamestarfield.h"
#include <gamegamebryo.h>

class StarfieldUnmangedMods : public GamebryoUnmangedMods
{
public:
  StarfieldUnmangedMods(const GameStarfield* game, const QString appDataFolder);
  ~StarfieldUnmangedMods();

  virtual QStringList mods(bool onlyOfficial) const override;
  virtual QFileInfo referenceFile(const QString& modName) const override;
  virtual QStringList secondaryFiles(const QString& modName) const override;
  virtual QString displayName(const QString& modName) const override;

private:
  QString m_AppDataFolder;
};

#endif  // STARFIELDUNMANAGEDMODS_H
