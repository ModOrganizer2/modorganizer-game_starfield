#include "starfieldgameplugins.h"

using namespace MOBase;

StarfieldGamePlugins::StarfieldGamePlugins(MOBase::IOrganizer* organizer)
    : CreationGamePlugins(organizer)
{}

bool StarfieldGamePlugins::overridePluginsAreSupported()
{
  return true;
}
