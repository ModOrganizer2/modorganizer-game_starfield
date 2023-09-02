#ifndef STARFIELDUNMANAGEDMODS_H
#define STARFIELDUNMANAGEDMODS_H


#include "gamebryounmanagedmods.h"
#include <gamegamebryo.h>


class StarfieldUnmangedMods : public GamebryoUnmangedMods {
public:
  StarfieldUnmangedMods(const GameGamebryo *game);
  ~StarfieldUnmangedMods();

  virtual QStringList mods(bool onlyOfficial) const override;
  virtual QStringList secondaryFiles(const QString &modName) const override;
  virtual QString displayName(const QString &modName) const override;
};



#endif // STARFIELDUNMANAGEDMODS_H
