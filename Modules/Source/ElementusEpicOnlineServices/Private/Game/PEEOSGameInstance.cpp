// Author: Lucas Vilas-Boas
// Year: 2023
// Repo: https://github.com/lucoiso/UEProject_Elementus

#include "Game/PEEOSGameInstance.h"
#include "Management/PEEOSLibrary.h"
#include "LogElementusEpicOnlineServices.h"
#include <JsonObjectConverter.h>
#include <Kismet/GameplayStatics.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PEEOSGameInstance)

UPEEOSGameInstance::UPEEOSGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPEEOSGameInstance::Init()
{
	Super::Init();
}

void UPEEOSGameInstance::InitializeVoiceChatFramework()
{
	if (!FEOSVoiceChat::Get()->IsInitialized())
	{
		OnVoiceChatInitializeCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatInitialized);
		FEOSVoiceChat::Get()->Initialize(OnVoiceChatInitializeCompleteDelegate);
	}
}

void UPEEOSGameInstance::ShutdownVoiceChatFramework()
{
	if (FEOSVoiceChat::Get()->IsInitialized())
	{
		DisconnectVoiceChatFramework();

		FOnlineSubsystemEOS* const OnlineSubsystemEOS = UPEEOSLibrary::GetOnlineSubsystemEOS();
		
		for (int32 Iterator = 0; Iterator < UGameplayStatics::GetNumLocalPlayerControllers(GetWorld()); ++Iterator)
		{
			if (const FUniqueNetIdPtr NetId = UPEEOSLibrary::GetUniqueNetId(Iterator, OnlineSubsystemEOS))
			{
				OnlineSubsystemEOS->ReleaseVoiceChatUserInterface(*NetId.Get());
			}
		}

		OnVoiceChatUninitializeCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatUninitialized);
		FEOSVoiceChat::Get()->Uninitialize(OnVoiceChatUninitializeCompleteDelegate);
	}
}

void UPEEOSGameInstance::ConnectVoiceChatFramework()
{
	if (!FEOSVoiceChat::Get()->IsConnected() && !FEOSVoiceChat::Get()->IsConnecting())
	{
		OnVoiceChatConnectCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatConnected);
		FEOSVoiceChat::Get()->Connect(OnVoiceChatConnectCompleteDelegate);
	}
}

void UPEEOSGameInstance::DisconnectVoiceChatFramework()
{
	if (FEOSVoiceChat::Get()->IsConnected())
	{
		OnVoiceChatDisconnectCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatDisconnected);
		FEOSVoiceChat::Get()->Disconnect(OnVoiceChatDisconnectCompleteDelegate);
	}
}

void UPEEOSGameInstance::OnVoiceChatInitialized(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess())

		ConnectVoiceChatFramework();
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatInitializeCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::OnVoiceChatUninitialized(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatUninitializeCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::OnVoiceChatConnected(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess());
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatConnectCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::OnVoiceChatDisconnected(const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d"), *FString(__func__), Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatDisconnectCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::LoginToVoiceChatFramework(const int32 LocalUserNum)
{
	if (const IOnlineIdentityPtr IdentityInterface = UPEEOSLibrary::GetIdentityInterface();
		IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
	{
		if (FEOSVoiceChatUser* const VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
		{
			OnVoiceChatLoginCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatLogin);

			VoiceChatUserPtr->Login(IdentityInterface->GetPlatformUserIdFromLocalUserNum(LocalUserNum), IdentityInterface->GetPlayerNickname(LocalUserNum), IdentityInterface->GetAuthToken(LocalUserNum), OnVoiceChatLoginCompleteDelegate);
		}
	}
}

void UPEEOSGameInstance::LogoutFromVoiceChatFramework(const int32 LocalUserNum)
{
	FOnlineSubsystemEOS* const OnlineSubsystemEOS = UPEEOSLibrary::GetOnlineSubsystemEOS();

	if (FEOSVoiceChatUser* const VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
	{
		OnVoiceChatLogoutCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatLogout);
		VoiceChatUserPtr->Logout(OnVoiceChatLogoutCompleteDelegate);

		if (const FUniqueNetIdPtr NetId = UPEEOSLibrary::GetUniqueNetId(LocalUserNum, OnlineSubsystemEOS))
		{
			OnlineSubsystemEOS->ReleaseVoiceChatUserInterface(*NetId.Get());
		}
	}
}

void UPEEOSGameInstance::OnVoiceChatLogin(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Player Name: %s; Result: %d"), *FString(__func__), *PlayerName, Result.IsSuccess())

		ConnectVoiceChatFramework();
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Player Name: %s; Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), Result.IsSuccess(), *PlayerName, *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatLoginCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::OnVoiceChatLogout(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Player Name: %s; Result: %d"), *FString(__func__), *PlayerName, Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Player Name: %s; Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), Result.IsSuccess(), *PlayerName, *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatLogoutCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::ConnectVoiceChatToSessionChannel(const int32 LocalUserNum, const FString& ChannelName, const FEOSVoiceChatChannelCredentials Credentials, const EEOSVoiceChatChannelType ChannelType)
{
	UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Local User Num: %d; Channel Name: %s"), *FString(__func__), LocalUserNum, *ChannelName);

	if (FEOSVoiceChatUser* const VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
	{
		OnVoiceChatChannelJoinCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatChannelJoined);

		FString CredentialsJson;
		FJsonObjectConverter::UStructToJsonObjectString(Credentials, CredentialsJson);

		VoiceChatUserPtr->JoinChannel(ChannelName, CredentialsJson, static_cast<EVoiceChatChannelType>(ChannelType), OnVoiceChatChannelJoinCompleteDelegate);
	}
}

void UPEEOSGameInstance::LeaveVoiceChatSessionChannel(const int32 LocalUserNum, const FString& ChannelName)
{
	UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Local User Num: %d; Channel Name: %s"), *FString(__func__), LocalUserNum, *ChannelName);

	if (FEOSVoiceChatUser* const VoiceChatUserPtr = UPEEOSLibrary::GetEOSVoiceChatUser(LocalUserNum))
	{
		OnVoiceChatChannelLeaveCompleteDelegate.BindUObject(this, &UPEEOSGameInstance::OnVoiceChatChannelLeft);
		VoiceChatUserPtr->LeaveChannel(ChannelName, OnVoiceChatChannelLeaveCompleteDelegate);
	}
}

void UPEEOSGameInstance::OnVoiceChatChannelJoined(const FString& ChannelName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Channel Name: %s; Result: %d"), *FString(__func__), *ChannelName, Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Channel Name: %s; Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), *ChannelName, Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatChannelJoinCompleteDelegate.Unbind();
}

void UPEEOSGameInstance::OnVoiceChatChannelLeft(const FString& ChannelName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Channel Name: %s; Result: %d"), *FString(__func__), *ChannelName, Result.IsSuccess())
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Log, TEXT("%s - Channel Name: %s; Result: %d - Error Code: %s; Error Description: %s"), *FString(__func__), *ChannelName, Result.IsSuccess(), *Result.ErrorCode, *Result.ErrorDesc);
	}

	OnVoiceChatChannelLeaveCompleteDelegate.Unbind();
}

bool UPEEOSGameInstance::CreateEOSSession(const FEOSSessionSettings SessionSettings)
{
	return EOS_CreateSession(0, SessionSettings.GetNativeEOSSettings());
}

bool UPEEOSGameInstance::FindEOSSessions(const int32 LocalUserNum, const bool bIsLANQuery, const int32 MaxResults)
{
	return EOS_FindSessions(LocalUserNum, bIsLANQuery, MaxResults);
}

bool UPEEOSGameInstance::CancelFindEOSSessions()
{
	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->OnCancelFindSessionsCompleteDelegates.AddUObject(this, &UPEEOSGameInstance::OnCancelFindSessions);

		return SessionInterface->CancelFindSessions();
	}

	return false;
}

bool UPEEOSGameInstance::JoinEOSSession(const int32 LocalUserNum, const FSessionDataHandler SessionData)
{
	return EOS_JoinSession(LocalUserNum, SessionData.Result);
}

bool UPEEOSGameInstance::DestroyEOSSession()
{
	return EOS_DestroySession();
}

void UPEEOSGameInstance::ServerTravelToLevel(const FName LevelName) const
{
	GetWorld()->ServerTravel(LevelName.ToString() + "?listen");
}

void UPEEOSGameInstance::ClientTravelToSessionLevel(const int32 LocalUserNum) const
{
	if (APlayerController* const PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), LocalUserNum);
		!EOSCurrentSessionInfo.IsEmpty())
	{
		PlayerController->ClientTravel(EOSCurrentSessionInfo, TRAVEL_Absolute);
	}
}

TArray<FSessionDataHandler> UPEEOSGameInstance::GetSessionsDataHandles() const
{
	TArray<FSessionDataHandler> SessionDataHandle_Arr;
	for (const FOnlineSessionSearchResult& SearchResult : EOSSearchSettings->SearchResults)
	{
		SessionDataHandle_Arr.Add(FSessionDataHandler{SearchResult});
	}

	return SessionDataHandle_Arr;
}

bool UPEEOSGameInstance::EOS_CreateSession(const uint8 HostingPlayerNum, const FOnlineSessionSettings& NewSessionSettings)
{
	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPEEOSGameInstance::OnSessionCreated);

		return SessionInterface->CreateSession(HostingPlayerNum, NAME_GameSession, NewSessionSettings);
	}

	return false;
}

bool UPEEOSGameInstance::EOS_FindSessions(const uint8 SearchingPlayerNum, const bool bIsLANQuery, const uint8 MaxResults)
{
	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		EOSSearchSettings = MakeShareable(new FOnlineSessionSearch());
		EOSSearchSettings->bIsLanQuery = bIsLANQuery;
		EOSSearchSettings->MaxSearchResults = MaxResults;
		EOSSearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("ProjectElementus"), EOnlineComparisonOp::Equals);
		EOSSearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPEEOSGameInstance::OnSessionsFound);

		return SessionInterface->FindSessions(SearchingPlayerNum, EOSSearchSettings.ToSharedRef());
	}

	return false;
}

bool UPEEOSGameInstance::EOS_JoinSession(const uint8 LocalUserNum, const FOnlineSessionSearchResult& DesiredSession)
{
	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPEEOSGameInstance::OnSessionJoined);

		return SessionInterface->JoinSession(LocalUserNum, NAME_GameSession, DesiredSession);
	}

	return false;
}

bool UPEEOSGameInstance::EOS_DestroySession()
{
	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPEEOSGameInstance::OnSessionDestroyed);

		return SessionInterface->DestroySession(NAME_GameSession, FOnDestroySessionCompleteDelegate());
	}

	return false;
}

void UPEEOSGameInstance::OnSessionCreated(const FName SessionName, const bool bResult)
{
	UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(), bResult);

	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->GetResolvedConnectString(SessionName, EOSCurrentSessionInfo);
		SessionInterface->ClearOnCreateSessionCompleteDelegates(this);

		EnableListenServer(true);
	}

	CreateSessionDelegate.Broadcast();
}

void UPEEOSGameInstance::OnSessionsFound(const bool bResult)
{
	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
	}

	if (EOSSearchSettings.IsValid())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - Result: %d - Sessions found: %d"), *FString(__func__), bResult, EOSSearchSettings->SearchResults.Num());

		TArray<FSessionDataHandler> SessionDataHandle_Arr;

		for (const FOnlineSessionSearchResult& SearchResult : EOSSearchSettings->SearchResults)
		{
			const FSessionDataHandler SessionDataHandle{SearchResult};

			UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("Session Found: %s - %s"), *SearchResult.GetSessionIdStr(), *SearchResult.Session.OwningUserName);

			SessionDataHandle_Arr.Add(SessionDataHandle);
		}

		FindSessionsDelegate.Broadcast(SessionDataHandle_Arr);
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - Result: %d - No sessions found"), *FString(__func__), bResult);
	}
}

void UPEEOSGameInstance::OnCancelFindSessions(const bool bResult)
{
	UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - Result: %d"), *FString(__func__), bResult);

	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->ClearOnCancelFindSessionsCompleteDelegates(this);
	}

	CancelFindSessionsDelegate.Broadcast();

	if (bResult)
	{
		EOSSearchSettings.Reset();
	}
}

void UPEEOSGameInstance::OnSessionJoined(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(), Result);

	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->GetResolvedConnectString(SessionName, EOSCurrentSessionInfo);
		SessionInterface->ClearOnJoinSessionCompleteDelegates(this);

		EnableListenServer(false);
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		JoinSessionDelegate.Broadcast();
	}
}

void UPEEOSGameInstance::OnSessionDestroyed(const FName SessionName, const bool bResult)
{
	UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - Session: %s; Result: %d"), *FString(__func__), *SessionName.ToString(), bResult);

	if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
	}

	DestroySessionDelegate.Broadcast();
}

void UPEEOSGameInstance::OnSessionInviteAccepted(const bool bWasSuccessful, const int32 LocalUserNum, const FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("User %s accepted invite to session %s"), *UserId->ToString(), *InviteResult.GetSessionIdStr());

		EOS_JoinSession(LocalUserNum, InviteResult);
	}
	else
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("User %s failed to accept invite to session %s"), *UserId->ToString(), *InviteResult.GetSessionIdStr());
	}
}

bool UPEEOSGameInstance::EOSLogin(const int32 LocalUserNum, const FString& Token, const int32 Port, const bool bUsePortal)
{
	const FOnlineAccountCredentials Credentials = bUsePortal ? FOnlineAccountCredentials("AccountPortal", FString(), FString())
															 : FOnlineAccountCredentials("Developer", "localhost:" + FString::FromInt(Port), Token);

	return EOS_Login(LocalUserNum, Credentials);
}

bool UPEEOSGameInstance::EOSLogout(const int32 LocalUserNum)
{
	return EOS_Logout(LocalUserNum);
}

bool UPEEOSGameInstance::EOS_Login(const uint8 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	if (const IOnlineIdentityPtr IdentityInterface = UPEEOSLibrary::GetIdentityInterface();
		IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::NotLoggedIn)
	{
		IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UPEEOSGameInstance::OnLoginComplete);

		return IdentityInterface->Login(LocalUserNum, AccountCredentials);
	}

	return false;
}

bool UPEEOSGameInstance::EOS_Logout(const uint8 LocalUserNum)
{
	if (const IOnlineIdentityPtr IdentityInterface = UPEEOSLibrary::GetIdentityInterface();
		IdentityInterface->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
	{
		IdentityInterface->OnLogoutCompleteDelegates->AddUObject(this, &UPEEOSGameInstance::OnLogoutComplete);

		return IdentityInterface->Logout(LocalUserNum);
	}

	return false;
}

void UPEEOSGameInstance::OnLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	IOnlineSubsystem* const OnlineSubsystem = FOnlineSubsystemEOS::Get(EOS_SUBSYSTEM);
	
	if (const IOnlineIdentityPtr IdentityInterface = UPEEOSLibrary::GetIdentityInterface(OnlineSubsystem))
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__), LocalUserNum, bWasSuccessful);

		IdentityInterface->ClearOnLoginCompleteDelegates(LocalUserNum, this);

		if (const IOnlineSessionPtr SessionInterface = UPEEOSLibrary::GetSessionInterface(OnlineSubsystem))
		{
			OnSessionUserInviteAcceptedDelegate.BindUObject(this, &UPEEOSGameInstance::OnSessionInviteAccepted);
			SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate);
		}
	}

	UserLoginDelegate.Broadcast();
}

void UPEEOSGameInstance::OnLogoutComplete(const int32 LocalUserNum, const bool bWasSuccessful)
{
	if (const IOnlineIdentityPtr IdentityInterface = UPEEOSLibrary::GetIdentityInterface())
	{
		UE_LOG(LogElementusEpicOnlineServices_Internal, Warning, TEXT("%s - LocalUserNum: %d; bWasSuccessful: %d"), *FString(__func__), LocalUserNum, bWasSuccessful);

		IdentityInterface->ClearOnLogoutCompleteDelegates(LocalUserNum, this);

		OnSessionUserInviteAcceptedDelegate.Unbind();
	}

	UserLogoutDelegate.Broadcast();
}
