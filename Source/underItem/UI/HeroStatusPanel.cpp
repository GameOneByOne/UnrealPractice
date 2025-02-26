#include "UI/HeroStatusPanel.h"

#include "PaperSprite.h"
#include "Utils/log.h"

namespace {
    const FString DEFAULT_SLOT_IMAGE = "/Script/Engine.Texture2D'/Game/UI/Texture/ItemBox_24x24.ItemBox_24x24'";
    const FVector2f SLOT_SIZE = {38, 38};
}

bool UHeroStatusPanel::Initialize()
{
    if (!Super::Initialize()) {
        return false;
    }

    static UTexture2D* LoadedTexture = LoadObject<UTexture2D>(nullptr, *DEFAULT_SLOT_IMAGE);
    if (LoadedTexture->IsValidLowLevel()) {
        DefaultSlotImage = LoadedTexture;
    }

    HeroHPBar->BrushDelegate.BindDynamic(this, &UHeroStatusPanel::GetHeroHP);
    HeroAttackText->TextDelegate.BindDynamic(this, &UHeroStatusPanel::GetHeroAttack);
    HeroDefenseText->TextDelegate.BindDynamic(this, &UHeroStatusPanel::GetHeroDefense);
    WeaponSlot->BrushDelegate.BindDynamic(this, &UHeroStatusPanel::GetWeaponImage);
    ArmorSlot->BrushDelegate.BindDynamic(this, &UHeroStatusPanel::GetArmorImage);
    return true;
}

FSlateBrush UHeroStatusPanel::GetHeroHP()
{
    FSlateBrush HPBrush = HeroHPBar->GetBrush();
    FSlateBrush HPBrushBorder = HeroHpBarBorder->GetBrush();
    if (!HeroObj->IsValidLowLevel()) {
        HPBrush.SetImageSize({HPBrushBorder.GetImageSize().X - 20, HPBrush.GetImageSize().Y});
    } else {
        float HPPercentage = (float)(HeroObj->CurrentHP) / HeroObj->CharacterConfig.MaxHP;
        HPBrush.SetImageSize({HPBrushBorder.GetImageSize().X * HPPercentage - 20, HPBrush.GetImageSize().Y});
    }
    return HPBrush;
}

FText UHeroStatusPanel::GetHeroAttack()
{
    if (HeroObj->IsValidLowLevel()) {
        return FText::AsNumber(HeroObj->CurrentAttackPower);
    }
    return FText::FromString("");
}

FText UHeroStatusPanel::GetHeroDefense()
{
    if (HeroObj->IsValidLowLevel()) {
        return FText::AsNumber(HeroObj->CurrentDefensePower);
    }
    return FText::FromString("");
}

void UHeroStatusPanel::SetHeroPtr(TObjectPtr<AHero> Hero)
{
    HeroObj = Hero;
}

FSlateBrush UHeroStatusPanel::GetWeaponImage()
{
    FSlateBrush Brush;
    Brush.SetImageSize(SLOT_SIZE);
    if (!HeroObj->IsValidLowLevel() || !HeroObj->Weapon->IsValidLowLevel()) {
        Brush.SetResourceObject(DefaultSlotImage);
    } else {
        TObjectPtr<UTexture2D> Texture = HeroObj->Weapon->ItemConfig.Sprite->GetSourceTexture();
        Brush.SetResourceObject(Texture);
        const FVector2d UVSize = HeroObj->Weapon->ItemConfig.Sprite->GetSourceSize();
        const FVector2d UV = HeroObj->Weapon->ItemConfig.Sprite->GetSourceUV();
        const int32 TextureWidth = Texture->GetSizeX();
        const int32 TextureHeight = Texture->GetSizeY();
        FVector2D UVMin(UV.X / static_cast<float>(TextureWidth), UV.Y / static_cast<float>(TextureHeight));
        FVector2D UVMax((UV.X + UVSize.X) / static_cast<float>(TextureWidth), (UV.Y + UVSize.Y) / static_cast<float>(TextureHeight));
        Brush.SetUVRegion(FBox2D(UVMin, UVMax));
    }
    return Brush;
}

FSlateBrush UHeroStatusPanel::GetArmorImage()
{
    FSlateBrush Brush;
    Brush.SetImageSize(SLOT_SIZE);
    if (!HeroObj->IsValidLowLevel() || !HeroObj->Armor->IsValidLowLevel()) {
        Brush.SetResourceObject(DefaultSlotImage);
    } else {
        TObjectPtr<UTexture2D> Texture = HeroObj->Armor->ItemConfig.Sprite->GetSourceTexture();
        Brush.SetResourceObject(Texture);
        const FVector2d UVSize = HeroObj->Armor->ItemConfig.Sprite->GetSourceSize();
        const FVector2d UV = HeroObj->Armor->ItemConfig.Sprite->GetSourceUV();
        const int32 TextureWidth = Texture->GetSizeX();
        const int32 TextureHeight = Texture->GetSizeY();
        FVector2D UVMin(UV.X / static_cast<float>(TextureWidth), UV.Y / static_cast<float>(TextureHeight));
        FVector2D UVMax((UV.X + UVSize.X) / static_cast<float>(TextureWidth), (UV.Y + UVSize.Y) / static_cast<float>(TextureHeight));
        Brush.SetUVRegion(FBox2D(UVMin, UVMax));
    }
    return Brush;
}
