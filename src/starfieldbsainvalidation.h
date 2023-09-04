#ifndef STARFIELDBSAINVALIDATION_H
#define STARFIELDBSAINVALIDATION_H


#include <bsainvalidation.h>
#include <dataarchives.h>
#include "gamebryobsainvalidation.h"
#include "starfielddataarchives.h"

#include <memory>

namespace MOBase {
  class IPluginGame;
}

class StarfieldBSAInvalidation : public GamebryoBSAInvalidation
{
public:

  StarfieldBSAInvalidation(DataArchives *dataArchives, MOBase::IPluginGame const *game);
  virtual bool isInvalidationBSA(const QString& bsaName) override;
  virtual bool prepareProfile(MOBase::IProfile* profile) override;

private:

  virtual QString invalidationBSAName() const override;
  virtual unsigned long bsaVersion() const override;

private:

  QString m_IniFileName;
  MOBase::IPluginGame const* m_Game;

};

#endif // STARFIELDBSAINVALIDATION_H
