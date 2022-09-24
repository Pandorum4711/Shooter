// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


UENUM(BlueprintType)
enum class EItemState : uint8
{
    EIS_Pickup UMETA(DisplayName = "Pickup"),
    EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
    EIS_PickedUp UMETA(DisplayName = "PickedUp"),
    EIS_Equipped UMETA(DisplayName = "Equipped"),
    EIS_Falling UMETA(DisplayName = "Falling"),
    EIS_MAX UMETA(DisplayName = "DefaultMax"),
};


UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void SetItemProperties(EItemState State);

    void FinishInterping();

    void ItemInterp(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

    /* Skeletal Mesh for the item */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    /* Line trace collides with box to show HUD widgets */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CollisionBox;

    /* Popup widget for the item properties */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* PickupWidget;

    /* Enables item tracing when overlapped */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* AreaSphere;

    /* Name which appears on the Pickup Widget */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FString ItemName;

    /* Counter which appears on the Pickup Widget */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    int32 ItemCount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemState ItemState;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UCurveFloat* ItemZCurve;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FVector ItemInterpStartLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FVector CameraTargetLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    bool bInterping;

    FTimerHandle ItemInterpTimer;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class AShooterCharacter* Character;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float ZCurveTime;

    float ItemInterpX;
    float ItemInterpY;

    float InterpInitialYawOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UCurveFloat* ItemScaleCurve;
 
public:
    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
    FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
    FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
    FORCEINLINE EItemState GetItemState() const { return ItemState; }
    void SetItemState(EItemState State);
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
    void StartItemCurve(AShooterCharacter* Char);
};
