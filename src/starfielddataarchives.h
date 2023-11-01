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
  virtual void addArchive(MOBase::IProfile* profile, int index,
                          const QString& archiveName) override;
  virtual void removeArchive(MOBase::IProfile* profile,
                             const QString& archiveName) override;

protected:
  const QDir m_GamePath;

private:
  virtual void writeArchiveList(MOBase::IProfile* profile,
                                const QStringList& before) override;
};

#endif  // STARFIELDDATAARCHIVES_H
