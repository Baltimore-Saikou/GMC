// Copyright 2022 Dominik Scherer. All Rights Reserved.

#include "PauseMenu.h"
#include "SteamGameInstance.h"
#include "GMC_LOG.h"

bool UPauseMenu::Initialize()
{
  if (Super::Initialize())
  {
    if (ExitButton) ExitButton->OnClicked.AddDynamic(this, &UPauseMenu::OnExitButtonClicked);
    return true;
  }
  return false;
}

void UPauseMenu::OnExitButtonClicked()
{
  if (USteamGameInstance* SteamGameInstance = Cast<USteamGameInstance>(GetGameInstance()))
  {
    IOnlineSessionPtr SessionInterface = SteamGameInstance->SessionInterface;
    if (SessionInterface.IsValid())
    {
      FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SteamGameInstance->SessionLabel);
      if (ExistingSession)
      {
        SessionInterface->DestroySession(ExistingSession->SessionName);
      }
    }
    SteamGameInstance->ReturnToMainMenu();
  }
}
