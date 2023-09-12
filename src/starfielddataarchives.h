#ifndef STARFIELDDATAARCHIVES_H
#define STARFIELDDATAARCHIVES_H

#include "gamebryodataarchives.h"

namespace MOBase
{
class IProfile;
}

#include <QDir>
#include <QStringList>

class StarfieldDataArchives : public GamebryoDataArchives
{

public:
  StarfieldDataArchives(const QDir& myGamesDir, const QDir& gamePath);

public:
  virtual QStringList vanillaArchives() const override;
  virtual QStringList archives(const MOBase::IProfile* profile) const override;

protected:
  const QDir m_GamePath;

private:
  virtual void writeArchiveList(MOBase::IProfile* profile,
                                const QStringList& before) override;
};

#endif  // STARFIELDDATAARCHIVES_H
