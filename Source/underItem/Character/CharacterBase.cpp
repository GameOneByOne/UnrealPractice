#include "CharacterBase.h"
#include "Utils/log.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Item/ItemBase.h"

class UPaperFlipbookComponent;

namespace {
const FString CHARACTER_CONFIG_DATATABLE_REF = TEXT("/Script/Engine.DataTable'/Game/Data/DT_CharacterConfig.DT_CharacterConfig'");
}

ACharacterBase::ACharacterBase()
{
	CollisionAndInteractComponent = CreateDefaultSubobject<UBoxComponent>("CollisionAndInteractComponent");
	CollisionAndInteractComponent->SetupAttachment(RootComponent);
	CollisionAndInteractComponent->SetBoxExtent({10,10,12});
	
	GetCapsuleComponent()->SetCapsuleSize(10, 15);
	GetSprite()->SetRelativeRotation({0, 0, 270});
	
	CollisionAndInteractComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	CollisionAndInteractComponent->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	CollisionAndInteractComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionAndInteractComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	CollisionAndInteractComponent->SetGenerateOverlapEvents(true);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

bool ACharacterBase::SetCharacter(const FString& CharacterName)
{
	if (!CharacterConfigDataTable->IsValidLowLevel()) {
		// 加载配置数据表格
		CharacterConfigDataTable = LoadObject<UDataTable>(nullptr, *CHARACTER_CONFIG_DATATABLE_REF);
	}
	if (!CharacterConfigDataTable->IsValidLowLevel()) {
		ERRORLOG("[Character Base] Set Character Failed. Character Name is %s", *CharacterName);
		return false;
	}
	// 读取对应角色的配置数据
	const FName CharacterRowName = FName(CharacterName);
	FCharacterConfig *Config = CharacterConfigDataTable->FindRow<FCharacterConfig>(CharacterRowName, CharacterRowName.ToString());
	if (!Config) {
		ERRORLOG("[Character Base] SetCharacter Failed Because Of Row Name Not Found. [Name=%s]", *CharacterName);
		return false;
	}
	CharacterConfig = *Config;

	// 赋值属性
	CurrentHP = CharacterConfig.MaxHP;
	CurrentAttackPower = CharacterConfig.AttackPower;
	CurrentDefensePower = CharacterConfig.DefensePower;
	GetSprite()->SetFlipbook(CharacterConfig.IdleFlipbook);
	return true;
}

bool ACharacterBase::SetRandomCharacterWithLevel(int32 Level)
{
	if (!CharacterConfigDataTable->IsValidLowLevel()) {
		// 加载配置数据表格
		CharacterConfigDataTable = LoadObject<UDataTable>(nullptr, *CHARACTER_CONFIG_DATATABLE_REF);
	}
	if (!CharacterConfigDataTable->IsValidLowLevel()) {
		ERRORLOG("[Character Base] Set Character Failed. Character Level is %d", Level);
		return false;
	}
	// 读取对应角色等级的配置数据
	TArray<FCharacterConfig*> DataRows;
	CharacterConfigDataTable->GetAllRows<FCharacterConfig>("GetAllRowsForLevel", DataRows);
	TArray<FCharacterConfig*> WaittingSelectRows;
	for (const auto &Row : DataRows) {
		if (Row->CharacterLevel == Level) {
			WaittingSelectRows.Push(Row);
		}
	}
	if (WaittingSelectRows.Num() == 0) {
		return false;
	}

	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();
	int32 Result = RandomStream.RandRange(0, WaittingSelectRows.Num() - 1);
	CharacterConfig = *WaittingSelectRows[Result];
	CurrentHP = CharacterConfig.MaxHP;
	CurrentAttackPower = CharacterConfig.AttackPower;
	CurrentDefensePower = CharacterConfig.DefensePower;
	GetSprite()->SetFlipbook(CharacterConfig.IdleFlipbook);
	return true;
}

void ACharacterBase::BeginInteract(TObjectPtr<ACharacterBase> ActorPtr)
{
	return;
}

void ACharacterBase::Attack(ACharacterBase* Opponent) const
{
	Opponent->CurrentHP = FMath::Clamp(Opponent->CurrentHP - this->CurrentAttackPower, 0, Opponent->CurrentHP);
}

void ACharacterBase::RecoverHP(int32 HP)
{
	CurrentHP = FMath::Clamp(CurrentHP + HP, 0, CharacterConfig.MaxHP);
}

void ACharacterBase::EquipWeapon(TObjectPtr<UItemBase> WeaponObj)
{
	// 先卸掉原来的装备
	if (Weapon->IsValidLowLevel()) {
		CurrentAttackPower -= Weapon->ItemConfig.AttackBuff;
	}
	Weapon = WeaponObj;
	CurrentAttackPower += WeaponObj->ItemConfig.AttackBuff;
	return;
}

void ACharacterBase::EquipArmor(TObjectPtr<UItemBase> ArmorObj)
{
	// 先卸掉原来的装备
	if (Armor->IsValidLowLevel()) {
		CurrentAttackPower -= Armor->ItemConfig.AttackBuff;
	}
	Armor = ArmorObj;
	CurrentDefensePower += ArmorObj->ItemConfig.DefenseBuff;
	return;
}

bool ACharacterBase::IsDead() const
{
	return CurrentHP == 0;	
}

void ACharacterBase::PreDestroy(TObjectPtr<ACharacterBase> ActorObj)
{
	return;
}
